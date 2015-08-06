/*
 * File    - btn_lockscreen_callback.c
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
 * Function - btn_lockscreen_callback()
 *
 * Callback function for the "Lock Screen" button
 */
sint32_t btn_lockscreen_callback(PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo)
{
   // Eliminate 'unreferenced' warnings
   widget = widget, apinfo = apinfo, cbinfo = cbinfo;

   PtNotice(ABW_base, NULL, "Message", NULL, "The infusion tab will now be locked, it can be unlocked using the 'Activate' button above!",  NULL, "Ok", NULL, Pt_BLOCK_PARENT | Pt_WAIT);

   // Block the infusion tab
   wgt_blocktab(__TAB_INFUSION);

   return (Pt_CONTINUE);
}

