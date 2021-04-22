#include "Menu.h"
#include "GUI/gui.h"
#include "skinchanger.h"
#include "scripting/java loader/js_loader.h"
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#define FCVAR_HIDDEN			(1<<4)	// Hidden. Doesn't appear in find or 
#define FCVAR_UNREGISTERED		(1<<0)	// If this is set, don't add to linked list, etc.
#define FCVAR_DEVELOPMENTONLY	(1<<1)	// Hidden in released products. Flag is removed 
vector<string> ConfigList;
vector<string> ScriptsList;
typedef void(*LPSEARCHFUNC)(LPCTSTR lpszFileName);

BOOL SearchFiles(LPCTSTR lpszFileName, LPSEARCHFUNC lpSearchFunc, BOOL bInnerFolders = FALSE)
{
	LPTSTR part;
	char tmp[MAX_PATH];
	char name[MAX_PATH];

	HANDLE hSearch = NULL;
	WIN32_FIND_DATA wfd;
	memset(&wfd, 0, sizeof(WIN32_FIND_DATA));

	if (bInnerFolders)
	{
		if (GetFullPathName(lpszFileName, MAX_PATH, tmp, &part) == 0) return FALSE;
		strcpy(name, part);
		strcpy(part, "*.*");
		wfd.dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
		if (!((hSearch = FindFirstFile(tmp, &wfd)) == INVALID_HANDLE_VALUE))
			do
			{
				if (!strncmp(wfd.cFileName, ".", 1) || !strncmp(wfd.cFileName, "..", 2))
					continue;

				if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					char next[MAX_PATH];
					if (GetFullPathName(lpszFileName, MAX_PATH, next, &part) == 0) return FALSE;
					strcpy(part, wfd.cFileName);
					strcat(next, "\\");
					strcat(next, name);

					SearchFiles(next, lpSearchFunc, TRUE);
				}
			} while (FindNextFile(hSearch, &wfd));
			FindClose(hSearch);
	}

	if ((hSearch = FindFirstFile(lpszFileName, &wfd)) == INVALID_HANDLE_VALUE)
		return TRUE;
	do
		if (!(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			char file[MAX_PATH];
			if (GetFullPathName(lpszFileName, MAX_PATH, file, &part) == 0) return FALSE;
			strcpy(part, wfd.cFileName);

			lpSearchFunc(wfd.cFileName);
		}
	while (FindNextFile(hSearch, &wfd));
	FindClose(hSearch);
	return TRUE;
}
void ReadConfigs(LPCTSTR lpszFileName)
{
	ConfigList.push_back(lpszFileName);
}
void ReadScripts(LPCTSTR lpszFileName)
{
	ScriptsList.push_back(lpszFileName);
}
void RefreshScripts()
{
	static TCHAR path[MAX_PATH];
	std::string folder, file;

	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, path)))
	{
		ScriptsList.clear();
		string ConfigDir = std::string(path) + "\\necrozma\\scripts\\*.js";
		SearchFiles(ConfigDir.c_str(), ReadScripts, FALSE);
	}

}
void RefreshConfigs()
{
	static TCHAR path[MAX_PATH];
	std::string folder, file;

	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, path)))
	{
		ConfigList.clear();
		string ConfigDir = std::string(path) + "\\necrozma\\*.dmp";
		SearchFiles(ConfigDir.c_str(), ReadConfigs, FALSE);
	}
}
void EnableHiddenCVars()
{
	auto p = **reinterpret_cast<ConCommandBase***>(interfaces.cvars + 0x34);
	for (auto c = p->m_pNext; c != nullptr; c = c->m_pNext)
	{
		ConCommandBase* cmd = c;
		cmd->m_nFlags &= ~FCVAR_DEVELOPMENTONLY;
		cmd->m_nFlags &= ~FCVAR_HIDDEN;
	}
}
template<class T>
static T* FindHudElement(const char* name)
{
	static auto pThis = *reinterpret_cast<DWORD**> (csgo->Utils.FindPatternIDA(GetModuleHandleA("client.dll"), "B9 ? ? ? ? E8 ? ? ? ? 8B 5D 08") + 1);

	static auto find_hud_element = reinterpret_cast<DWORD(__thiscall*)(void*, const char*)>(csgo->Utils.FindPatternIDA(GetModuleHandleA("client.dll"), "55 8B EC 53 8B 5D 08 56 57 8B F9 33 F6 39 77 28"));
	return (T*)find_hud_element(pThis, name);
}
struct hud_weapons_t
{
	std::int32_t* get_weapon_count()
	{
		return reinterpret_cast<std::int32_t*>(std::uintptr_t(this) + 0x80);
	}
};

void KnifeApplyCallbk()
{
	static auto clear_hud_weapon_icon_fn = reinterpret_cast<std::int32_t(__thiscall*)(void*, std::int32_t)>(csgo->Utils.FindPatternIDA(GetModuleHandleA("client.dll"), ("55 8B EC 51 53 56 8B 75 08 8B D9 57 6B FE 2C")));

	auto element = FindHudElement<std::uintptr_t*>(("CCSGO_HudWeaponSelection"));

	auto hud_weapons = reinterpret_cast<hud_weapons_t*>(std::uintptr_t(element) - 0xA0);
	if (hud_weapons == nullptr)
		return;

	if (!*hud_weapons->get_weapon_count())
		return;

	for (std::int32_t i = 0; i < *hud_weapons->get_weapon_count(); i++)
		i = clear_hud_weapon_icon_fn(hud_weapons, i);

	csgo->client_state->ForceFullUpdate();
}
Vector2D g_mouse;
color_t main_color = color_t(vars.misc.accentcolor.get_red(), vars.misc.accentcolor.get_green(), vars.misc.accentcolor.get_blue(), vars.misc.accentcolor.get_alpha());
bool enable_rage() { return true; };
#include "AntiAims.h"
void c_menu::draw_indicators()
{
}
bool need_warning() {
	return (csgo->expire_date - time(0)) / 3600 < 48;
};
void c_menu::render() {
	if (window) {
		window->update_keystates();
		window->update_animation();
		update_binds();
	}

	static bool once = false;
	if (!once) {
		Config.ResetToDefault();
		vars.menu.open = true;
		once = true;
	}

	if (initialized) {
		if (window->get_transparency() < 100.f && vars.menu.open)
			window->increase_transparency(animation_speed * 80.f);
	}
	else {
		window = new c_window();
		window->set_size(Vector2D(640, 488));
		window->set_position(Vector2D(200, 100));
		window->add_tab(new c_tab("ragebot", tab_t::rage, window)); {
			auto main_child = new c_child("main", tab_t::rage, window);
			main_child->set_size(Vector2D(230, 460));
			main_child->set_position(Vector2D(120, -50)); {

				main_child->add_element(new c_checkbox("enable", &vars.ragebot.enable));

				main_child->add_element(new c_checkbox("autoshoot", &vars.ragebot.autoshoot));

				main_child->add_element(new c_checkbox("autoscope", &vars.ragebot.autoscope));

				main_child->add_element(new c_checkbox("backtrack", &vars.ragebot.posadj));

				main_child->add_element(new c_checkbox("correct anti-aim", &vars.ragebot.resolver));

				main_child->add_element(new c_checkbox("knife bot", &vars.misc.knifebot));

				main_child->add_element(new c_checkbox("doubletap", &vars.ragebot.doubletap, nullptr, vars.ragebot.double_tap));
				main_child->add_element(new c_combo("doubletap type", &vars.ragebot.dttype,
					{
						"defensive",
						"offensive",
					}));
				main_child->add_element(new c_checkbox("hide shots", &vars.ragebot.hideshot, nullptr, vars.ragebot.hide_shots));

				main_child->add_element(new c_checkbox("override baim", &vars.ragebot.preferbodyaim, nullptr, vars.ragebot.force_body));
				main_child->add_element(new c_checkbox("override min damage", &vars.ragebot.mindmgoverride, nullptr, vars.ragebot.override_dmg));

				main_child->initialize_elements();
			}
			window->add_element(main_child);

			reinit_weapon_cfg();
		}
		window->set_transparency(100.f);
		window->add_tab(new c_tab("anti-aim", tab_t::antiaim, window));
		{
			auto antiaim_main = new c_child("main settings", tab_t::antiaim, window);
			antiaim_main->set_size(Vector2D(230, 460));
			antiaim_main->set_position(Vector2D(120, -50)); {

				antiaim_main->add_element(new c_checkbox("enable", &vars.antiaim.enable));
				antiaim_main->add_element(new c_checkbox("at targets", &vars.antiaim.attargets));

				antiaim_main->add_element(new c_combo("pitch", &vars.antiaim.pitch,
					{
						"off",
						"down",
						"emotion",
						"up",
					}));

				antiaim_main->add_element(new c_combo("direction", &vars.antiaim.yaw,
					{
						"off",
						"backwards",
					}));
				antiaim_main->add_element(new c_checkbox("jitter", &vars.antiaim.enabled_jitter));
				antiaim_main->add_element(new c_slider("jitter range", &vars.antiaim.jitter, 0, 180, "%.0f"));

				antiaim_main->add_element(new c_combo("desync", &vars.antiaim.desync_type,
					{
						"static",
						"lagsync",
						"jitter"
					}));

				antiaim_main->add_element(new c_checkbox("desync inverter", &vars.ragebot.desyncinverter, nullptr, vars.antiaim.inverter));


				antiaim_main->add_element(new c_slider("first desync value", &vars.antiaim.blin, 0, 100, "%.0f"));
				antiaim_main->add_element(new c_slider("second desync value", &vars.antiaim.inverterblin, 0, 100, "%.0f"));

				antiaim_main->initialize_elements();
			}
			window->add_element(antiaim_main);

			auto antiaim_other = new c_child("antiaim other", tab_t::antiaim, window);
			antiaim_other->set_size(Vector2D(250, 460));
			antiaim_other->set_position(Vector2D(365, -50));
			{
				antiaim_other->add_element(new c_combo("fake lag", &vars.antiaim.fakelag,
					{
						"off",
						"factor",
						"adaptive",
						"switch",
					}));

				antiaim_other->add_element(new c_slider("", &vars.antiaim.fakelagfactor, 0, 14, "%.0f"));
				antiaim_other->add_element(new c_slider("variance", &vars.antiaim.fakelagvariance, 0, 100, "%.0f"));

				antiaim_other->add_element(new c_checkbox("desync on shot", &vars.antiaim.fakelag_onshot));

				antiaim_other->add_element(new c_checkbox("duck peek assist", &vars.antiaim.fakeduck_check, nullptr, vars.antiaim.fakeduck));
				antiaim_other->add_element(new c_checkbox("slowmotion", &vars.antiaim.slowwalk_check, nullptr, vars.antiaim.slowwalk));

				antiaim_other->add_element(new c_checkbox("allow manuals", &vars.antiaim.aa_override.enable));

				antiaim_other->add_element(new c_checkbox("back", &vars.antiaim.manualback, nullptr, vars.antiaim.aa_override.back));
				antiaim_other->add_element(new c_checkbox("left", &vars.antiaim.manualleft, nullptr, vars.antiaim.aa_override.left));
				antiaim_other->add_element(new c_checkbox("right", &vars.antiaim.manualright, nullptr, vars.antiaim.aa_override.right));

				antiaim_other->initialize_elements();
			}
			window->add_element(antiaim_other);

		}
		window->add_tab(new c_tab("players", tab_t::esp, window)); {

			auto esp_main = new c_child("esp", tab_t::esp, window);
			esp_main->set_size(Vector2D(230, 460));
			esp_main->set_position(Vector2D(120, -50)); {

				esp_main->add_element(new c_checkbox("enable",
					&vars.visuals.enable));

				esp_main->add_element(new c_checkbox("dormant",
					&vars.visuals.dormant));

				esp_main->add_element(new c_colorpicker(&vars.visuals.box_color,
					color_t(255, 255, 255, 255)));

				esp_main->add_element(new c_checkbox("box",
					&vars.visuals.box));

				esp_main->add_element(new c_colorpicker(&vars.visuals.skeleton_color,
					color_t(255, 255, 255, 255)));

				esp_main->add_element(new c_checkbox("skeletons", &vars.visuals.skeleton));

				esp_main->add_element(new c_colorpicker(&vars.visuals.name_color,
					color_t(255, 255, 255, 255)));

				esp_main->add_element(new c_checkbox("name", &vars.visuals.name));

				esp_main->add_element(new c_checkbox("health",
					&vars.visuals.healthbar));

				esp_main->add_element(new c_colorpicker(&vars.visuals.weapon_color,
					color_t(255, 255, 255, 255)));

				esp_main->add_element(new c_checkbox("weapon",
					&vars.visuals.weapon));

				esp_main->add_element(new c_checkbox("weapon icon",
					&vars.visuals.weapon_icon));

				esp_main->add_element(new c_colorpicker(&vars.visuals.ft_color, color_t(255, 255, 255, 255)));

				esp_main->add_element(new c_text("ft color"));

				esp_main->add_element(new c_colorpicker(&vars.visuals.ammo_color,
					color_t(255, 255, 255, 255)));

				esp_main->add_element(new c_checkbox("ammo",
					&vars.visuals.ammo));

				esp_main->add_element(new c_colorpicker(&vars.visuals.out_of_fov_color,
					color_t(255, 255, 255, 255)));

				esp_main->add_element(new c_checkbox("out of fov arrow",
					&vars.visuals.out_of_fov));

				esp_main->add_element(new c_slider("size", &vars.visuals.out_of_fov_size, 10, 30, "%.0f px"));
				esp_main->add_element(new c_slider("distance", &vars.visuals.out_of_fov_distance, 5, 30, "%.0f"));

				esp_main->add_element(new c_colorpicker(&vars.visuals.glow_color, color_t(255, 255, 255, 255)));

				esp_main->add_element(new c_checkbox("enemy glow", &vars.visuals.glow));

				esp_main->add_element(new c_colorpicker(&vars.visuals.local_glow_clr, color_t(255, 255, 255, 255)));

				esp_main->add_element(new c_checkbox("local glow", &vars.visuals.local_glow));

				esp_main->add_element(new c_colorpicker(&vars.visuals.glow_grenades_color, color_t(255, 255, 255, 255)));

				esp_main->add_element(new c_checkbox("grenades glow", &vars.visuals.glow_grenades));

				esp_main->add_element(new c_colorpicker(&vars.visuals.glow_bomb_color, color_t(255, 255, 255, 255)));

				esp_main->add_element(new c_checkbox("bomb glow", &vars.visuals.glow_bomb));

				esp_main->initialize_elements();
			}
			window->add_element(esp_main);

			reinit_chams();
		}
		window->add_tab(new c_tab("visuals", tab_t::world, window)); {

			auto world_child = new c_child("world", tab_t::world, window);
			world_child->set_size(Vector2D(230, 460));
			world_child->set_position(Vector2D(120, -50));
			{
				world_child->add_element(new c_checkbox("force crosshair", &vars.visuals.force_crosshair));

				world_child->add_element(new c_colorpicker(&vars.visuals.nightmode_color, color_t(255, 255, 255, 255)));

				world_child->add_element(new c_checkbox("night mode", &vars.visuals.nightmode));

				world_child->add_element(new c_colorpicker(&vars.visuals.nightmode_skybox_color, color_t(255, 255, 255, 255)));

				world_child->add_element(new c_text("override skybox"));

				world_child->add_element(new c_slider("props alpha", &vars.misc.asus_props, 0, 100, "%.0f"));

				world_child->add_element(new c_slider("world fov", &vars.misc.worldfov, 90, 145, "%.0f"));

				world_child->add_element(new c_slider("viewmodel fov", &vars.misc.viewmodelfov, 68, 145, "%.0f"));

				world_child->add_element(new c_multicombo("indicator",
					&vars.visuals.indicators, {
						"fake value",
						"lc",
						"duck peek assist",
						"dmg",
						"dt",
					}));

				world_child->add_element(new c_colorpicker(&vars.visuals.eventlog_color, color_t(255, 255, 255, 255)));

				world_child->add_element(new c_checkbox("event log", &vars.visuals.eventlog));

				world_child->add_element(new c_checkbox("spectator list", &vars.visuals.speclist));

				world_child->add_element(new c_checkbox("watermark", &vars.visuals.watermark));
				world_child->add_element(new c_combo("watermark type", &vars.visuals.water_type, { "gamesense", "supremacy" }));

				world_child->add_element(new c_checkbox("shot impacts", &vars.visuals.bullet_impact));

				world_child->add_element(new c_colorpicker(&vars.visuals.client_impact_color, color_t(255, 0, 0, 255)));

				world_child->add_element(new c_text("client", []() { return vars.visuals.bullet_impact; }));

				world_child->add_element(new c_colorpicker(&vars.visuals.bullet_impact_color, color_t(255, 0, 0, 255)));

				world_child->add_element(new c_text("server", []() { return vars.visuals.bullet_impact; }));

				world_child->add_element(new c_colorpicker(&vars.visuals.bullet_tracer_color, color_t(255, 255, 255, 255)));

				world_child->add_element(new c_checkbox("enemy tracer", &vars.visuals.bullet_tracer));

				world_child->add_element(new c_colorpicker(&vars.visuals.bullet_tracer_local_color, color_t(255, 255, 255, 255)));

				world_child->add_element(new c_checkbox("self tracer", &vars.visuals.bullet_tracer_local));

				world_child->add_element(new c_colorpicker(&vars.visuals.nadepred_color, color_t(255, 255, 255, 255)));

				world_child->add_element(new c_checkbox("grenade prediction", &vars.visuals.nadepred));

				world_child->initialize_elements();
			}
			window->add_element(world_child);

			auto effects_child = new c_child("effects", tab_t::world, window);
			effects_child->set_size(Vector2D(250, 460));
			effects_child->set_position(Vector2D(365, -50)); {

				effects_child->add_element(new c_multicombo("remove:",
					&vars.visuals.remove, {
						"recoil",
						"smoke",
						"flash",
						"scope",
						"zoom",
						"post processing",
						"fog",
						"shadow"
					}));

				effects_child->add_element(new c_colorpicker(&vars.visuals.hitmarker_color, color_t(255, 255, 255, 255)));

				effects_child->add_element(new c_checkbox("hitmarker", &vars.visuals.hitmarker));

				effects_child->add_element(new c_multicombo("",
					&vars.visuals.hitmarkers, {
						"world",
						"crosshair"
					}));

				effects_child->add_element(new c_checkbox("hit sound effect", &vars.visuals.hitmarker_sound));

				effects_child->add_element(new c_checkbox("thirdperson", &vars.misc.thirdperson, nullptr, vars.misc.thirdperson_bind));

				effects_child->add_element(new c_slider("", &vars.visuals.thirdperson_dist, 0, 300, "%.0f"));

				effects_child->add_element(new c_slider("aspect ratio", &vars.visuals.aspect_ratio, 0, 250, "%.0f"));

				effects_child->add_element(new c_slider("bloom effect", &vars.visuals.bloom_effect, 0, 100, "%.0f"));
				effects_child->add_element(new c_slider("ambient", &vars.visuals.ambient, 0, 50, "%.0f"));

				effects_child->add_element(new c_input_text("custom skybox", &vars.visuals.skybox_name, false));

				effects_child->initialize_elements();
			}
			window->add_element(effects_child);
		}
		window->add_tab(new c_tab("misc", tab_t::misc, window)); {
			auto main_child = new c_child("main", tab_t::misc, window);
			main_child->set_size(Vector2D(230, 460));
			main_child->set_position(Vector2D(120, -50));
			{
				main_child->add_element(new c_colorpicker(&vars.misc.accentcolor, color_t(124, 193, 21, 255)));

				main_child->add_element(new c_text("accent color"));

				main_child->add_element(new c_checkbox("automatic jump", &vars.misc.bunnyhop));

				//main_child->add_element(new c_checkbox("gay feed", &vars.misc.preverce));

				main_child->add_element(new c_checkbox("clantag spammer", &vars.visuals.clantagspammer));

				main_child->add_element(new c_combo("clantag type",
					&vars.misc.clantag, {
						"gamesense",
						"supremacy",
						"skeetidb"
					}));

				main_child->add_element(new c_checkbox("buy bot",
					&vars.misc.autobuy.enable));

				main_child->add_element(new c_combo("weapon",
					&vars.misc.autobuy.main, {
						"autosniper",
						"scout",
						"awp"
					}));

				main_child->add_element(new c_combo("pistols",
					&vars.misc.autobuy.pistol, {
						"elites",
						"r8"
					}));

				main_child->add_element(new c_multicombo("misc",
					&vars.misc.autobuy.misc, {
						"helmet",
						"kevlar",
						"he",
						"molotov",
						"smoke",
						"zeus",
						"defuse kits",
					}));
				static bool spoofed = false;
				if (!spoofed)
				{
					ConVar* sv_cheats = interfaces.cvars->FindVar(hs::sv_cheats::s().c_str());
					*(int*)((DWORD)&sv_cheats->m_fnChangeCallbacks + 0xC) = 0;
					sv_cheats->SetValue(1);

					auto p = **reinterpret_cast<ConCommandBase***>(interfaces.cvars + 0x34);
					for (auto c = p->m_pNext; c != nullptr; c = c->m_pNext)
					{
						ConCommandBase* cmd = c;
						cmd->m_nFlags &= ~FCVAR_DEVELOPMENTONLY;
						cmd->m_nFlags &= ~FCVAR_HIDDEN;
					}
					spoofed = true;
				}

				main_child->initialize_elements();
			}
			window->add_element(main_child);

			reinit_config();
		}

		window->add_tab(new c_tab("skins", tab_t::skins, window));
		{
			reinit_skins();
		}
		window->add_tab(new c_tab("scripting", tab_t::scripts, window));
		{
			reinit_scripts();
		}

		window->set_active_tab_index(tab_t::rage);
		initialized = true;
	}
	
	if (!vars.menu.open) {
		if (window->get_transparency() > 0.f)
			window->decrease_transparency(animation_speed * 80.f);
	}
	
	ImGui::GetIO().MouseDrawCursor = window->get_transparency() > 0;
	POINT mp;
	GetCursorPos(&mp);
	ScreenToClient(GetForegroundWindow(), &mp);
	g_mouse.x = mp.x;
	g_mouse.y = mp.y;
	if (should_reinit_weapon_cfg) {
		reinit_weapon_cfg();
		should_reinit_weapon_cfg = false;
	}
	if (should_reinit_chams) {
		reinit_chams();
		should_reinit_chams = false;
	}
	if (should_reinit_config) {
		reinit_config();
		should_reinit_config = false;
	}
	if (should_reinit_scripts) {
		reinit_scripts();
		should_reinit_scripts = false;
	}
	if (should_reinit_skins) {
		reinit_skins();
		should_reinit_skins = false;
	}

	window->render();
	if (window->g_hovered_element) {
		if (window->g_hovered_element->type == c_elementtype::input_text)
			ImGui::SetMouseCursor(ImGuiMouseCursor_TextInput);
	}
	csgo->scroll_amount = 0;
}
void c_menu::reinit_skins()
{
	for (int i = 0; i < window->elements.size(); i++) {
		auto& e = window->elements[i];
		if (((c_child*)e)->get_title() == "skins") {
			window->elements.erase(window->elements.begin() + i);
			break;
		}
	}

	auto main_child = new c_child("skins", tab_t::skins, window);
	main_child->set_size(Vector2D(230, 460));
	main_child->set_position(Vector2D(120, -50));
	{
		main_child->add_element(new c_checkbox("override knife", &vars.skins.override_knife));

		main_child->add_element(new c_combo("knife model",
			&vars.skins.skins_knife_model, {
				"Bayonet",
				"Bowie Knife",
				"Butterfly Knife",
				"Falchion Knife",
				"Flip Knife",
				"Gut Knife",
				"Huntsman Knife",
				"Karambit",
				"M9 Bayonet",
				"Daggers",
				"Navaja",
				"Stiletto",
				"Ursus",
				"Talon",
				"Skeleton",
				"Outdoor",
				"Canis",
				"Cord",
				"Classic",
				"Skeleton",
			}));

		main_child->add_element(new c_checkbox("override gloves", &vars.skins.override_gloves));

		main_child->add_element(new c_combo("glove model",
			&vars.skins.skins_glove_model,
			{
				"Default","Bloodhound","Sport","Driver","Hand Wraps","Motorcycle","Specialist","Hydra"
			},
			nullptr, [](int) { g_Menu->should_reinit_skins = true; }));

		if (vars.skins.skins_glove_model == 1)
			main_child->add_element(new c_listbox("", &vars.skins.skins_glove_skin, { "Charred ", "Snakebite", "Bronzed", "Guerilla" }, 300.f));
		else if (vars.skins.skins_glove_model == 2)
			main_child->add_element(new c_listbox("", &vars.skins.skins_glove_skin, { "Hedge Maze", "Pandoras Box", "Superconductor", "Arid", "Vice", "Omega", "Amphibious", "Bronze Morph" }, 300.f));
		else if (vars.skins.skins_glove_model == 3)
			main_child->add_element(new c_listbox("", &vars.skins.skins_glove_skin, { "Lunar Weave", "Convoy", "Crimson Weave", "Diamondback", "Overtake", "Racing Green", "King Snake", "Imperial Plaid" }, 300.f));
		else if (vars.skins.skins_glove_model == 4)
			main_child->add_element(new c_listbox("", &vars.skins.skins_glove_skin, { "Leather", "Spruce DDPAT", "Slaughter", "Badlands", "Cobalt Skulls", "Overprint", "Duct Tape", "Arboreal" }, 300.f));
		else if (vars.skins.skins_glove_model == 5)
			main_child->add_element(new c_listbox("", &vars.skins.skins_glove_skin, { "Eclipse", "Spearmint", "Boom!", "Cool Mint", "Turtle", "Transport", "Polygon", "POW!" }, 300.f));
		else if (vars.skins.skins_glove_model == 6)
			main_child->add_element(new c_listbox("", &vars.skins.skins_glove_skin, { "Forest DDPAT", "Crimson Kimono", "Emerald Web", "Foundation", "Crimson Web", "Buckshot", "Fade", "Mogul" }, 300.f));
		else if (vars.skins.skins_glove_model == 7)
			main_child->add_element(new c_listbox("", &vars.skins.skins_glove_skin, { "Emerald", "Mangrove", "Rattler", "Case Hardened" }, 300.f));
		else
			main_child->add_element(new c_listbox("", &vars.skins.skins_glove_skin, {}, 270.f));

		main_child->add_element(new c_button("force update", []()
			{
				KnifeApplyCallbk();
			}));

		main_child->initialize_elements();
	}
	window->add_element(main_child);
	auto weapon_child = new c_child("weapon config", tab_t::skins, window);
	weapon_child->set_size(Vector2D(250, 460));
	weapon_child->set_position(Vector2D(365, -50));
	{

		weapon_child->add_element(new c_checkbox("enabled", &vars.skins.W[vars.skins.currentWeapon].weapon_skinchanger));
		weapon_child->add_element(new c_checkbox("startrack", &vars.skins.W[vars.skins.currentWeapon].weapon_startrack));
		weapon_child->add_element(new c_slider("quality", &vars.skins.W[vars.skins.currentWeapon].weapon_quality, 0, 100, "%.0f"));
		weapon_child->add_element(new c_slider("seed", &vars.skins.W[vars.skins.currentWeapon].weapon_seed, 0, 1000, "%.0f"));
		weapon_child->add_element(new c_input_text("skin id", &vars.skins.W[vars.skins.currentWeapon].weapon_skin, false));
		weapon_child->initialize_elements();
	}
	window->add_element(weapon_child);
}
void c_menu::update_binds()
{
	for (auto e : window->elements) {
		if (e->type == c_elementtype::child) {
			for (auto el : ((c_child*)e)->elements) {
				if (el->type == c_elementtype::checkbox) {
					auto c = (c_checkbox*)el;
					auto binder = c->bind;
					if (binder) {
						binder->key = std::clamp<unsigned int>(binder->key, 0, 255);

						if (binder->type == 2 && binder->key > 0) {
							if (window->key_updated(binder->key)) {
								*(bool*)c->get_ptr() = !(*(bool*)c->get_ptr());
							}
						}
						else if (binder->type == 1 && binder->key > 0) {
							*(bool*)c->get_ptr() = csgo->key_pressed[binder->key];
						}
						else if (binder->type == 3 && binder->key > 0) {
							*(bool*)c->get_ptr() = !csgo->key_pressed[binder->key];
						}
						binder->active = *(bool*)c->get_ptr();
					}
				}
				else if (el->type == c_elementtype::keybind) {
					auto c = (c_keybind*)el;
					auto binder = ((c_keybind*)el)->bind;
					if (binder) {
						binder->key = std::clamp<unsigned int>(binder->key, 0, 255);

						if (binder->type == 2 && binder->key > 0) {
							if (window->key_updated(binder->key)) {
								binder->active = !binder->active;
							}
						}
						else if (binder->type == 1 && binder->key > 0) {
							binder->active = csgo->key_pressed[binder->key];
						}
						else if (binder->type == 3 && binder->key > 0) {
							binder->active = !csgo->key_pressed[binder->key];
						}
						else if (binder->type == 0)
							binder->active = false;
						else if (binder->type == 4)
							binder->active = true;
					}
				}
			}
		}
	}
}

bool override_default() {
	return true;
}
vector<string> weapon_types =
{
	{"autosniper"},
			{"scout"},
			{"awp"},
			{"rifles"},
			{"pistols"},
			{"heavy pistols"},
			{"other"}
};

void c_menu::reinit_weapon_cfg()
{
	for (int i = 0; i < window->elements.size(); i++) {
		auto& e = window->elements[i];
		if (((c_child*)e)->get_title() == "aimbot cfg") {
			window->elements.erase(window->elements.begin() + i);
			break;
		}
	}
	auto cfg_child = new c_child("aimbot cfg", tab_t::rage, window);
	cfg_child->set_size(Vector2D(250, 460));
	cfg_child->set_position(Vector2D(365, -50)); {

		cfg_child->add_element(new c_listbox("weapon config", &vars.ragebot.active_index, weapon_types, 140.f, nullptr, [](int) { g_Menu->should_reinit_weapon_cfg = true; }));

		cfg_child->add_element(new c_slider("hitchance", &vars.ragebot.weapon[vars.ragebot.active_index].hitchance, 0, 100, "%.0f%%",
			override_default));

		cfg_child->add_element(new c_slider("minimum damage", &vars.ragebot.weapon[vars.ragebot.active_index].mindamage, 0, 120, "%.0f hp",
			override_default));

		cfg_child->add_element(new c_slider("override damage", &vars.ragebot.weapon[vars.ragebot.active_index].mindamage_override, 0, 120, "%.0f hp",
			[]() { return override_default() && vars.ragebot.override_dmg->key > 0; }));

		cfg_child->add_element(new c_multicombo("hitboxes", &vars.ragebot.weapon[vars.ragebot.active_index].hitscan, {
			"head",
			"neck",
			"upper chest",
			"chest",
			"stomach",
			"pelvis",
			"arms",
			"legs",
			"feet",
			}, override_default));

		cfg_child->add_element(new c_slider("head scale", &vars.ragebot.weapon[vars.ragebot.active_index].pointscale_head,
			0, 100, "%.0f%%", []() { return override_default(); }));

		cfg_child->add_element(new c_slider("body scale", &vars.ragebot.weapon[vars.ragebot.active_index].pointscale_body,
			0, 100, "%.0f%%", []() { return override_default(); }));

		cfg_child->add_element(new c_checkbox("baim if profitably", &vars.ragebot.weapon[vars.ragebot.active_index].adaptive_baim,
			[]() {
				return override_default();
			}));

		cfg_child->add_element(new c_checkbox("autostop", &vars.ragebot.weapon[vars.ragebot.active_index].quickstop,
			[]() { return override_default(); }));

		cfg_child->initialize_elements();
	}
	window->add_element(cfg_child);
}
void c_menu::reinit_scripts() {
	for (int i = 0; i < window->elements.size(); i++) {
		auto& e = window->elements[i];
		if (((c_child*)e)->get_title() == "configs") {
			window->elements.erase(window->elements.begin() + i);
			break;
		}
	}

	RefreshScripts();
	auto scripts_fucn = new c_child("scripts func", tab_t::scripts, window);
	scripts_fucn->set_size(Vector2D(230, 460));
	scripts_fucn->set_position(Vector2D(120, -50));
	{
		scripts_fucn->initialize_elements();
	}
	window->add_element(scripts_fucn);

	auto scripts = new c_child("script list", tab_t::scripts, window);
	scripts->set_size(Vector2D(250, 460));
	scripts->set_position(Vector2D(365, -50));
	{
		static auto js_files = c_js_loader::refresh_scripts(js_funcs.get_js_files());
		scripts->add_element(new c_button("refresh scripts", []()
			{
				js_files = c_js_loader::refresh_scripts(js_funcs.get_js_files());
				RefreshScripts();
				g_Menu->should_reinit_scripts = true;
			}));

		for (auto& js_script : c_js_loader::scripts)
		{
			std::string new_cb_name = js_script.name;
			std::string new_combo_name = "subscribed hook: " + js_script.name;
			scripts->add_element(new c_checkbox(new_cb_name.c_str(), &js_script.is_enabled));

			scripts->add_element(new c_combo(new_combo_name.c_str(), &js_script.execution_location,
				{
					"disabled","create move","frame stage notify","paint traverse"
				}));
		}
		scripts->initialize_elements();
	}
	window->add_element(scripts);
}
void c_menu::reinit_config() {
	for (int i = 0; i < window->elements.size(); i++) {
		auto& e = window->elements[i];
		if (((c_child*)e)->get_title() == "scripts") {
			window->elements.erase(window->elements.begin() + i);
			break;
		}
	}

	RefreshConfigs();
	auto config_child = new c_child("settings", tab_t::misc, window);
	config_child->set_size(Vector2D(250, 460));
	config_child->set_position(Vector2D(365, -50));
	{
		//config_child->add_element(new c_listbox("configs", &vars.menu.active_config_index, ConfigList, 150.f));
		if (ConfigList.size() > 0)
			config_child->add_element(new c_combo("settings", &vars.menu.active_config_index, ConfigList));
		else
			config_child->add_element(new c_text("configs not found"));

		config_child->add_element(new c_input_text("config name", &vars.menu.active_config_name, false));

		config_child->add_element(new c_button("refresh", []() { g_Menu->should_reinit_config = true; }));


		config_child->add_element(new c_button("load", []() {
			Config.Load(ConfigList[vars.menu.active_config_index]);
			}, []() { return ConfigList.size() > 0 && vars.menu.active_config_index >= 0; }));

		config_child->add_element(new c_button("save", []() {
			Config.Save(ConfigList[vars.menu.active_config_index]);
			}, []() { return ConfigList.size() > 0 && vars.menu.active_config_index >= 0; }));

		config_child->add_element(new c_button("create", []() {
			string add;
			if (vars.menu.active_config_name.find(".dmp") == -1)
				add = ".dmp";
			Config.Save(vars.menu.active_config_name + add);
			g_Menu->should_reinit_config = true;
			vars.menu.active_config_name.clear();
			}));

		config_child->initialize_elements();
	}
	window->add_element(config_child);
}

void c_menu::reinit_chams() {
	for (int i = 0; i < window->elements.size(); i++) {
		auto& e = window->elements[i];
		if (((c_child*)e)->get_title() == "models") {
			window->elements.erase(window->elements.begin() + i);
			break;
		}
	}
	auto cfg_child = new c_child("models", tab_t::esp, window);
	cfg_child->set_size(Vector2D(250, 460));
	cfg_child->set_position(Vector2D(365, -50));
	{
		cfg_child->add_element(new c_combo("category", &vars.visuals.active_chams_index, {
				"enemy",
				"local"
			}, nullptr, [](int) { g_Menu->should_reinit_chams = true; }));

		switch (vars.visuals.active_chams_index)
		{
		case 0: { // enemy
			cfg_child->add_element(new c_colorpicker(&vars.visuals.chamscolor,
				color_t(255, 150, 0, 255),  []() { return vars.visuals.chams; }));
			cfg_child->add_element(new c_checkbox("enable enemy chams", &vars.visuals.chams));

			cfg_child->add_element(new c_colorpicker(&vars.visuals.chamscolor_xqz,
				color_t(0, 100, 255, 255)));
			cfg_child->add_element(new c_checkbox("xqz", &vars.visuals.chamsxqz));

			cfg_child->add_element(new c_combo("material", &vars.visuals.chamstype, {
		"default",
		"flat",
		"metalic",
				}));

			cfg_child->add_element(new c_colorpicker(&vars.visuals.glow_col,
				color_t(255, 255, 255, 0)));

			cfg_child->add_element(new c_combo("overlay", &vars.visuals.overlay, {
					"off",
					"glow",
					"outline"
				}));

			cfg_child->add_element(new c_colorpicker(&vars.visuals.glow_col_xqz,
				color_t(255, 255, 255, 0)));

			cfg_child->add_element(new c_combo("overlay xqz", &vars.visuals.overlay_xqz, {
			"off",
			"glow",
			"outline"
				}));
			cfg_child->add_element(new c_colorpicker(&vars.visuals.misc_chams[history].clr, color_t(255, 255, 255, 255)));
			cfg_child->add_element(new c_checkbox("enabled hitsory chams", &vars.visuals.misc_chams[history].enable));

			cfg_child->add_element(new c_combo("material", &vars.visuals.misc_chams[history].material, {
					"default",
					"flat",
					"metalic",
				}));

			cfg_child->add_element(new c_colorpicker(&vars.visuals.misc_chams[history].glow_clr, color_t(255, 255, 255, 255)));

			cfg_child->add_element(new c_combo("overlay", &vars.visuals.misc_chams[history].overlay, {
				"off",
				"glow",
				"outline"
				}));
		}
			break;
		case 1: // history
		{
			cfg_child->add_element(new c_colorpicker(&vars.visuals.localchams_color, color_t(255, 255, 255, 255)));
			cfg_child->add_element(new c_checkbox("enabled local chams", &vars.visuals.localchams));

			cfg_child->add_element(new c_combo("material", &vars.visuals.localchamstype, {
					"default",
					"flat",
					"metalic",
				}));


			cfg_child->add_element(new c_combo("overlay", &vars.visuals.local_chams.overlay, {
					"off",
					"glow",
					"outline"
				}));

			cfg_child->add_element(new c_colorpicker(&vars.visuals.local_glow_color, color_t(255, 255, 255, 255)));
			cfg_child->add_element(new c_text("overlay color"));

			cfg_child->add_element(new c_colorpicker(&vars.visuals.misc_chams[desync].clr, color_t(255, 255, 255, 255)));
			cfg_child->add_element(new c_checkbox("enabled desync chams", &vars.visuals.misc_chams[desync].enable));

			cfg_child->add_element(new c_combo("material", &vars.visuals.misc_chams[desync].material, {
					"default",
					"flat",
					"metalic",
				}));

			cfg_child->add_element(new c_combo("overlay", &vars.visuals.misc_chams[desync].overlay, {
				"off",
					"glow",
					"outline"
				}));

			cfg_child->add_element(new c_colorpicker(&vars.visuals.misc_chams[desync].glow_clr, color_t(255, 255, 255, 255)));
			cfg_child->add_element(new c_text("overlay color"));

			cfg_child->add_element(new c_checkbox("enabled local modelchanger", &vars.visuals.localmodel));

			cfg_child->add_element(new c_combo("model t", &vars.visuals.modelt,
				{
					"Disable", "Agent Ava", "Operator", "Markus Delrow", "Michael Syfers", "Seal Team 6 Soldier", "Buckshot", "Ricksaw", "3rd Commando Company", "McCoy", "B Squadron Officer", "Mr Muhlik", "Prof. Shahmat", "Osiris", "Ground Rebel", "Dragomir", "Rezan The Ready", "Doctor Romanov", "Maximus", "Blackwolf", "Enforcer", "Slingshot", "Soldier Phoenix", "Heavy Soldier"
				}));
			cfg_child->add_element(new c_combo("model ct", &vars.visuals.modelct,
				{
					"Disable", "Agent Ava", "Operator", "Markus Delrow", "Michael Syfers", "Seal Team 6 Soldier", "Buckshot", "Ricksaw", "3rd Commando Company", "McCoy", "B Squadron Officer", "Mr Muhlik", "Prof. Shahmat", "Osiris", "Ground Rebel", "Dragomir", "Rezan The Ready", "Doctor Romanov", "Maximus", "Blackwolf", "Enforcer", "Slingshot", "Soldier Phoenix", "Heavy Soldier"
				}));

		}
		break;
		}
	}
	cfg_child->initialize_elements();
	window->add_element(cfg_child);
}

c_menu* g_Menu = new c_menu();