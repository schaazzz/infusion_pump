/*
 * File    - txt_default_focus_callback.c
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
#include <sys/socket.h>
#include <arpa/inet.h>

/*
 * Local header files
 */
#include "ablibs.h"
#include "abimport.h"
#include "proto.h"
#define __USE_STDINT       (1)
#include "utility.h"

/*
 * Global scope variables
 */
extern PtWidget_t * txt_widget;

/*
 * Function - txt_default_focus_callback()
 *
 * Callback function for all text field
 */
sint32_t txt_default_focus_callback(PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo)
{
   // Eliminate 'unreferenced' warnings
   widget = widget, apinfo = apinfo, cbinfo = cbinfo;

   if(Pt_CB_GOT_FOCUS == cbinfo->reason)
   {
      txt_widget = widget;
   }

   return (Pt_CONTINUE);
}

