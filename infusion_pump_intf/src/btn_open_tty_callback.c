/*
 * File    - btn_open_tty_callback.c
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
#include <time.h>
#include <errno.h>

/*
 * Local header files
 */
#include "ablibs.h"
#include "abimport.h"
#include "proto.h"
#define __USE_STDINT       (1)
#include "utility.h"

/*
 * Function - btn_open_tty_callback()
 *
 * Callback function for the "Open calibration terminal" button
 */
sint32_t btn_open_tty_callback( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
   PtArg_t arg;

   // Eliminate 'unreferenced' warnings
   widget = widget, apinfo = apinfo, cbinfo = cbinfo;

   // Create the terminal dialog
   ApCreateModule(ABM_terminal_dialog, ABW_base, NULL);

   PtTerminalPuts(ABW_tty, "Digital Infusion Pump - Calibration terminal...\r\n\r\n");

   // Open a psuedo TTY
   PtSetArg(&arg, Pt_ARG_TTY_PSEUDO, NULL, 0);
   PtSetResources(ABW_tty, 1, &arg);

   PtSetArg(&arg, Pt_ARG_TTY_FDS, 0, 0);
   PtGetResources(ABW_tty, 1, &arg);

   if(arg.value == -1)
   {
      PtTerminalPuts(ABW_tty, "Unable to find a ptty\r\n");
   }
   else
   {
      // Attach "/bin/sh" to the terminal
      PtSetArg(&arg, Pt_ARG_TTY_ARGV, NULL, 0);
      PtSetResources(ABW_tty, 1, &arg);

      PtSetArg(&arg, Pt_ARG_TTY_PID, NULL, 0);
      PtGetResources(ABW_tty, 1, &arg);

      if(arg.value == 0)
      {
         PtTerminalPuts(ABW_tty, "Unable to spawn the shell\r\n");
      }
   }

   return (Pt_CONTINUE);
}

