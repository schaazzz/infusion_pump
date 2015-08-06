/*
 * File    - txt_pin_focus_callback.c
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
extern char pin[5];

/*
 * Function - txt_pin_focus_callback()
 *
 * Callback function for the pin text field
 */
sint32_t txt_pin_focus_callback(PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo)
{
   // Eliminate 'unreferenced' warnings
   widget = widget, apinfo = apinfo, cbinfo = cbinfo;

   if(Pt_CB_GOT_FOCUS == cbinfo->reason)
   {
      wgt_settext(ApName(widget), "");
      txt_widget = widget;
      pin[0] = 0;
   }

   return (Pt_CONTINUE);
}

