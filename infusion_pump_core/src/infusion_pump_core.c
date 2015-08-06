/*
 * File    - infusion_pump_core.c
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
#include <sched.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/dispatch.h>
#include <sys/types.h>
#include <sys/stat.h>

/*
 * Local header files
 */
#include "utility.h"

/*
 * Function     - main()
 *
 * Arguments    - <argc> Number of arguments
 *                <argv[]> Array of arguments
 *
 * Return Value - EXIT_SUCCESS or EXIT_FAILURE
 */
int main(int argc, char * argv[])
{
   pthread_attr_t sensor_thread_attr, operation_thread_attr, ipc_client_thread_attr;
   sched_param_t sensor_thread_param, operation_thread_param, ipc_client_thread_param;

   // Setup the sensor thread
   pthread_attr_init(&sensor_thread_attr);
   pthread_attr_setdetachstate (&sensor_thread_attr, PTHREAD_CREATE_DETACHED);
   pthread_attr_setinheritsched (&sensor_thread_attr, PTHREAD_EXPLICIT_SCHED);
   pthread_attr_setschedpolicy (&sensor_thread_attr, SCHED_RR);
   sensor_thread_param.sched_priority = __PRIO_SENSOR_THREAD;
   pthread_attr_setschedparam (&sensor_thread_attr, &sensor_thread_param);

   // Setup the IPC client thread
   pthread_attr_init(&ipc_client_thread_attr);
   pthread_attr_setdetachstate (&ipc_client_thread_attr, PTHREAD_CREATE_DETACHED);
   pthread_attr_setinheritsched (&ipc_client_thread_attr, PTHREAD_EXPLICIT_SCHED);
   pthread_attr_setschedpolicy (&ipc_client_thread_attr, SCHED_RR);
   ipc_client_thread_param.sched_priority = __PRIO_IPC_CLIENT_THREAD;
   pthread_attr_setschedparam (&ipc_client_thread_attr, &ipc_client_thread_param);

   // Setup the operation thread
   pthread_attr_init(&operation_thread_attr);
   pthread_attr_setdetachstate (&operation_thread_attr, PTHREAD_CREATE_DETACHED);
   pthread_attr_setinheritsched (&operation_thread_attr, PTHREAD_EXPLICIT_SCHED);
   pthread_attr_setschedpolicy (&operation_thread_attr, SCHED_RR);
   operation_thread_param.sched_priority = __PRIO_OPERATION_THREAD;
   pthread_attr_setschedparam (&operation_thread_attr, &operation_thread_param);

   // Create the threads
   pthread_create(NULL, &sensor_thread_attr, sensor_thread, NULL);
   pthread_create(NULL, &ipc_client_thread_attr, ipc_client_thread, NULL);
   pthread_create(NULL, &operation_thread_attr, operation_thread, NULL);

   for(;;)
   {
   }

   return EXIT_SUCCESS;
}
