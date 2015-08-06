/*
 * File    - btn_setpwd_callback.c
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
 * Function - btn_setpwd_callback()
 *
 * Callback function for the "Set" password button
 */
sint32_t btn_setpwd_callback(PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo)
{
   char profile[15], field[15], pin[10];

   // Eliminate 'unreferenced' warnings
   widget = widget, apinfo = apinfo, cbinfo = cbinfo;

   // Read the profile combo box
   wgt_gettext(ABN_combo_pin, profile, sizeof(profile));

   if(0 == strcmp(profile, "Infusion"))
      strcpy(field, "pwd_inf");
   else if(0 == strcmp(profile, "Filling"))
      strcpy(field, "pwd_fill");
   else if(0 == strcmp(profile, "Configuration"))
      strcpy(field, "pwd_conf");
   else if(0 == strcmp(profile, "Diagnostics"))
      strcpy(field, "pwd_diag");

   // Read the pin
   wgt_gettext(ABN_txt_new_pin, pin, sizeof(pin));

   if(__PIN_NUM_DIGITS != strlen(pin))
      PtNotice(widget, NULL, "Error!", NULL, "Incorrect pin length, please enter 4 digits!",  NULL, "Ok", NULL, Pt_WAIT | Pt_BLOCK_PARENT);
   else
      cfg_setfield(field, pin);

   return (Pt_CONTINUE);
}
