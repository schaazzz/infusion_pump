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
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>
#include <dirent.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/neutrino.h>
#include <sys/procfs.h>

/*
 * Structure used to get the process info
 */
struct _proc_info
{
   procfs_debuginfo info;
   char path_buf[_POSIX_PATH_MAX];
};

/*
 * Local header files
 */
#define __USE_STDINT          (0)
#include "utility.h"

/*
 *  Function     - sys_getcorepid()
 *
 * Arguments    - None
 *
 * Return Value - -1 if PID not found or , PID otherwise
 */
sint32_t sys_getcorepid(void)
{
   DIR * dir;
   uint32_t pid = 0;
   struct dirent * dir_entry;
   struct _proc_info dbg_info;
   char path[_POSIX_PATH_MAX + 1];
   sint32_t proc_fd = 0, status, ret_val;

   // Open up the "proc" name space to find the core process PID
   dir = opendir("/proc");

   // Cycle through "/proc" to find the core process
   for(;;)
   {
      dir_entry = readdir(dir);
      if(dir_entry == NULL)
      {
         ret_val = -1;
         break;
      }

      snprintf(path, sizeof(path), "/proc/%s", dir_entry->d_name);
      if((proc_fd = open(path, O_RDONLY)) != -1)
      {
         // Get the information from procmgr
         memset(&dbg_info, 0, sizeof(struct _proc_info));
         status = devctl(proc_fd, DCMD_PROC_MAPDEBUG_BASE, &dbg_info, sizeof(struct _proc_info), 0);
         if( status != EOK )
         {
            ret_val = -1;
            continue;
         }

         // Check if the current path matches the core process, if
         // so, return the PID
         if(!strcmp(dbg_info.info.path, __PROC_CORE_NAME))
         {
            sscanf((char *)dir_entry->d_name, "%d", &pid);
            ret_val = pid;
            break;
         }
      }
   }

   close(proc_fd);
   return (ret_val);
}

/*
 *  Function     - sys_setip()
 *
 * Arguments    - <ip> Pointer to a string containing IP address
 *
 * Return Value - None
 */
void sys_setip(char * ip)
{
   char cmd_str[20];

   snprintf(cmd_str, sizeof(cmd_str), "ifconfig en0 %s", ip);
   system(cmd_str);
}
