/*
 * File    - utility_wgt.c
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
#include "ablibs.h"
#include "abimport.h"
#include "proto.h"
#define __USE_STDINT       (1)
#include "utility.h"

/*
 * Global scope variables
 */
syringe_t sel_syringe;

/*
 * Local scope variables
 */
static sint32_t infusion_tab[7];
static sint32_t filling_tab[5];
static sint32_t config_tab[11];
static sint32_t diag_tab[2];

static inf_mode_t cont_inf = {"Volume (mL):",
                              "Transfer rate (mL/min):",
                              "Start time (HH-MM):"};

/*
 * Macro definitions
 */
#define M_DISABLE_TAB(_tab)                                                               \
{                                                                                         \
   uint32_t index;                                                                        \
   for(index = 0; index < (sizeof(_tab) / sizeof(_tab[0])); index++)                      \
      PtSetResource(AbGetABW(_tab[index]), Pt_ARG_FLAGS, Pt_BLOCKED, Pt_BLOCKED);         \
}

#define M_ENABLE_TAB(_tab)                                                                \
{                                                                                         \
   uint32_t index;                                                                        \
   for(index = 0; index < (sizeof(_tab) / sizeof(_tab[0])); index++)                      \
      PtSetResource(AbGetABW(_tab[index]), Pt_ARG_FLAGS, 0, Pt_BLOCKED);                  \
}

/*
 * Function     - wgt_setup()
 *
 * Arguments    - None
 *
 * Return Value - None
 */
void wgt_setup(void)
{
   char temp[200], * ptr;
   FILE * drug_list, * syringe_list;

   ptr = (char *)&temp;

   // Initialize the infusion tab
   infusion_tab[0] = ABN_combo_infusion_mode;
   infusion_tab[1] = ABN_btn_lock_screen;
   infusion_tab[2] = ABN_txt_param1;
   infusion_tab[3] = ABN_txt_param2;
   infusion_tab[4] = ABN_txt_param3;
   infusion_tab[5] = ABN_btn_start_inf;
   infusion_tab[6] = ABN_btn_stop_inf;

   // Initialize the filling tab
   filling_tab[0] = ABN_combo_drug;
   filling_tab[1] = ABN_combo_syringe;
   filling_tab[2] = ABN_txt_vol_fill;
   filling_tab[3] = ABN_btn_start_filling;
   filling_tab[4] = ABN_btn_stop_filling;

   // Initialize the configuration tab
   config_tab[0] = ABN_txt_host_ip;
   config_tab[1] = ABN_txt_dev_ip;
   config_tab[2] = ABN_txt_nw_port;
   config_tab[3] = ABN_btn_setip;
   config_tab[4] = ABN_combo_pin;
   config_tab[5] = ABN_txt_new_pin;
   config_tab[6] = ABN_btn_setpin;
   config_tab[7] = ABN_txt_time;
   config_tab[8] = ABN_txt_date;
   config_tab[9] = ABN_combo_ampm;
   config_tab[10] = ABN_btn_setdate;

   // Initialize the diagnostic tab
   diag_tab[0] = ABN_btn_clr_log;
   diag_tab[1] = ABN_btn_open_tty;

   // Set default infusion mode to continuous
   wgt_select_inf_mode(__MODE_INF_CONT);

   // Populate the drug list
   drug_list = fopen("/usr/bin/.drug_dict", "r");

   while(!feof(drug_list))
   {
      fgets(temp, sizeof(temp), drug_list);
      PtListAddItems(ABW_combo_drug, (const char **)&ptr, 1, 0);
   }

   // Populate the syringe list
   syringe_list = fopen("/usr/bin/.syringe_db", "r");

   while(!feof(syringe_list))
   {
      fgets(temp, sizeof(temp), syringe_list);
      ptr = strtok(temp, ",");
      PtListAddItems(ABW_combo_syringe, (const char **)&ptr, 1, 0);
   }

   wgt_selectsyringe(0);

   fclose(drug_list);
   fclose(syringe_list);
}

/*
 * Function     - wgt_select_inf_mode()
 *
 * Arguments    - <mode> Infusion mode to select
 *
 * Return Value - None
 */
void wgt_select_inf_mode(uint32_t mode)
{
   // Select continuous infusion mode
   if(__MODE_INF_CONT == mode)
   {
      wgt_settext(ABN_lbl_param1, cont_inf.param1);
      wgt_settext(ABN_lbl_param2, cont_inf.param2);
      wgt_settext(ABN_lbl_param3, cont_inf.param3);
   }
}

/*
 * Function     - wgt_blocktab()
 *
 * Arguments    - <tab> Tab to block
 *
 * Return Value - None
 */
void wgt_blocktab(uint32_t tab)
{
   if(__TAB_INFUSION == tab)
   {
      M_DISABLE_TAB(infusion_tab);
   }
   else if(__TAB_FILL == tab)
   {
      M_DISABLE_TAB(filling_tab);
   }
   else if(__TAB_CONFIG == tab)
   {
      M_DISABLE_TAB(config_tab);
   }
   else if(__TAB_DIAG == tab)
   {
      M_DISABLE_TAB(diag_tab);
   }
}

/*
 * Function     - wgt_unblocktab()
 *
 * Arguments    - <tab> Tab to unblock
 *
 * Return Value - None
 */
void wgt_unblocktab(uint32_t tab)
{
   if(__TAB_INFUSION == tab)
   {
      M_ENABLE_TAB(infusion_tab);
   }
   else if(__TAB_FILL == tab)
   {
      M_ENABLE_TAB(filling_tab);
   }
   else if(__TAB_CONFIG == tab)
   {
      M_ENABLE_TAB(config_tab);
   }
   else if(__TAB_DIAG == tab)
   {
      M_ENABLE_TAB(diag_tab);
   }
}

/*
 * Function     - wgt_gettext()
 *
 * Arguments    - <widget_name> Name of the widget
 *              - <str> Destination string pointer
 *              - <length> Length of the destination buffer
 *
 * Return Value - -1 if length of the destination buffer is not large enough
 */
sint32_t wgt_gettext(const sint32_t widget_name, char * str, uint32_t length)
{
   PtArg_t arg;
   char * buff;

   // Get the string from the widget
   PtSetArg(&arg, Pt_ARG_TEXT_STRING, 0, 0);
   PtGetResources(AbGetABW(widget_name), 1, &arg);
   buff = (char *)arg.value;

   // Copy the string
   if(length < strlen(buff))
      return (-1);
   else
      strcpy(str, buff);

   return (0);
}

/*
 * Function     - wgt_settext()
 *
 * Arguments    - <widget_name> Name of the widget
 *              - <str> Source string pointer
 *
 * Return Value - None
 */
void wgt_settext(const sint32_t widget_name, const char * str)
{
   // Set widget text
   PtSetResource(AbGetABW(widget_name), Pt_ARG_TEXT_STRING, str, 0);
}

/*
 * Function     - wgt_clearlog()
 *
 * Arguments    - None
 *
 * Return Value - None
 */
void wgt_clearlog(void)
{
   wgt_settext(ApName(ABW_multi_txt_diag), "");
}

/*
 * Function     - wgt_insertlog()
 *
 * Arguments    - <str> String to insert
 *
 * Return Value - None
 */
void wgt_insertlog(const char * str)
{
   char temp[30];
   struct tm * cal_time;
   struct timespec system_time;

   // Get system time and convert to calendar format
   clock_gettime(CLOCK_REALTIME, &system_time);
   cal_time = gmtime(&(system_time.tv_sec));

   // Format date string
   snprintf(temp, 30, "[%d/%02d/%02d, %02d:%02d] ",
            (cal_time->tm_year + 1900), cal_time->tm_mon,
            cal_time->tm_mday, cal_time->tm_hour, cal_time->tm_min);

   // Display the log string
   PtMultiTextModifyText(ABW_multi_txt_diag, NULL, NULL, -1, temp, strlen(temp), NULL, Pt_MT_FONT );
   PtMultiTextModifyText(ABW_multi_txt_diag, NULL, NULL, -1, str, strlen(str), NULL, Pt_MT_FONT );
   PtMultiTextModifyText(ABW_multi_txt_diag, NULL, NULL, -1, "\n", 1, NULL, Pt_MT_FONT );
}

/*
 * Function     - wgt_selectsyringe()
 *
 * Arguments    - <index> Syringe index
 *
 * Return Value - None
 */
void wgt_selectsyringe(uint32_t index)
{
   uint32_t i = 0;
   FILE * syringe_list;
   char temp[150], * max_vol, * dia, * len, * ml_mm, * min_flow_min, * max_flow_min, * max_press;

   syringe_list = fopen("/usr/bin/.syringe_db", "r");

   do
   {
      fgets(temp, sizeof(temp), syringe_list);
   }
   while(!feof(syringe_list) && (index != i++));

   // Set the syringe parameters and the respective parameters
   max_vol = strtok(temp, ",");
   sel_syringe.vol_ml = atof(max_vol);
   wgt_settext(ABN_lbl_syringe_vol, max_vol);

   dia = strtok(NULL, ",");
   sel_syringe.dia_mm = atof(dia);
   strcpy(temp, dia);
   strcat(temp, "mm");
   wgt_settext(ABN_lbl_syringe_dia, temp);

   len = strtok(NULL, ",");
   sel_syringe.len_mm = atof(len);

   ml_mm = strtok(NULL, ",");
   sel_syringe.ml_per_mm = atof(ml_mm);

   min_flow_min = strtok(NULL, ",");
   sel_syringe.min_ml_per_min = atof(min_flow_min);
   strcpy(temp, min_flow_min);
   strcat(temp, "ml/min");
   wgt_settext(ABN_lbl_syringe_min_flow, temp);

   max_flow_min = strtok(NULL, ",");
   sel_syringe.max_ml_per_min = atof(max_flow_min);
   strcpy(temp, max_flow_min);
   strcat(temp, "ml/min");
   wgt_settext(ABN_lbl_syringe_max_flow, temp);

   max_press = strtok(NULL, ",");
   sel_syringe.max_press = atof(max_press);

   sel_syringe.selected = TRUE;
}
