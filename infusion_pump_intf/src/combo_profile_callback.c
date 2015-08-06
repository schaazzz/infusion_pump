/*
 * File    - combo_profile_callback.c
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
 * Function - combo_profile_callback()
 *
 * Callback function for the profile combo box
 */
sint32_t combo_profile_callback(PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo)
{
   // Eliminate 'unreferenced' warnings
   widget = widget, apinfo = apinfo, cbinfo = cbinfo;

   // Unblock the pin text field
   if(Pt_LIST_SELECTION_FINAL == cbinfo->reason_subtype)
   {
      PtSetResource(ABW_txt_pin, Pt_ARG_FLAGS, 0, Pt_BLOCKED);
   }

   return (Pt_CONTINUE);
}

