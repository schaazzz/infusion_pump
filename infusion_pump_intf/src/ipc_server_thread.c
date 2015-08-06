/*
 * File    - ipc_server_thread.c
 * Process - Infusion Pump Firmware
 * Module  - Core Process
 * Author  - Shahzeb Ihsan
 */

/*
 * Standard header files
 */
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sched.h>
#include <sys/siginfo.h>
#include <sys/dispatch.h>
#include <unistd.h>

/*
 * Local header files
 */
#include "ablibs.h"
#include "abimport.h"
#include "proto.h"
#define __USE_STDINT       (1)
#include "utility.h"

/*
 * External variables
 */
extern syringe_t sel_syringe;

/*
 * Global scope variables
 */
uint32_t core_pid = 0;
msg_intf_t op_global_msg;
uint32_t time_to_start = 0;
uint32_t operation = __OP_IDLE;
pthread_mutex_t operation_mutex = PTHREAD_MUTEX_INITIALIZER;

/*
 * Local scope variables
 */
static name_attach_t * name_attr;

/*
 * Function     - operation_stop()
 *
 * Arguments    - <lock_photon> TRUE if the Photon library should be locked
 *
 * Return Value - None
 */
void operation_stop(bool_t lock_photon)
{
   sint32_t pt_flags;

   pt_flags = PtEnter(0);
   if((pt_flags >= 0) || lock_photon)
   {
      PtSetResource(ABW_btn_start_inf, Pt_ARG_FLAGS, 0, Pt_BLOCKED);
      kill(core_pid, __SIG_INTF_STOP);
      PtLeave(pt_flags);
   }
}

/*
 * Function     - operation_start()
 *
 * Arguments    - <lock_photon> TRUE if the Photon library should be locked
 *
 * Return Value - None
 */
void operation_start(bool_t lock_photon)
{
   sint32_t pt_flags;

   pt_flags = PtEnter(0);
   if((pt_flags >= 0) || lock_photon)
   {
      PtSetResource(ABW_btn_start_inf, Pt_ARG_FLAGS, Pt_BLOCKED, Pt_BLOCKED);
      PtLeave(pt_flags);
   }
}

/*
 * Function     - __check_sensor_errors()
 *
 * Arguments    - <flags> Sensor flags
 *                <batt_percentage> Battery percentage
 *
 * Return Value - Sensor errors of the type sensor_error_t
 */
sensor_error_t __check_sensor_errors(uint32_t flags, uint32_t batt_percentage)
{
   // Errors checking policy and order of processing...
   // - Air-in-Line sensor
   // ... If set, Filling will continue with a warning but Infusion will not
   // ... start, if Infusion is already in progress, it is immiediately terminated
   //
   // - Lid sensor
   // ... Infusion/Filling will not start if set, if either operation is
   // ... already in progress, it will immediately be terminated
   //
   // - Battery sensor
   // ... If the battery is not in use, the battery low sensor is ignored, if the
   // ... battery is in use and battery is low, Infusion/Filling won't start, if
   // ... either operation is already in progress, a warning message is displayed
   //
   // - Temperature sensor
   // ... Warning if temperature reaches 5% of maximum, neither of Filling/Infusion
   // ... will start, if already in progress, a warning is displayed
   // ...
   // ... If the temperature reaches the maximum, both Infusion and Filling are
   // ... terminated
   //
   // - Pressure sensor
   // ... Same policy as the temperature sensor

   if(flags & __FLG_AIRINLINE_SENSOR)
   {
      return (__ERROR_AIRINLINE);
   }

   if(flags & __FLG_LID_SENSOR)
   {
      return (__ERROR_LID);
   }

   if((flags & __FLG_BATTERY_LOW_SENSOR) &&
      (flags & __FLG_BATTERY_USE_SENSOR))
   {
      return (__ERROR_BATTERY);
   }

   if(flags & __FLG_TEMP_PANIC_SENSOR)
   {
      return (__ERROR_TEMPERATURE_PANIC);
   }

   if(flags & __FLG_TEMP_HIGH_SENSOR)
   {
      return (__ERROR_TEMPERATURE_HIGH);
   }

   if(flags & __FLG_PRESSURE_PANIC_SENSOR)
   {
      return (__ERROR_PRESSURE_PANIC);
   }

   if(flags & __FLG_PRESSURE_HIGH_SENSOR)
   {
      return (__ERROR_PRESSURE_HIGH);
   }

   return (__ERROR_NONE);
}

/*
 * Function     - __ipc_server_setup()
 *
 * Arguments    - None
 *
 * Return Value - None
 */
static __inline void __ipc_server_setup(void)
{
   // Register server name and create channel
   name_attr = name_attach(NULL, __IPC_SERV_NAME, 0);
   if (NULL == name_attr)
   {
       perror("Registering server name\n");
       exit(EXIT_FAILURE);
   }
}

/*
 * Function     - ipc_server_thread()
 *
 * Arguments    - <args> Pointer to this thread's arguments
 *
 * Return Value - Pointer to the return value
 */
void * ipc_server_thread(void * args)
{
   msg_core_t sensor_msg;
   msg_intf_t operation_msg;
   struct _msg_info msg_info;
   bool_t display_error = FALSE;
   sint32_t rcv_id, client_id = 0, pt_flags, error, reply;
   sensor_error_t sensor_errors = __ERROR_NONE, last_error = __ERROR_NONE;
   char batt_str[15], status_str[30], progress_str[30], usr_msg_title[20], usr_msg_str[75];

   // Setup the IPC server thread
   __ipc_server_setup();

   // Setup default operation to idle
   operation_msg.msg.operation = __OP_IDLE;

   // Wait for messages from the client
   for(;;)
   {
      // Wait to receive a message from the client
      rcv_id = MsgReceive(name_attr->chid, &sensor_msg, sizeof(msg_core_t), &msg_info);
      if(-1 == rcv_id)
      {
         perror("Message receive failed\n");
         continue;
      }

      // System pulse received
      if(0 == rcv_id)
      {
         // Handle system pulses
         switch(sensor_msg.pulse.code)
         {
            // System disconnection pulse
            case (_PULSE_CODE_DISCONNECT):
               // A client has disconnected, verify that it is the sensor client
               if(sensor_msg.pulse.scoid == client_id)
               {
                  client_id = 0;
               }

               ConnectDetach(sensor_msg.pulse.scoid);
               break;

            default:
               break;
         }
         continue;
      }

      // Not an error, not a pulse, therefore a message
      // Reply to _IO_CONNECT
      if (sensor_msg.msg.type == _IO_CONNECT)
      {
         MsgReply(rcv_id, EOK, NULL, 0);
         continue;
      }

      // Unexpected system message
      if ((sensor_msg.msg.type > _IO_BASE) &&
          (sensor_msg.msg.type <= _IO_MAX))
      {
         MsgError(rcv_id, ENOSYS);
         continue;
      }

      // Sensor client messages
      switch(sensor_msg.msg.type)
      {
         case (__IPC_MSG_REGISTER):
            if(client_id)
            {
               MsgError(rcv_id, EBUSY);
            }
            else
            {
               client_id = rcv_id;
               MsgReply(rcv_id, 0, NULL, 0);
               core_pid = sys_getcorepid();
            }
            break;

         case (__IPC_MSG_SENSOR):
            error = pthread_mutex_trylock(&operation_mutex);
            if(0 == error)
            {
               // Copy operation data
               memcpy(&operation_msg, &op_global_msg, sizeof(msg_intf_t));

               // Reset operation data
               memset(&op_global_msg, 0x00, sizeof(msg_intf_t));

               pthread_mutex_unlock(&operation_mutex);
            }

            // Check for sensor errors
            sensor_errors = __check_sensor_errors(sensor_msg.msg.flags, sensor_msg.msg.batt_percentage);

            // Condition: No operation in progress, operation triggered
            if((__OP_IDLE == sensor_msg.msg.status) &&
               (__OP_IDLE != operation_msg.msg.operation))
            {
               display_error = FALSE;

               if(__ERROR_LID == sensor_errors)
               {
                  operation_msg.msg.operation =__OP_IDLE;
                  strcpy(usr_msg_title, "Error");
                  strcpy(usr_msg_str, "Pump lid open, cannot start operation!");
                  display_error = TRUE;
               }
               else if(__ERROR_BATTERY == sensor_errors)
               {
                  operation_msg.msg.operation =__OP_IDLE;
                  strcpy(usr_msg_title, "Error");
                  strcpy(usr_msg_str, "Battery too low, cannot start operation!");
                  display_error = TRUE;
               }
               else if(__ERROR_TEMPERATURE_PANIC == sensor_errors)
               {
                  operation_msg.msg.operation =__OP_IDLE;
                  strcpy(usr_msg_title, "Error");
                  strcpy(usr_msg_str, "Temperature too high, cannot start operation!");
                  display_error = TRUE;
               }
               else if(__ERROR_PRESSURE_PANIC == sensor_errors)
               {
                  operation_msg.msg.operation =__OP_IDLE;
                  strcpy(usr_msg_title, "Error");
                  strcpy(usr_msg_str, "Pressure too high, cannot start operation!");
                  display_error = TRUE;
               }
               else if(__ERROR_AIRINLINE == sensor_errors)
               {
                  if(__OP_START_INFUSION == operation_msg.msg.operation)
                  {
                     operation_msg.msg.operation =__OP_IDLE;
                     strcpy(usr_msg_title, "Error");
                     strcpy(usr_msg_str, "Air bubble detected, cannot start infusion!");
                  }
                  else if(__OP_START_FILLING == operation_msg.msg.operation)
                  {
                     strcpy(usr_msg_title, "Warning");
                     strcpy(usr_msg_str, "Air bubble detected, please check the line!");
                  }

                  display_error = TRUE;
               }
            }
            // Condition: operation in progress
            else if((__OP_IDLE != sensor_msg.msg.status) &&
                    (__OP_COMPLETE != sensor_msg.msg.status) &&
                    (sensor_errors != last_error))
            {
               display_error = FALSE;

               if(__ERROR_AIRINLINE == sensor_errors)
               {
                  if(__OP_START_INFUSION == sensor_msg.msg.status)
                  {
                     operation = __OP_IDLE;
                     operation_msg.msg.operation =__OP_IDLE;
                     operation_stop(FALSE);
                     strcpy(usr_msg_title, "Error");
                     strcpy(usr_msg_str, "Air bubble detected, terminating infusion!");
                  }
                  else if(__OP_START_FILLING == sensor_msg.msg.status)
                  {
                     strcpy(usr_msg_title, "Warning");
                     strcpy(usr_msg_str, "Air bubble detected, please check the line!");
                  }

                  display_error = TRUE;
               }
               else if(__ERROR_BATTERY == sensor_errors)
               {
                  strcpy(usr_msg_title, "Warning");
                  strcpy(usr_msg_str, "Battery too low, please connect to a power outlet!");
                  display_error = TRUE;
               }
               else if(__ERROR_LID == sensor_errors)
               {
                  operation = __OP_IDLE;
                  operation_msg.msg.operation =__OP_IDLE;
                  operation_stop(FALSE);
                  strcpy(usr_msg_title, "Error");
                  strcpy(usr_msg_str, "Pump lid open, terminating operation!");
                  display_error = TRUE;
               }
               else if(__ERROR_TEMPERATURE_PANIC == sensor_errors)
               {
                  operation = __OP_IDLE;
                  operation_msg.msg.operation =__OP_IDLE;
                  operation_stop(FALSE);
                  strcpy(usr_msg_title, "Error");
                  strcpy(usr_msg_str, "Temperature too high, terminating infusion!");
                  display_error = TRUE;
               }
               else if(__ERROR_PRESSURE_PANIC == sensor_errors)
               {
                  operation = __OP_IDLE;
                  operation_msg.msg.operation =__OP_IDLE;
                  operation_stop(FALSE);
                  strcpy(usr_msg_title, "Error");
                  strcpy(usr_msg_str, "Pressure too high, terminating infusion!");
                  display_error = TRUE;
               }

               if(display_error)
               {
                  last_error = sensor_errors;
               }
            }

            if(display_error)
            {
               display_error = FALSE;
               pt_flags = PtEnter(0);
               if(pt_flags >= 0)
               {
                  PtNotice(ABW_base, NULL, usr_msg_title, NULL, usr_msg_str,  NULL, "Ok", NULL, Pt_BLOCK_PARENT);
                  PtLeave(pt_flags);
               }
            }

            // Setup reply value
            if((operation_msg.msg.operation == __OP_START_FILLING) ||
               (operation_msg.msg.operation == __OP_START_INFUSION))
            {
               reply = __REPLY_NEW_OP;
               operation = operation_msg.msg.operation;
               operation_start(FALSE);
            }
            else
               reply = __REPLY_NO_OP;

            // Reply to the sensor message and reset
            MsgReply(rcv_id, reply, &(operation_msg), sizeof(msg_intf_t));

            // Setup battery, volume transferred and status strings
            snprintf(batt_str, sizeof(batt_str), "Battery: %d%%", sensor_msg.msg.batt_percentage);

            if(__OP_IDLE == sensor_msg.msg.status)
               snprintf(status_str, sizeof(status_str), "Status: %s", "Idle");
            else if(__OP_START_FILLING == sensor_msg.msg.status)
               snprintf(status_str, sizeof(status_str), "Status: %s", "Filling");
            else if(__OP_START_INFUSION == sensor_msg.msg.status)
               snprintf(status_str, sizeof(status_str), "Status: %s", "Infusion");

            snprintf(progress_str, sizeof(progress_str), "Volume Transferred: %d%%", sensor_msg.msg.progress);

            // Update status and battery labels
            pt_flags = PtEnter(0);
            if(pt_flags >= 0)
            {
               // Set battery label color  to red if the percentage is
               // less than 5%
               if(5 > sensor_msg.msg.batt_percentage)
                  PtSetResource (ABW_lbl_battery, Pt_ARG_COLOR, Pg_RED, 0);
               else
                  PtSetResource (ABW_lbl_battery, Pt_ARG_COLOR, Pg_BLACK, 0);

               wgt_settext(ABN_lbl_battery, batt_str);
               wgt_settext(ABN_lbl_status, status_str);
               wgt_settext(ABN_lbl_vol_xfrd, progress_str);

               if(__OP_COMPLETE  == sensor_msg.msg.status)
               {
                  PtNotice(ABW_base, NULL, "Notification", NULL, "Transfer complete!",  NULL, "Ok", NULL, Pt_BLOCK_PARENT);
                  operation_stop(TRUE);
               }

               PtLeave(pt_flags);
            }
            break;

         default:
            // Some other unexpected message
            MsgError(rcv_id, ENOSYS);
            break;
      }
   }

   return ((void *)NULL);
}
