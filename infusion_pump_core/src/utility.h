/*
 * File    - utility.h
 * Process - Infusion Pump Firmware
 * Module  - Core Process
 * Author  - Shahzeb Ihsan
 */

#ifndef UTILITY_H_
#define UTILITY_H_

/*
 * Standard header files
 */
#include <sys/neutrino.h>

/*
 * Configuration flags
 */
#define __SIMULATOR_UDP             (0)

#define __SIMULATOR_SERIAL          (1)

#define __SERIAL_BAUDRATE           (115200)

#define __SENSOR_POLL_DELAY_MS      (750)

#define __PRIO_SENSOR_THREAD        (10)
#define __PRIO_IPC_CLIENT_THREAD    (10)
#define __PRIO_OPERATION_THREAD     (10)

#define __MIN_SENSOR_BYTES          (2)
#define __SENSOR_DATA_PKT_LEN       (14)

#define __FLG_LID_SENSOR            (1 << 0)
#define __FLG_PRESSURE_HIGH_SENSOR  (1 << 1)
#define __FLG_PRESSURE_PANIC_SENSOR (1 << 2)
#define __FLG_TEMP_HIGH_SENSOR      (1 << 3)
#define __FLG_TEMP_PANIC_SENSOR     (1 << 4)
#define __FLG_BATTERY_USE_SENSOR    (1 << 5)
#define __FLG_BATTERY_LOW_SENSOR    (1 << 6)
#define __FLG_AIRINLINE_SENSOR      (1 << 7)

#define __IPC_SERV_NAME             ("__INFUSION_IPC__")

#define __IPC_MSG_REGISTER          (1)
#define __IPC_MSG_SENSOR            (2)

#define __OP_IDLE                   (0)
#define __OP_START_INFUSION         (1)
#define __OP_START_FILLING          (2)
#define __OP_COMPLETE               (3)

#define __REPLY_NO_OP               (0)
#define __REPLY_NEW_OP              (1)

#define __TIMER_PULSE_CODE          (_PULSE_CODE_MINAVAIL)

#define __TIMER_TICK_NS             (1000000)

#define __SIG_INTF_STOP             (SIGRTMIN)
#define __SIG_SETUP_OP              (__SIG_INTF_STOP + 1)
#define __SIG_PWM_TICK              (__SIG_SETUP_OP + 1)

#define __MAX_NUM_STEPS             (1000)

/*
 * Data type definitions
 */
typedef unsigned char uint8_t;               // 8-bit unsigned data type
typedef unsigned short uint16_t;             // 16-bit unsigned data type
typedef unsigned int uint32_t;               // 32-bit unsigned data type

typedef char sint8_t;                        // 8-bit unsigned data type
typedef short sint16_t;                      // 16-bit unsigned data type
typedef int sint32_t;                        // 32-bit unsigned data type

typedef enum                                 // Boolean data type
{
   FALSE,
   TRUE
}bool_t;

/*
 * Sensor data structure
 */
typedef struct
{
   bool_t new_data;
   uint8_t flags;
   sint16_t batt_percentage;
}
sensor_data_t;

/*
 * Core IPC structure
 */
typedef union
{
   struct _pulse pulse;
   struct
   {
      uint32_t type;
      uint32_t flags;
      uint32_t batt_percentage;
      uint32_t status;
      uint32_t progress;
   }
   msg;
}
msg_core_t;

/*
 * Interface IPC structure
 */
typedef union
{
   struct _pulse pulse;
   struct
   {
      uint32_t type;
      uint32_t vol;
      uint32_t rate;
      float min_rate;
      uint32_t max_press;
      uint32_t operation;
   }
   msg;
}
msg_intf_t;

/*
 * Function prototypes - Threads
 */
void * sensor_thread(void * args);
void * operation_thread(void * args);
void * ipc_client_thread(void * args);
void * ipc_signal_thread(void * args);

/*
 * Function prototypes - Network API
 */
bool_t network_connect(sint32_t * server_sock, sint32_t * client_sock, char * server_ip, uint16_t server_port, bool_t server);
sint32_t network_send(sint32_t * sock_desc, void * data_buff, uint32_t data_len);
sint32_t network_close(sint32_t * sock_desc);

bool_t udp_setup(sint32_t * sock_desc, char * server_ip, uint16_t server_port, bool_t server);
sint32_t udp_receive(sint32_t * sock_desc, void * data_buff, uint32_t data_len);
sint32_t udp_send(sint32_t * sock_desc, void * data_buff, uint32_t data_len);

/*
 * Function prototypes - Configuration API
 */
uint16_t cfg_getsensorport(void);
char * cfg_gethostip(void);

#endif /* UTILITY_H_ */
