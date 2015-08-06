/* Link header for application - AppBuilder 2.03  */

extern ApContext_t AbContext;

ApDialogLink_t terminal_dialog = {
	"terminal_dialog.wgtd",
	&AbContext,
	NULL, 0, 0
	};

ApWindowLink_t base = {
	"base.wgtw",
	&AbContext,
	AbLinks_base, 2, 50
	};

