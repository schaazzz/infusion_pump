/*
 * File    - combo_syringe_callback.c
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
 * Function - combo_syringe_callback()
 *
 * Callback function for the syringe combo box
 */
sint32_t combo_syringe_callback(PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo)
{
   char temp[20];

   // Eliminate 'unreferenced' warnings
   widget = widget, apinfo = apinfo, cbinfo = cbinfo;

   if(Pt_LIST_SELECTION_FINAL == cbinfo->reason_subtype)
   {
      // Get the selected syringe data
      wgt_gettext(ApName(widget), temp, sizeof(temp));
      wgt_selectsyringe(PtListItemPos(widget, temp) - 2);
   }

   return (Pt_CONTINUE);
}
