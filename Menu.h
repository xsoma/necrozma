#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "Hooks.h"

enum tab_t : int {
	undefined = -1,
	rage,
	antiaim,
	esp,
	world,
	misc,
	skins,
	scripts
};

class c_window;
class c_child;
class c_menu
{
public:
	void draw_indicators();
	c_child* weapon_cfg = nullptr;
	c_window* window = nullptr;
	bool initialized = false;
	void render();
	void update_binds();
	bool should_reinit_weapon_cfg = false;
	bool should_reinit_chams = false;
	bool should_reinit_config = false;
	bool should_reinit_scripts = false;
	bool should_reinit_skins = false;
	void reinit_chams();
	void reinit_config();
	void reinit_weapon_cfg();
	void reinit_scripts();
	void reinit_skins();
};

extern c_menu *g_Menu;

extern float g_Menucolor_t[4];
