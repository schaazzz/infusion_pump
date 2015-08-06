/* Import (extern) header for application - AppBuilder 2.03  */

#include "abdefine.h"

extern ApEventLink_t AbInternalLinks[];

extern ApDialogLink_t terminal_dialog;
extern ApWindowLink_t base;
extern ApWidget_t AbWidgets[ 59 ];


#ifdef __cplusplus
extern "C" {
#endif
int keypad_callback( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int txt_pin_focus_callback( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int combo_profile_callback( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int txt_default_focus_callback( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int btn_turnoff_callback( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int btn_setdate_callback( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int btn_setpwd_callback( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int btn_setip_callback( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int btn_activate_callback( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int combo_syringe_callback( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int btn_clr_log_callback( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int btn_open_tty_callback( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int btn_start_infusion_callback( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int btn_stop_infusion_callback( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int btn_start_filling_callback( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int btn_stop_filling_callback( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int btn_lockscreen_callback( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
#ifdef __cplusplus
}
#endif
