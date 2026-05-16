#include <obs-module.h>
#include <obs-frontend-api.h>
#include "videowall-dialog.hpp"



OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("plugintemplate-for-obs", "en-US")

static VideoWallDialog *dialog = nullptr;

static void videowall_menu_callback(void *data)
{
	UNUSED_PARAMETER(data);

	if (!dialog)
		dialog = new VideoWallDialog();

	dialog->refreshSceneTree();
	dialog->show();
	dialog->raise();
	dialog->activateWindow();

	blog(LOG_INFO, "[VideoWall] Menu clicked!");
}

bool obs_module_load(void)
{
	obs_frontend_add_tools_menu_item("VideoWall Layout", videowall_menu_callback, NULL);

	blog(LOG_INFO, "[VideoWall] plugin loaded successfully");

	

	return true;
}