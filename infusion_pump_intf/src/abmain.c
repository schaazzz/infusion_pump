/*
 * File    - abmain.c
 * Process - Infusion Pump Firmware
 * Module  - Interface Process
 * Author  - Shahzeb Ihsan
 */

/*
 * Standard header files
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include <time.h>
#include <errno.h>
#include <sys/neutrino.h>
#include <sys/dispatch.h>
#include <photon/PtTty.h>

/*
 * Local header files
 */
#include "ablibs.h"
#include "abimport.h"
#include "proto.h"
#include "abwidgets.h"
#include "abdefine.h"
#include "abevents.h"
#include "ablinks.h"
#include "abvars.h"
#define __USE_STDINT       (1)
#include "utility.h"

/*
 * External variables
 */
extern const char * month[];
extern syringe_t sel_syringe;

sint32_t main (int argc, char *argv[])
{
   pthread_attr_t ui_thread_attr, nw_thread_attr, ipc_server_thread_attr;
   sched_param_t ui_thread_param, nw_thread_param, ipc_server_thread_param;

   // Setup system defaults
   sel_syringe.selected = FALSE;

   // Setup the UI thread
   pthread_attr_init(&ui_thread_attr);
   pthread_attr_setdetachstate (&ui_thread_attr, PTHREAD_CREATE_DETACHED);
   pthread_attr_setinheritsched (&ui_thread_attr, PTHREAD_EXPLICIT_SCHED);
   pthread_attr_setschedpolicy (&ui_thread_attr, SCHED_RR);
   ui_thread_param.sched_priority = __PRIO_UI_THREAD;
   pthread_attr_setschedparam (&ui_thread_attr, &ui_thread_param);

   // Short wait here to make sure the UI initializes first
   sleep(1);

   // Setup the network thread
   pthread_attr_init(&nw_thread_attr);
   pthread_attr_setdetachstate (&nw_thread_attr, PTHREAD_CREATE_DETACHED);
   pthread_attr_setinheritsched (&nw_thread_attr, PTHREAD_EXPLICIT_SCHED);
   pthread_attr_setschedpolicy (&nw_thread_attr, SCHED_RR);
   nw_thread_param.sched_priority = __PRIO_NW_THREAD;
   pthread_attr_setschedparam (&nw_thread_attr, &nw_thread_param);

   // Setup the IPC server thread
   pthread_attr_init(&ipc_server_thread_attr);
   pthread_attr_setdetachstate (&ipc_server_thread_attr, PTHREAD_CREATE_DETACHED);
   pthread_attr_setinheritsched (&ipc_server_thread_attr, PTHREAD_EXPLICIT_SCHED);
   pthread_attr_setschedpolicy (&ipc_server_thread_attr, SCHED_RR);
   ipc_server_thread_param.sched_priority = __PRIO_IPC_SERVER_THREAD;
   pthread_attr_setschedparam (&ipc_server_thread_attr, &ipc_server_thread_param);

   // Create the threads
   pthread_create(NULL, &ui_thread_attr, ui_thread, NULL);
   pthread_create(NULL, &nw_thread_attr, nw_thread, NULL);
   pthread_create(NULL, &ipc_server_thread_attr, ipc_server_thread, NULL);

   for(;;)
   {
   }

   return (0);
}

/*
 * Function     - ui_thread()
 *
 * Arguments    - <args> Pointer to this thread's arguments
 *
 * Return Value - Pointer to the return value
 */
void * ui_thread(void * args)
{
   char value_str[50];
   struct tm * cal_time;
   struct timespec system_time;

   _Ap_.Ap_winstate = 0;

   // AppBuilder Initialization
   ApInitialize(0, NULL, &AbContext);

   // Display main window
   ApLinkWindow(NULL, &AbApplLinks[0], NULL);

   // Initialize widget utility API
   wgt_setup();
   wgt_insertlog("Interface initialized...");

   // Disable all tabs
   wgt_blocktab(__TAB_INFUSION);
   wgt_blocktab(__TAB_CONFIG);
   wgt_blocktab(__TAB_FILL);
   wgt_blocktab(__TAB_DIAG);

   // Get system time and convert to calendar format
   clock_gettime(CLOCK_REALTIME, &system_time);
   cal_time = gmtime(&(system_time.tv_sec));

   // Format date string
   snprintf(value_str, 30, "%2d %s, %d", cal_time->tm_mday, month[cal_time->tm_mon], (cal_time->tm_year + 1900));

   // Display current date
   wgt_settext(ABN_lbl_date, value_str);

   // Load host IP address
   cfg_getfield("host_ip", value_str, sizeof(value_str));
   wgt_settext(ABN_txt_host_ip, value_str);

   // Load device IP address
   cfg_getfield("dev_ip", value_str, sizeof(value_str));
   wgt_settext(ABN_txt_dev_ip, value_str);

   // Set device IP
   sys_setip(value_str);

   // Load network port
   cfg_getfield("nw_port", value_str, sizeof(value_str));
   wgt_settext(ABN_txt_nw_port, value_str);

   // Loop until user quits application
   PtMainLoop();

   // Exit application
   PtExit(0);

   return ((void *)NULL);
}

static const ApClassTab_t ClassTable[] = {{ "PtWindow", &PtWindow },
                                          { "PtComboBox", &PtComboBox },
                                          { "PtButton", &PtButton },
                                          { "PtPanelGroup", &PtPanelGroup },
                                          { "PtContainer", &PtContainer },
                                          { "PtText", &PtText },
                                          { "PtLabel", &PtLabel },
                                          { "PtMultiText", &PtMultiText },
                                          { "PtBasic", &PtBasic },
                                          { "PtClock", &PtClock },
                                          { "PtOnOffButton", &PtOnOffButton },
                                          { "PtTty", &PtTty },
                                          { NULL, NULL }};

ApContext_t AbContext = {ClassTable, 1, AbWidgets};
