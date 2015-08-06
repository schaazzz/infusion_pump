/*
 * File    - nw_thread.c
 * Process - Infusion Pump Firmware
 * Module  - Interface Process
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
 * Local definitions
 */
#define __OFFSET_RATE      (2)
#define __OFFSET_VOL       (10)

/*
 * External variables
 */
extern syringe_t sel_syringe;
extern msg_intf_t op_global_msg;
extern pthread_mutex_t operation_mutex;

/*
 * Local scope variables
 */
static sint32_t client_sock, server_sock;

/*
 * Function     - nw_thread()
 *
 * Arguments    - <args> Pointer to this thread's arguments
 *
 * Return Value - Pointer to the return value
 */
void * nw_thread(void * args)
{
   float val;
   sint32_t len;
   bool_t setup = TRUE;
   uint8_t recv_buff[512];

   for(;;)
   {
      // Setup the network connection
      if(setup)
      {
         network_connect(&server_sock, &client_sock, NULL, 17231, TRUE);
         setup = FALSE;
      }

      // Wait for a command from the control software
      len = network_recv(&client_sock, recv_buff, sizeof(recv_buff));

      // TODO: Lookup on how to detect a disconnection while using BSD sockets...
      // If we're receiving 0 length packets, the client is probably disconnected,
      // might not be the best way to detect a disconnection but it will have to
      // do for the time being
      if(0 == len)
      {
         network_close(&server_sock);
         setup = TRUE;
         continue;
      }

      if(0 == pthread_mutex_lock(&operation_mutex))
      {
         memcpy(&val, (recv_buff + __OFFSET_RATE), 4);
         op_global_msg.msg.vol = val;

         memcpy(&val, (recv_buff + __OFFSET_VOL), 4);
         op_global_msg.msg.rate = val;

         op_global_msg.msg.max_press = sel_syringe.max_press;
         op_global_msg.msg.operation = __OP_START_INFUSION;
         op_global_msg.msg.min_rate = sel_syringe.min_ml_per_min;
         pthread_mutex_unlock(&operation_mutex);
      }
   }

   return ((void *)NULL);
}
