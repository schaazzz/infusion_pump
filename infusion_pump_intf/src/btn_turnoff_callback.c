/*
 * File    - btn_turnoff_callback.c
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
 * Function - btn_turnoff_callback()
 *
 * Callback function for the "Turn Off" button
 */
sint32_t btn_turnoff_callback(PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo)
{
   // Eliminate 'unreferenced' warnings
   widget = widget, apinfo = apinfo, cbinfo = cbinfo;

   // Exit application
   PtExit(0);
	return( Pt_CONTINUE );
}
