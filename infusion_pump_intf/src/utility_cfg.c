/*
 * File    - utility_cfg.c
 * Process - Infusion Pump Firmware
 * Module  - Interface Process
 * Author  - Shahzeb Ihsan
 */

/*
 * Standard header files
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

/*
 * Local header files
 */
#define __USE_STDINT          (0)
#include "utility.h"

/*
 *  Function     - cfg_getfield()
 *
 * Arguments    - <field> Pointer to a memory location containing the field name
 *                <value> Pointer to a location where the value will be saved
 *                <size> Size of the value buffer
 *
 * Return Value - -1 if field not found or buffer too small, 0 otherwise
 */
sint32_t cfg_getfield(const char * field, char * value, uint32_t size)
{
   FILE * grep_out;
   char cmd_str[50], tmp_field[20], tmp_value[30];

   // Construct the command string
   strcpy(cmd_str, "cat /usr/bin/.infusion_general | grep ");
   strcat(cmd_str, field);

   // Issue command and get pointer to the file for piped output
   grep_out = popen(cmd_str, "r");

   // Retrieve value and close file
   fscanf(grep_out, "%s %s", tmp_field, tmp_value);
   pclose(grep_out);

   // TODO: Probably a good idea to check the return value from fscanf()
   //       for errors
   if(strlen(tmp_value) <= size)
      strcpy(value, tmp_value);
   else
      return (-1);

   return (0);
}

/*
 * Function     - cfg_setfield()
 *
 * Arguments    - <field> Pointer to a memory location containing the field name
 *                <value> Pointer to a location where the value will be saved
 *                <size> Size of the value buffer
 *
 * Return Value - -1 if field not found or buffer too small, 0 otherwise
 */
sint32_t cfg_setfield(const char * field, const char * value)
{
   char cmd_str[100];

   // Construct the command string
   strcpy(cmd_str, "sed 's/\\(");
   strcat(cmd_str, field);
   strcat(cmd_str, " *\\).*/\\1");
   strcat(cmd_str, value);
   strcat(cmd_str, "/' .infusion_general > tmp");

   // Replace the value for the specified field
   system(cmd_str);
   system("mv tmp .infusion_general");

   return (0);
}
