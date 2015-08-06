/* Event header for application - AppBuilder 2.03  */

static const ApEventLink_t AbApplLinks[] = {
	{ 3, 0, 0L, 0L, 0L, &base, NULL, NULL, 0, NULL, 0, 0, 0, 0, },
	{ 0 }
	};

ApEventLink_t AbInternalLinks[] = {
	{ 4, 0, 0L, 0L, 0L, &terminal_dialog, NULL, NULL, 0, NULL, 0, 11, 0, 0, },
	{ 0 }
	};

static const ApEventLink_t AbLinks_base[] = {
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "combo_profile", 23010, combo_profile_callback, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "btn_activate", 2009, btn_activate_callback, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "txt_param3", 2010, txt_default_focus_callback, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "txt_param3", 2011, txt_default_focus_callback, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "txt_param1", 2010, txt_default_focus_callback, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "txt_param1", 2011, txt_default_focus_callback, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "txt_param2", 2010, txt_default_focus_callback, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "txt_param2", 2011, txt_default_focus_callback, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "btn_start_inf", 2009, btn_start_infusion_callback, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "btn_stop_inf", 2009, btn_stop_infusion_callback, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "btn_lock_screen", 2009, btn_lockscreen_callback, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "combo_syringe", 23010, combo_syringe_callback, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "txt_vol_fill", 2010, txt_default_focus_callback, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "txt_vol_fill", 2011, txt_default_focus_callback, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "btn_start_filling", 2009, btn_start_filling_callback, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "btn_stop_filling", 2009, btn_stop_filling_callback, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "txt_dev_ip", 2010, txt_default_focus_callback, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "txt_dev_ip", 2011, txt_default_focus_callback, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "txt_host_ip", 2010, txt_default_focus_callback, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "txt_host_ip", 2011, txt_default_focus_callback, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "txt_nw_port", 2010, txt_default_focus_callback, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "txt_nw_port", 2011, txt_default_focus_callback, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "btn_setip", 2009, btn_setip_callback, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "txt_new_pin", 2010, txt_default_focus_callback, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "txt_new_pin", 2011, txt_default_focus_callback, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "btn_setpin", 2009, btn_setpwd_callback, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "txt_time", 2010, txt_default_focus_callback, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "txt_time", 2011, txt_default_focus_callback, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "txt_date", 2010, txt_default_focus_callback, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "txt_date", 2011, txt_default_focus_callback, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "btn_setdate", 2009, btn_setdate_callback, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "btn_clr_log", 2009, btn_clr_log_callback, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "btn_open_tty", 2009, btn_open_tty_callback, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "txt_pin", 2010, txt_pin_focus_callback, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "txt_pin", 2011, txt_pin_focus_callback, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "btn_key_3", 2009, keypad_callback, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "btn_key_7", 2009, keypad_callback, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "btn_key_8", 2009, keypad_callback, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "btn_key_6", 2009, keypad_callback, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "btn_key_4", 2009, keypad_callback, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "btn_key_2", 2009, keypad_callback, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "btn_key_5", 2009, keypad_callback, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "btn_key_clear", 2009, keypad_callback, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "btn_key_0", 2009, keypad_callback, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "btn_key_9", 2009, keypad_callback, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "btn_key_1", 2009, keypad_callback, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "btn_key_slash", 2009, keypad_callback, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "btn_key_dash", 2009, keypad_callback, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "btn_key_dot", 2009, keypad_callback, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "btn_turnoff", 2009, btn_turnoff_callback, 0, 0, 0, 0, },
	{ 0 }
	};

const char ApOptions[] = AB_OPTIONS;
