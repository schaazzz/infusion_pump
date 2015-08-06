/*
 * File    - btn_setdate_callback.c
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
const char * month[] = {"Jan", "Feb", "Mar",
                        "Apr", "May", "Jun",
                        "Jul", "Aug", "Sep",
                        "Oct", "Nov", "Dec"};

/*
 * Function - btn_setdate_callback()
 *
 * Callback function for the "Set" date/time button
 */
sint32_t btn_setdate_callback(PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo)
{
   struct timespec system_time;
   struct tm new_time, * orig_time;
   char time_str[30], date_str[30], meridian_str[5];

   // Eliminate 'unreferenced' warnings
   widget = widget, apinfo = apinfo, cbinfo = cbinfo;

   // Get system time and convert to calendar format
   clock_gettime(CLOCK_REALTIME, &system_time);
   orig_time = gmtime(&(system_time.tv_sec));

   // Read the AM/PM combo box
   wgt_gettext(ABN_combo_ampm, meridian_str, sizeof(meridian_str));

   // Read the date text field
   wgt_gettext(ABN_txt_date, date_str, sizeof(date_str));

   // Read the time text field
   wgt_gettext(ABN_txt_time, time_str, sizeof(time_str));

   // Parse date string
   if(3 != sscanf(date_str,
                  "%d/%d/%d",
                  &(new_time.tm_mday),
                  &(new_time.tm_mon),
                  &(new_time.tm_year)))
   {
      if(0 != strcmp(date_str, ""))
         PtNotice(widget, NULL, "Error!", NULL, "Incorrect date format!",  NULL, "Ok", NULL, Pt_WAIT | Pt_BLOCK_PARENT);

      new_time.tm_year = orig_time->tm_year;
      new_time.tm_mon = orig_time->tm_mon;
      new_time.tm_mday = orig_time->tm_mday;
   }
   else
   {
      // Adjust year and month
      new_time.tm_year -= 1900;
      new_time.tm_mon--;
   }

   // Parse time string
   if(2 != sscanf(time_str,
                  "%d:%d",
                  &(new_time.tm_hour),
                  &(new_time.tm_min)))
   {
      if(0 != strcmp(time_str, ""))
         PtNotice(widget, NULL, "Error!", NULL, "Incorrect time format!",  NULL, "Ok", NULL, Pt_WAIT | Pt_BLOCK_PARENT);

      new_time.tm_hour = orig_time->tm_hour;
      new_time.tm_min = orig_time->tm_min;
      new_time.tm_sec = orig_time->tm_sec;
   }
   else
   {
      // Set seconds to zero
      new_time.tm_sec = 0;

      // Adjust hour according to the meridian selection
      if((12 != new_time.tm_hour) && (0 == strcmp(meridian_str, "PM")))
         new_time.tm_hour += 12;
      else if((12 == new_time.tm_hour) && (0 == strcmp(meridian_str, "AM")))
         new_time.tm_hour = 0;
   }

   // Set system time
   system_time.tv_sec = mktime(&new_time);

   if((system_time.tv_sec < 0) ||
      (clock_settime(CLOCK_REALTIME, &system_time) < 0))
   {
      PtNotice(widget, NULL, "Error!", NULL, "Could not set time and date, please check input!",  NULL, "Ok", NULL, Pt_WAIT | Pt_BLOCK_PARENT);
   }
   else
   {
      // Format date string and update date display
      snprintf(date_str, 30, "%2d %s, %d", new_time.tm_mday, month[new_time.tm_mon], (new_time.tm_year + 1900));
      wgt_settext(ABN_lbl_date, date_str);
   }

   return (Pt_CONTINUE);
}

