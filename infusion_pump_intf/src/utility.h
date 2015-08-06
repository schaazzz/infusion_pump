/*
 * File    - utility.h
 * Process - Infusion Pump Firmware
 * Module  - Interface Process
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
#define __TAB_INFUSION              (0)
#define __TAB_FILL                  (1)
#define __TAB_CONFIG                (2)
#define __TAB_DIAG                  (3)

#define __MODE_INF_CONT             (0)
#define __MODE_INF_PCA              (1)

#define __PIN_NUM_DIGITS            (4)

#define __PRIO_UI_THREAD            (13)
#define __PRIO_NW_THREAD            (10)
#define __PRIO_IPC_SERVER_THREAD    (10)

#define __FLG_LID_SENSOR            (1 << 0)
#define __FLG_PRESSURE_HIGH_SENSOR  (1 << 1)
#define __FLG_PRESSURE_PANIC_SENSOR (1 << 2)
#define __FLG_TEMP_HIGH_SENSOR      (1 << 3)
#define __FLG_TEMP_PANIC_SENSOR     (1 << 4)
#define __FLG_BATTERY_USE_SENSOR    (1 << 5)
#define __FLG_BATTERY_LOW_SENSOR    (1 << 6)
#define __FLG_AIRINLINE_SENSOR      (1 << 7)

#define __IPC_SERV_NAME             ("__INFUSION_IPC__")

#if defined (__X86__)
#define __PROC_CORE_NAME            ("usr/bin/infusion_pump_core")
#endif   // #if defined (__X86__)

#if defined (__ARM__)
#define __PROC_CORE_NAME            ("infusion_pump_core")
#endif   // #if defined (__ARM__)

#define __IPC_MSG_REGISTER          (1)
#define __IPC_MSG_SENSOR            (2)

#define __OP_IDLE                   (0)
#define __OP_START_INFUSION         (1)
#define __OP_START_FILLING          (2)
#define __OP_COMPLETE               (3)

#define __REPLY_NO_OP               (0)
#define __REPLY_NEW_OP              (1)

#define __NW_CMD_ID_INFUSION        (3)

#define __NW_RESP_SUCCESS           (0x00)
#define __NW_RESP_INVALID_CMD       (0x01)
#define __NW_DEVICE_BUSY            (0x02)
#define __NW_RESP_FAIL              (0xFF)

#define __SIG_INTF_STOP             (SIGRTMIN)

/*
 * Data type definitions
 */
#if (__USE_STDINT == 0)
typedef unsigned char uint8_t;               // 8-bit unsigned data type
typedef unsigned short uint16_t;             // 16-bit unsigned data type
typedef unsigned int uint32_t;               // 32-bit unsigned data type
#endif   // #if (__USE_STDINT == 0)

typedef char sint8_t;                        // 8-bit unsigned data type
typedef short sint16_t;                      // 16-bit unsigned data type
typedef int sint32_t;                        // 32-bit unsigned data type

typedef enum                                 // Boolean data type
{
   FALSE,
   TRUE
}
bool_t;

/*
 * Infusion mode structure
 */
typedef struct
{
   char param1[30];
   char param2[30];
   char param3[30];
}
inf_mode_t;

/*
 * Syringe data structure
 */
typedef struct
{
   bool_t selected;
   char syringe[20];
   float vol_ml;
   float dia_mm;
   float len_mm;
   float ml_per_mm;
   float min_ml_per_min;
   float max_ml_per_min;
   float max_press;
}
syringe_t;

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
 * Sensor errors
 */
typedef enum
{
   __ERROR_NONE = 0,
   __ERROR_BATTERY = -1,
   __ERROR_AIRINLINE = -2,
   __ERROR_PRESSURE_HIGH = -3,
   __ERROR_PRESSURE_PANIC = -4,
   __ERROR_TEMPERATURE_HIGH = -5,
   __ERROR_TEMPERATURE_PANIC = -6,
   __ERROR_LID = -7,
}
sensor_error_t;

/*
 * Function prototypes - Threads
 */
void * ui_thread(void * args);
void * nw_thread(void * args);
void * ipc_server_thread(void * args);

/*
 * Function prototypes - Configuration API
 */
sint32_t cfg_getfield(const char * field, char * value, uint32_t size);
sint32_t cfg_setfield(const char * field, const char * value);

/*
 * Function prototypes - Widget API
 */
void wgt_setup(void);
void wgt_clearlog(void);
void wgt_blocktab(uint32_t tab);
void wgt_unblocktab(uint32_t tab);
void wgt_insertlog(const char * str);
void wgt_select_inf_mode(uint32_t mode);
sint32_t wgt_gettext(const sint32_t widget_name, char * str, uint32_t length);
void wgt_settext(const sint32_t widget_name, const char * str);
void wgt_selectsyringe(uint32_t index);

/*
 * Function prototypes - Network API
 */
bool_t network_connect(sint32_t * server_sock, sint32_t * client_sock, char * server_ip, uint16_t server_port, bool_t server);
sint32_t network_send(sint32_t * sock_desc, void * data_buff, uint32_t data_len);
sint32_t network_recv(sint32_t * sock_desc, void * data_buff, uint32_t data_len);
sint32_t network_close(sint32_t * sock_desc);

bool_t udp_setup(sint32_t * sock_desc, char * server_ip, uint16_t server_port, bool_t server);
sint32_t udp_receive(sint32_t * sock_desc, void * data_buff, uint32_t data_len);
sint32_t udp_send(sint32_t * sock_desc, void * data_buff, uint32_t data_len);

/*
 * Function prototypes - System API
 */
sint32_t sys_getcorepid(void);
void sys_setip(char * ip);

/*
 * Function prototypes - Operation API
 */
void operation_start(bool_t lock_photon);
void operation_stop(bool_t lock_photon);

#endif /* UTILITY_H_ */
