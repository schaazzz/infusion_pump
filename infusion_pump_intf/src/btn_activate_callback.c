/*
 * File    - btn_activate_callback.c
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
 * Global scope variables
 */
extern char pin[5];

/*
 * Function - btn_activate_callback()
 *
 * Callback function for the "Activate" button
 */
sint32_t btn_activate_callback(PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo)
{
   char profile[20], temp[10];

   // Eliminate 'unreferenced' warnings
   widget = widget, apinfo = apinfo, cbinfo = cbinfo;

   // Get the selected profile
   wgt_gettext(ABN_combo_profile, profile, sizeof(profile));

   // Clear the entered pin
   wgt_settext(ABN_txt_pin, "");

   // Check the pin
   if((0 == strlen(pin)) ||
      ((0 != strcmp(profile, "Infusion")) &&
      (0 != strcmp(profile, "Filling")) &&
      (0 != strcmp(profile, "Configuration")) &&
      (0 != strcmp(profile, "Diagnostics"))))
   {
      PtNotice(widget, NULL, "Error!", NULL, "Select a profile and enter the corresponding pin!",  NULL, "Ok", NULL, Pt_WAIT | Pt_BLOCK_PARENT);
      return (Pt_CONTINUE);
   }

   // Check infusion pin
   if(0 == strcmp(profile, "Infusion"))
   {
      cfg_getfield("pwd_inf", temp, sizeof(temp));

      if(0 != strcmp(temp, pin))
      {
         PtNotice(widget, NULL, "Error!", NULL, "Incorrect pin!",  NULL, "Ok", NULL, Pt_WAIT | Pt_BLOCK_PARENT);
         return (Pt_CONTINUE);
      }

      // Unblock the infusion pin
      wgt_unblocktab(__TAB_INFUSION);

      wgt_insertlog("Infusion tab unlocked");
   }

   // Check filling pin
   if(0 == strcmp(profile, "Filling"))
   {
      cfg_getfield("pwd_fill", temp, sizeof(temp));

      if(0 != strcmp(temp, pin))
      {
         PtNotice(widget, NULL, "Error!", NULL, "Incorrect pin!",  NULL, "Ok", NULL, Pt_WAIT | Pt_BLOCK_PARENT);
         return (Pt_CONTINUE);
      }

      // Unblock the filling pin
      wgt_unblocktab(__TAB_FILL);

      wgt_insertlog("Filling tab unlocked");
   }

   // Check configuration pin
   if(0 == strcmp(profile, "Configuration"))
   {
      cfg_getfield("pwd_conf", temp, sizeof(temp));

      if(0 != strcmp(temp, pin))
      {
         PtNotice(widget, NULL, "Error!", NULL, "Incorrect pin!",  NULL, "Ok", NULL, Pt_WAIT | Pt_BLOCK_PARENT);
         return (Pt_CONTINUE);
      }

      // Unblock the configuration pin
      wgt_unblocktab(__TAB_CONFIG);
   }

   // Check diagnostics pin
   if(0 == strcmp(profile, "Diagnostics"))
   {
      cfg_getfield("pwd_diag", temp, sizeof(temp));

      if(0 != strcmp(temp, pin))
      {
         PtNotice(widget, NULL, "Error!", NULL, "Incorrect pin!",  NULL, "Ok", NULL, Pt_WAIT | Pt_BLOCK_PARENT);
         return (Pt_CONTINUE);
      }

      // Unblock the diagnostics pin
      wgt_unblocktab(__TAB_DIAG);
   }
   return (Pt_CONTINUE);
}
