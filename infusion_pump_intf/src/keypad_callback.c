/*
 * File    - keypad_callback.c
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
PtWidget_t * txt_widget = NULL;
char pin[5] = {0, 0, 0, 0, 0};

/*
 * Function - keypad_callback()
 *
 * Callback function for all keypad buttons
 */
sint32_t keypad_callback(PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo)
{
   char str[256];
   char key[2] = {0, 0};

	// Eliminate 'unreferenced' warnings
   widget = widget, apinfo = apinfo, cbinfo = cbinfo;

   if(NULL == txt_widget)
   {
      return (Pt_CONTINUE);
   }

   // Get key ID
   key[0] = (char)*widget->user_data;

   // Append the pressed key to the text of the currently
   // selected field or clear it if the "Clear" key was
   // pressed
   if(key[0] != 'c')
   {
      wgt_gettext(ApName(txt_widget), str, sizeof(str));

      if((ApName(txt_widget) == ABN_txt_pin) &&
         (strlen(pin) < 4))
      {
         strcat(str, "*");
         strcat(pin, key);
      }
      else if(ApName(txt_widget) != ABN_txt_pin)
      {
         strcat(str, key);
      }
   }
   else
   {
      str[0] = 0;
      pin[0]=0;
   }

   // Set the text in the focused text field
   wgt_settext(ApName(txt_widget), str);
   return (Pt_CONTINUE);
}
