/*
 * File    - btn_setip_callback.c
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
 * Function - btn_setip_callback()
 *
 * Callback function for the "Apply" IP, port button
 */
sint32_t btn_setip_callback( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
   uint32_t temp_var;
   uint8_t temp_buff[15];
   char host_ip[15], dev_ip[15], nw_port[10];

   // Eliminate 'unreferenced' warnings
   widget = widget, apinfo = apinfo, cbinfo = cbinfo;

   // Read the host IP text field
   wgt_gettext(ABN_txt_host_ip, host_ip, sizeof(host_ip));

   // Read the device IP text field
   wgt_gettext(ABN_txt_dev_ip, dev_ip, sizeof(dev_ip));

   // Read the network port text field
   wgt_gettext(ABN_txt_nw_port, nw_port, sizeof(nw_port));

   // Set host IP
   if(0 == inet_pton(AF_INET, host_ip, temp_buff))
      PtNotice(widget, NULL, "Error!", NULL, "Invalid host IP!",  NULL, "Ok", NULL, Pt_WAIT | Pt_BLOCK_PARENT);
   else
      cfg_setfield("host_ip", host_ip);

   // Set device IP
   if(0 == inet_pton(AF_INET, dev_ip, temp_buff))
      PtNotice(widget, NULL, "Error!", NULL, "Invalid device IP!",  NULL, "Ok", NULL, Pt_WAIT | Pt_BLOCK_PARENT);
   else
   {
      cfg_setfield("dev_ip", dev_ip);
      sys_setip(dev_ip);
   }

   // Set network port
   if(1 != sscanf(nw_port, "%d", &temp_var))
      PtNotice(widget, NULL, "Error!", NULL, "Invalid network port!",  NULL, "Ok", NULL, Pt_WAIT | Pt_BLOCK_PARENT);
   else if(temp_var > 65535)
      PtNotice(widget, NULL, "Error!", NULL, "Invalid network port!",  NULL, "Ok", NULL, Pt_WAIT | Pt_BLOCK_PARENT);
   else
      cfg_setfield("nw_port", nw_port);

   return( Pt_CONTINUE );
}
