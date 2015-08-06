/*
 * File    - sensor_thread.c
 * Process - Infusion Pump Firmware
 * Module  - Core Process
 * Author  - Shahzeb Ihsan
 */

/*
 * Standard header files
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

/*
 * Local header files
 */
#include "utility.h"

/*
 * Global scope variables
 */
pthread_mutex_t sensor_mutex = PTHREAD_MUTEX_INITIALIZER;
sensor_data_t sensor_data = {FALSE, 0, -1};

/*
 * Sensor type definition
 */
typedef struct
{
   uint8_t id;          // Sensor ID
   uint8_t num_bytes;   // Number of bytes in sensor data
}
sensor_type_t;

/*
 * Initialize sensor types and their data length
 */
static const sensor_type_t sensor_lid = {0x10, 1};
static const sensor_type_t sensor_pressure = {0x20, 2};
static const sensor_type_t sensor_temp = {0x30, 2};
static const sensor_type_t sensor_battery = {0x40, 3};
static const sensor_type_t sensor_airinline = {0x50, 1};

/*
 * Local scope variables
 */
#if(__SIMULATOR_UDP == 1)
static sint32_t sensor_sock = NULL;
#endif   // #if(__VIRTUAL_MACHINE == 1)

#if(__SIMULATOR_SERIAL == 1)
sint32_t serial_fd = -1;
#endif   // #if(__SIMULATOR_SERIAL == 1)

#if((__SIMULATOR_SERIAL == 1) && (__SIMULATOR_UDP == 1))
#error "Both __SIMULATOR_SERIAL and __SIMULATOR_UDP cannot be set to 1"
#endif   // #if((__SIMULATOR_SERIAL == 1) && (__SIMULATOR_UDP == 1))

static struct
{
  uint8_t data[256];
  uint32_t head;
  uint32_t tail;
}
sensor_buff;

/*
 * Macro definitions
 */
#define M_RD_INDEX(_index)             ((sensor_buff.tail + _index) % sizeof(sensor_buff.data))

#define M_WR_INDEX(_index)             ((sensor_buff.head + _index) % sizeof(sensor_buff.data))

#define M_INC_RD_INDEX(_count)         sensor_buff.tail = ((sensor_buff.tail + _count) % sizeof(sensor_buff.data))

#define M_INC_WR_INDEX(_count)         sensor_buff.head = ((sensor_buff.head + _count) % sizeof(sensor_buff.data))

#define M_COPY_TO_INDEX(_buff, _count)                         \
{                                                              \
   uint32_t index;                                             \
   for(index = 0; index < _count; index++)                     \
   {                                                           \
      sensor_buff.data[M_WR_INDEX(index)] = _buff[index];      \
   }                                                           \
   M_INC_WR_INDEX(_count);                                     \
}

/*
 * Function     - __parse_sensor_data
 *
 * Arguments    - <length> Data length
 *
 * Return Value - -1 if more data is needed, 0 otherwise
 */
static __inline sint32_t __parse_sensor_data(uint32_t length)
{
   uint8_t sensor_id;
   uint32_t num_bytes;
   sint32_t ret_val = -1;;

   while(1)
   {
      if(length < __MIN_SENSOR_BYTES)
      {
         ret_val = -1;
         break;
      }

      sensor_id = sensor_buff.data[M_RD_INDEX(0)];
      num_bytes = length - 1;

      // Check if the number of data bytes available correspond to the
      // minimum data length for this sensor
      if(
         ((sensor_id == sensor_lid.id) && (num_bytes < sensor_lid.num_bytes)) ||
         ((sensor_id == sensor_pressure.id) && (num_bytes < sensor_pressure.num_bytes)) ||
         ((sensor_id == sensor_temp.id) && (num_bytes < sensor_temp.num_bytes)) ||
         ((sensor_id == sensor_battery.id) && (num_bytes < sensor_battery.num_bytes)) ||
         ((sensor_id == sensor_airinline.id) && (num_bytes < sensor_airinline.num_bytes))
        )
      {
         return (-1);
      }

      // Set new sensor data flag
      sensor_data.new_data = TRUE;

      // Check for lid sensor error
      if(sensor_id == sensor_lid.id)
      {
         // Modify the lid sensor error flag
         if(0 == sensor_buff.data[M_RD_INDEX(1)])
            sensor_data.flags |= (__FLG_LID_SENSOR);
         else
            sensor_data.flags &= ~(__FLG_LID_SENSOR);

         // Update buffer index and received length
         M_INC_RD_INDEX(sensor_lid.num_bytes + 1);
         length -= sensor_lid.num_bytes + 1;
      }
      // Check for pressure sensor error
      else if(sensor_id == sensor_pressure.id)
      {
         uint16_t value = (sensor_buff.data[M_RD_INDEX(1)] << 8) |
                          sensor_buff.data[M_RD_INDEX(2)];

         // Modify the pressure sensor error flag
         if(value > 60000)
            sensor_data.flags |= (__FLG_PRESSURE_PANIC_SENSOR);
         else
            sensor_data.flags &= ~(__FLG_PRESSURE_PANIC_SENSOR);

         // Update tail pointer and received length
         M_INC_RD_INDEX(sensor_pressure.num_bytes + 1);
         length -= sensor_pressure.num_bytes + 1;
      }
      // Check for temperature sensor error
      else if(sensor_id == sensor_temp.id)
      {
         uint16_t value = (sensor_buff.data[M_RD_INDEX(1)] << 8) |
                          sensor_buff.data[M_RD_INDEX(2)];

         // Modify the temperature sensor error flag
         if(value > 60000)
            sensor_data.flags |= (__FLG_TEMP_PANIC_SENSOR);
         else
            sensor_data.flags &= ~(__FLG_TEMP_PANIC_SENSOR);

         // Update tail pointer and received length
         M_INC_RD_INDEX(sensor_temp.num_bytes + 1);
         length -= sensor_temp.num_bytes + 1;
      }
      // Check for battery sensor error
      else if(sensor_id == sensor_battery.id)
      {
         uint16_t value = (sensor_buff.data[M_RD_INDEX(1)] << 8) |
                          sensor_buff.data[M_RD_INDEX(2)];

         // Approximate battery percentage
         sensor_data.batt_percentage = (value * 100) / 60000;

         // Modify the battery usage flag
         if(sensor_buff.data[M_RD_INDEX(3)])
            sensor_data.flags |= (__FLG_BATTERY_USE_SENSOR);
         else
            sensor_data.flags &= ~(__FLG_BATTERY_USE_SENSOR);

         // Modify the battery low error flag
         if(sensor_data.batt_percentage <= 5)
            sensor_data.flags |= (__FLG_BATTERY_LOW_SENSOR);
         else
            sensor_data.flags &= ~(__FLG_BATTERY_LOW_SENSOR);

         // Update tail pointer and received length
         M_INC_RD_INDEX(sensor_battery.num_bytes + 1);
         length -= sensor_battery.num_bytes + 1;
      }
      // Check for air-in-line sensor error
      else if(sensor_id == sensor_airinline.id)
      {
         // Modify the air-in-line sensor error flag
         if(0x22 == sensor_buff.data[M_RD_INDEX(1)])
            sensor_data.flags |= (__FLG_AIRINLINE_SENSOR);
         else
            sensor_data.flags &= ~(__FLG_AIRINLINE_SENSOR);

         // Update tail pointer and received length
         M_INC_RD_INDEX(sensor_airinline.num_bytes + 1);
         length -= sensor_airinline.num_bytes + 1;
      }
      // Unexpected data, probably the error can be handled better here!?
      else
      {
         M_INC_RD_INDEX(1);
         length--;
      }

      if(0 ==length)
      {
         ret_val = 0;
         break;
      }
   }

   return (ret_val);
}

/*
 * Function     - __sensor_setup()
 *
 * Arguments    - None
 *
 * Return Value - None
 */
static __inline void __sensor_setup(void)
{
#if(__SIMULATOR_UDP == 1)
   // Setup UDP connection
   udp_setup(&sensor_sock, NULL, cfg_getsensorport(), TRUE);
#endif   // #if(__SIMULATOR_UDP == 1)

#if(__SIMULATOR_SERIAL == 1)
   struct termios options;

   // Setup serial port
   if(-1 == (serial_fd = open("/dev/ser2", O_RDWR)))
   {
      perror("Opening /dev/ser2\n");
      exit(EXIT_FAILURE);
   }

   // Get serial port attributes
   if(tcgetattr(serial_fd, &options))
   {
      close(serial_fd);
      perror("Getting /dev/ser2 attributes\n");
      exit(EXIT_FAILURE);
   }

   // Set baud rate
   cfsetispeed(&options, __SERIAL_BAUDRATE);
   cfsetospeed(&options, __SERIAL_BAUDRATE);

   // TODO: Figure out all the serial port options
   // Set serial port options, at this time I have no idea what
   // they mean
   options.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON );

   options.c_oflag &= ~(OPOST);

   options.c_cflag &= ~CSIZE;
   options.c_cflag |= CS8;
   options.c_cflag &= ~CSTOPB;
   options.c_cflag &= ~PARENB;

   options.c_lflag &= ~(ECHO | ICANON | ISIG | ECHOE | ECHOK | ECHONL | IEXTEN);

   options.c_cc[VMIN] =  1;
   options.c_cc[VTIME] = 0;

   if(-1 == tcsetattr(serial_fd, TCSADRAIN, &options))
   {
      perror ("Setting /dev/ser2 attributes\n");
      exit(EXIT_FAILURE);
   }
#endif   // #if(__SIMULATOR_SERIAL == 1)

   // Initialize the circular buffer for sensor data
   sensor_buff.head = 0;
   sensor_buff.tail = 0;
}

/*
 * Function     - sensor_thread()
 *
 * Arguments    - <args> Pointer to this thread's arguments
 *
 * Return Value - Pointer to the return value
 */
void * sensor_thread(void * args)
{
   sint32_t error = 0, rcv_len;
   uint8_t temp_buff[__SENSOR_DATA_PKT_LEN];

   // Setup the sensor thread
   __sensor_setup();

   for(;;)
   {
#if(__SIMULATOR_UDP == 1)
      // Get sensor data over UDP
      rcv_len = udp_receive(&sensor_sock, temp_buff, __SENSOR_DATA_PKT_LEN);
#endif   // #if(__SIMULATOR_UDP == 1)

#if(__SIMULATOR_SERIAL == 1)
      // Get sensor data over the serial port
      rcv_len = read(serial_fd, temp_buff, __SENSOR_DATA_PKT_LEN);
#endif   // #if(__SIMULATOR_SERIAL == 1)

      if(rcv_len < 0)
      {
         // TODO: Handle error here...
      }
      else
      {
         M_COPY_TO_INDEX(temp_buff, rcv_len);
         if(0 == pthread_mutex_lock(&sensor_mutex))
         {
            error = __parse_sensor_data(rcv_len);
            pthread_mutex_unlock(&sensor_mutex);
         }
      }

      delay(__SENSOR_POLL_DELAY_MS);
   }

   return ((void *)NULL);
}
