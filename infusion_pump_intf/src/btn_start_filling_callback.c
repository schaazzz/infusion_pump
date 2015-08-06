/*
 * File    - btn_start_filling_callback.c
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
extern syringe_t sel_syringe;
extern msg_intf_t op_global_msg;
extern pthread_mutex_t operation_mutex;

/*
 * Function - btn_start_filling_callback()
 *
 * Callback function for the "Start" filling button
 */
sint32_t btn_start_filling_callback(PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo)
{
   char data[15];
   float flowrate, vol;

   // Eliminate 'unreferenced' warnings
   widget = widget, apinfo = apinfo, cbinfo = cbinfo;

   // Get filling parameters
   wgt_gettext(ABN_txt_vol_fill, data, sizeof(data));
   vol = atof(data);

   flowrate = sel_syringe.max_ml_per_min / 2;

   // Setup filling parameters
   if((0 == vol) || (0 == flowrate))
   {
      PtNotice(widget, NULL, "Error!", NULL, "Invalid filling parameters!",  NULL, "Ok", NULL, Pt_WAIT | Pt_BLOCK_PARENT);
   }
   else if(!sel_syringe.selected)
   {
      PtNotice(widget, NULL, "Error!", NULL, "Please select a syringe, first!",  NULL, "Ok", NULL, Pt_WAIT | Pt_BLOCK_PARENT);
   }
   else if(0 == pthread_mutex_lock(&operation_mutex))
   {
      op_global_msg.msg.vol = vol;
      op_global_msg.msg.rate = flowrate;
      op_global_msg.msg.max_press = sel_syringe.max_press;
      op_global_msg.msg.operation = __OP_START_FILLING;
      op_global_msg.msg.min_rate = sel_syringe.min_ml_per_min;
      pthread_mutex_unlock(&operation_mutex);
   }

   return (Pt_CONTINUE);
}
