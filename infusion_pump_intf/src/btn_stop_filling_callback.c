/*
 * File    - btn_stop_filling_callback.c
 * Process - Infusion Pump Firmware
 * Module  - Interface Process
 * Author  - Shahzeb Ihsan
 */

/*
 * Standard header files
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <signal.h>

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
extern uint32_t core_pid;
extern uint32_t operation;

/*
 * Function - btn_stop_filling_callback()
 *
 * Callback function for the "Stop" filling button
 */
sint32_t btn_stop_filling_callback(PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo)
{
   // Eliminate 'unreferenced' warnings */
   widget = widget, apinfo = apinfo, cbinfo = cbinfo;

   if((0 != core_pid) && (__OP_START_FILLING == operation))
   {
      kill(core_pid, __SIG_INTF_STOP);
   }

   return (Pt_CONTINUE);
}
