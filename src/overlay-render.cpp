#include "overlay-render.hpp"
#include <obs-module.h>
#include <graphics/graphics.h>
#include <obs-frontend-api.h>

static int g_rows = 2;
static int g_cols = 2;

void set_grid_size(int rows, int cols)
{
	g_rows = rows;
	g_cols = cols;
}

static void draw_overlay(void *data, uint32_t cx, uint32_t cy)
{
	UNUSED_PARAMETER(data);

	if (g_rows <= 0 || g_cols <= 0)
		return;

	float cellWidth = (float)cx / g_cols;
	float cellHeight = (float)cy / g_rows;

	gs_effect_t *effect = obs_get_base_effect(OBS_EFFECT_SOLID);

	while (gs_effect_loop(effect, "Solid")) {

		for (int x = 1; x < g_cols; x++) {

			float xpos = cellWidth * x;

			struct vec2 p1 = {xpos, 0};
			struct vec2 p2 = {xpos, (float)cy};

			gs_render_line(&p1, &p2, 2.0f);
		}

		for (int y = 1; y < g_rows; y++) {

			float ypos = cellHeight * y;

			struct vec2 p1 = {0, ypos};
			struct vec2 p2 = {(float)cx, ypos};

			gs_render_line(&p1, &p2, 2.0f);
		}
	}
}

void register_overlay()
{
	obs_display_t *display = obs_frontend_get_main_preview();

	if (display)
		obs_display_add_draw_callback(display, draw_overlay, nullptr);
}

void unregister_overlay()
{
	obs_display_t *display = obs_frontend_get_main_preview();

	if (display)
		obs_display_remove_draw_callback(display, draw_overlay, nullptr);
}