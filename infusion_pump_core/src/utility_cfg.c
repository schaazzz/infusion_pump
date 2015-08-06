/*
 * File    - utility_cfg.c
 * Process - Infusion Pump Firmware
 * Module  - Core Process
 * Author  - Shahzeb Ihsan
 */

/*
 * Standard header files
 */
#include <string.h>

/*
 * Local header files
 */
#include "utility.h"

/*
 * Local scope variables
 */
static char host_ip[20];

#if(__VIRTUAL_MACHINE == 1)
static uint16_t sensor_port;
#endif   // #if(__VIRTUAL_MACHINE == 1)

/*
 * Function     - cfg_gethostip()
 *
 * Arguments    - None
 *
 * Return Value - Pointer to a string containing host IP
 */
char * cfg_gethostip(void)
{
   // TODO: Place holder - need to read value from the configuration file
   strcpy(host_ip, "192.168.11.132");
   return (host_ip);
}

#if(__VIRTUAL_MACHINE == 1)
/*
 * Function     - cfg_getsensorport()
 *
 * Arguments    - None
 *
 * Return Value - Sensor port
 */
uint16_t cfg_getsensorport(void)
{
   // TODO: Place holder - need to read value from the configuration file
   sensor_port = 17236;
   return (sensor_port);
}
#endif   // #if(__VIRTUAL_MACHINE == 1)
