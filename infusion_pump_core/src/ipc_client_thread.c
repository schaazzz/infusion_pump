/*
 * File    - ipc_client_thread.c
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
#include <signal.h>

/*
 * Local header files
 */
#include "utility.h"

/*
 * External variables
 */
extern bool_t infusion;
extern sensor_data_t sensor_data;
extern pthread_mutex_t sensor_mutex;

/*
 * Global scope variables
 */
float vol;
float flow_rate;
float min_rate;
msg_intf_t operation_msg;
uint32_t operation = __OP_IDLE;
uint32_t op_percentage = 0;
pthread_mutex_t operation_mutex = PTHREAD_MUTEX_INITIALIZER;

/*
 * Local scope variables
 */
static float max_press;
static sint32_t rcv_id;
static sint32_t self_coid;
static sint32_t channel_id;
static sint32_t server_coid;
static msg_core_t sensor_msg;

/*
 * Function     - __ipc_client_setup()
 *
 * Arguments    - None
 *
 * Return Value - None
 */
static __inline void __ipc_client_setup(void)
{
   sensor_msg.msg.status = __OP_IDLE;
   sensor_msg.msg.progress = 0;

   // Setup a thread to wait for the stop signal if infusion
   // or filling is in progress
   pthread_create(NULL, NULL, ipc_signal_thread, NULL);

   // Wait for the server to connect
   do
   {
      server_coid = name_open(__IPC_SERV_NAME, 0);
      sleep(1);
   }
   while(-1 == server_coid);

   // Create channel
   channel_id = ChannelCreate(0);
   if(-1 == channel_id)
   {
      perror("Creating channel\n");
      exit(EXIT_FAILURE);
   }

   // Connect to the channel
   self_coid = ConnectAttach(0, 0, channel_id, _NTO_SIDE_CHANNEL, 0);
   if(-1 == self_coid)
   {
      perror("Connecting to the channel\n");
      exit(EXIT_FAILURE);
   }

   // Register the IPC client
   sensor_msg.msg.type = __IPC_MSG_REGISTER;
   if(-1 == MsgSend(server_coid, &sensor_msg, sizeof(msg_core_t), NULL, 0))
   {
      perror("Registering IPC client\n");
      exit(EXIT_FAILURE);
   }
}

/*
 * Function     - ipc_client_thread()
 *
 * Arguments    - <args> Pointer to this thread's arguments
 *
 * Return Value - Pointer to the return value
 */
void * ipc_client_thread(void * args)
{
   sint32_t error, reply;

   // Setup the IPC client thread
   __ipc_client_setup();

   // Send sensor data
   sensor_msg.msg.type = __IPC_MSG_SENSOR;
   for(;;)
   {
      // Read sensor data
      error = pthread_mutex_trylock(&sensor_mutex);
      if(0 == error)
      {
         if(sensor_data.new_data)
         {
            sensor_msg.msg.flags = sensor_data.flags;
            sensor_msg.msg.batt_percentage = sensor_data.batt_percentage;
            sensor_data.new_data = FALSE;
         }

         pthread_mutex_unlock(&sensor_mutex);
      }

      error = pthread_mutex_trylock(&operation_mutex);
      if(0 == error)
      {
         sensor_msg.msg.status = operation;
         sensor_msg.msg.progress = op_percentage;
         pthread_mutex_unlock(&operation_mutex);
      }

      // Send sensor data
      reply = MsgSend(server_coid, &sensor_msg, sizeof(msg_core_t), &operation_msg, sizeof(msg_intf_t));
      if(-1 == reply)
      {
          perror("Sending sensor data\n");
          exit(EXIT_FAILURE);
      }

      // If the last message sent was __OP_COMPLETE, reset values
      if(__OP_COMPLETE == sensor_msg.msg.status)
      {
         operation = __OP_IDLE;
         op_percentage = 0;
      }

      // Parse the reply
      if(__REPLY_NEW_OP == reply)
      {
         if((__OP_START_INFUSION == operation) || (__OP_START_FILLING == operation))
         {
            printf("An operation is already in progress\n");
         }

         if(__OP_START_INFUSION == operation_msg.msg.operation)
         {
            operation = __OP_START_INFUSION;
            flow_rate = operation_msg.msg.rate;
            vol = operation_msg.msg.vol;
            max_press = operation_msg.msg.max_press;
            min_rate = operation_msg.msg.min_rate;
            sensor_msg.msg.progress = 0;
            infusion = TRUE;
            raise(__SIG_SETUP_OP);
         }
         else if(__OP_START_FILLING == operation_msg.msg.operation)
         {
            operation = __OP_START_FILLING;
            flow_rate = operation_msg.msg.rate;
            vol = operation_msg.msg.vol;
            max_press = operation_msg.msg.max_press;
            min_rate = operation_msg.msg.min_rate;
            sensor_msg.msg.progress = 0;
            infusion = FALSE;
            raise(__SIG_SETUP_OP);
         }
      }

      sleep(1);
   }

   return ((void *)NULL);
}

/*
 * Function     - ipc_signal_thread()
 *
 * Arguments    - <args> Pointer to this thread's arguments
 *
 * Return Value - Pointer to the return value
 */
void * ipc_signal_thread(void * args)
{
   sigset_t set;
   siginfo_t sig_info;

   // Setup the signal
   sigemptyset(&set);
   sigaddset(&set, __SIG_INTF_STOP);
   pthread_sigmask(SIG_BLOCK, &set, NULL);

   // Wait for the stop signal
   for(;;)
   {
      sigwaitinfo(&set, &sig_info);
      if(__SIG_INTF_STOP == sig_info.si_signo)
      {
         flow_rate = 0;
         raise(__SIG_SETUP_OP);
         operation = __OP_IDLE;
         op_percentage = 0;
      }
   }
}
