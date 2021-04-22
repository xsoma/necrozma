#include "Variables.h"
#include "Hooks.h"
#include "aes256/aes256.hpp"
#include <sstream>


unsigned char enc_key[32] = { 0x1f, 0x01, 0x02, 0x03, 0x04, 0x13, 0x05, 0x06, 0x07,0x17, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
									  0x10, 0x11, 0x12, 0x14, 0x15, 0x16, 0x18, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x19, 0x00 };


ByteArray encryption_key;

void CConfig::ReadConfigs(LPCTSTR lpszFileName)
{
	ConfigList.push_back(lpszFileName);
}
void CConfig::ResetToDefault()
{
	vars.ragebot.enable = false;
	vars.ragebot.autoscope = false;
	vars.ragebot.autoshoot = false;
	vars.ragebot.hitchance_consider_hitbox = false;
	vars.ragebot.posadj = false;
	vars.ragebot.backshoot_bt = false;
	vars.ragebot.resolver = false;
	vars.ragebot.onshot_invert = false;
	vars.ragebot.onshot_resolver = false;
	vars.ragebot.base_angle = false;
	vars.ragebot.shotrecord = false;
	vars.ragebot.shot_clr = color_t(255, 255, 255, 255);

	vars.ragebot.override_dmg = new c_bind();
	vars.ragebot.override_dmg->key = 0;
	vars.ragebot.override_dmg->active = false;
	vars.ragebot.override_dmg->type = 1;

	vars.ragebot.force_body = new c_bind();
	vars.ragebot.force_body->key = 0;
	vars.ragebot.force_body->active = false;
	vars.ragebot.force_body->type = 1;

	vars.ragebot.double_tap = new c_bind();
	vars.ragebot.double_tap->active = false;
	vars.ragebot.double_tap->key = 0;
	vars.ragebot.double_tap->type = 1;
	vars.ragebot.hide_shots = new c_bind();
	vars.ragebot.hide_shots->active = false;
	vars.ragebot.hide_shots->key = 0;
	vars.ragebot.hide_shots->type = 1;

	vars.ragebot.dt_instant = false;

	for (auto a : vars.ragebot.weapon) {
		a.enable = true;
		a.mindamage = 0;
		a.mindamage_override = 0;
		a.hitchance = 0;
		a.quickstop = false;
		a.pointscale_head = 0.f;
		a.pointscale_body = 0.f;
		a.pointscale_limbs = 0.f;
		a.hitscan = 0;
		a.hitscan_baim = 0;
		a.baim = 0;
		a.baim_under_hp = 0;
		a.max_misses = 0;
		a.adaptive_baim = false;
	}
	
	vars.ragebot.dt_charge_delay = 0;
	vars.ragebot.doubletap = false;
	vars.ragebot.hideshot = false;

	vars.antiaim.enable = false;
	
	vars.antiaim.aa_override.enable = false;

	vars.antiaim.aa_override.left = new c_bind();
	vars.antiaim.aa_override.left->active = false;
	vars.antiaim.aa_override.left->key = 0;
	vars.antiaim.aa_override.left->type = 1;

	vars.antiaim.aa_override.right = new c_bind();
	vars.antiaim.aa_override.right->active = false;
	vars.antiaim.aa_override.right->key = 0;
	vars.antiaim.aa_override.right->type = 1;

	vars.antiaim.aa_override.back = new c_bind();
	vars.antiaim.aa_override.back->active = false;
	vars.antiaim.aa_override.back->key = 0;
	vars.antiaim.aa_override.back->type = 1;

	vars.antiaim.fakelag = 0;
	vars.antiaim.fakelagfactor = 1;
	vars.antiaim.fakelagvariance = 25;
	vars.antiaim.fakelag_onshot = false;

	vars.antiaim.fakeduck = new c_bind();
	vars.antiaim.fakeduck->active = false;
	vars.antiaim.fakeduck->key = 0;
	vars.antiaim.fakeduck->type = 1;

	vars.antiaim.slowwalk = new c_bind();
	vars.antiaim.slowwalk->active = false;
	vars.antiaim.slowwalk->key = 0;
	vars.antiaim.slowwalk->type = 1;

	vars.visuals.enable = false;
	vars.visuals.dormant = false;
	vars.visuals.skeleton = false;
	vars.visuals.skeleton_color = color_t(255, 255, 255, 255);
	vars.visuals.box = false;
	vars.visuals.box_color = color_t(255, 255, 255, 255);
	vars.visuals.healthbar = false;
	vars.visuals.override_hp = false;
	vars.visuals.hp_color = color_t(0, 255, 0, 255);
	vars.visuals.weapon = false;
	vars.visuals.weapon_color = color_t(255, 255, 255, 255);
	vars.visuals.name_color = color_t(255, 255, 255, 255);
	vars.visuals.ammo = false;
	vars.visuals.ammo_color = color_t(255, 255, 255, 255);
	vars.visuals.flags = 0;
	vars.visuals.flags_color = color_t(255, 255, 255, 255);
	vars.visuals.out_of_fov = false;
	vars.visuals.out_of_fov_distance = 30;
	vars.visuals.out_of_fov_size = 30;
	vars.visuals.out_of_fov_color = color_t(255, 255, 255, 255);
	vars.visuals.hitmarker = false;
	vars.visuals.hitmarker_sound = false;
	vars.visuals.hitmarker_color = color_t(255, 255, 255, 255);
	vars.visuals.speclist = false;
	vars.visuals.monitor = false;
	vars.visuals.nadepred = false;
	vars.visuals.nadepred_color = color_t(255, 255, 255, 255);
	vars.misc.thirdperson = false;

	vars.misc.thirdperson_bind = new c_bind();
	vars.misc.thirdperson_bind->active = false;
	vars.misc.thirdperson_bind->key = 0;
	vars.misc.thirdperson_bind->type = 2;

	vars.visuals.thirdperson_dist = 100;
	vars.visuals.aspect_ratio = 0;
	vars.visuals.indicators = 0;
	vars.visuals.eventlog = false;
	vars.visuals.eventlog_color = color_t(255, 255, 255, 255);
	vars.visuals.bullet_tracer = false;
	vars.visuals.bullet_tracer_type = 0;
	vars.visuals.bullet_tracer_color = color_t(255, 255, 255, 255);
	vars.visuals.bullet_impact = false;
	vars.visuals.bullet_impact_color = color_t(0, 0, 255, 175);
	vars.visuals.client_impact_color = color_t(255, 0, 0, 175);
	vars.visuals.remove = 0;
	vars.visuals.nightmode = false;
	vars.visuals.nightmode_color = color_t(255, 255, 255, 255);
	vars.visuals.nightmode_prop_color = color_t(255, 255, 255, 255);
	vars.visuals.nightmode_skybox_color = color_t(255, 255, 255, 255);
	vars.visuals.kill_effect = false;
	vars.misc.worldfov = 90;
	vars.misc.viewmodelfov = 68;

	vars.visuals.impacts_size = 2.f;

	vars.visuals.chams = false;
	vars.visuals.chamsxqz = false;
	vars.visuals.glow = false;
	vars.visuals.glowtype = 0;
	vars.visuals.local_glow = false;
	vars.visuals.glow_color = color_t(255, 255, 255, 255);
	vars.visuals.local_glow_clr = color_t(255, 255, 255, 255);
	vars.visuals.chamscolor = color_t(255, 255, 255, 255);
	vars.visuals.chamscolor_xqz = color_t(255, 255, 255, 255);
	vars.visuals.glow_col = color_t(255, 255, 255, 255);
	vars.visuals.glow_col_xqz = color_t(255, 255, 255, 255);
	vars.visuals.chamstype = 0;
	vars.visuals.overlay = 0;
	vars.visuals.overlay_xqz = 0;

	vars.visuals.localchams = false;
	vars.visuals.localchams_color = color_t(255, 255, 255, 255);
	vars.visuals.localchams_color_glow = color_t(255, 255, 255, 255);
	vars.visuals.bullet_tracer_local_color = color_t(255, 255, 255, 255);
	vars.visuals.localchamstype = 0;
	vars.visuals.local_chams.overlay = 0;

	for (int i = 0; i < 4; ++i) {
		vars.visuals.misc_chams[i].enable = false;
		vars.visuals.misc_chams[i].clr = color_t(255, 255, 255, 255);
		vars.visuals.misc_chams[i].glow_clr = color_t(255, 255, 255, 255);
		vars.visuals.misc_chams[i].material = 0;
		vars.visuals.misc_chams[i].overlay = 0;
	}
	vars.misc.antiuntrusted = true;
	vars.misc.bunnyhop = false;
	vars.misc.knifebot = false;
	vars.visuals.clantagspammer = false;
	vars.misc.hold_firinganims = false;

	vars.misc.accentcolor = color_t(124, 193, 21, 255);

	vars.misc.autobuy.enable = false;
	vars.misc.autobuy.main = 0;
	vars.misc.autobuy.misc = 0;
	vars.misc.autobuy.pistol = 0;
}

void CConfig::Setup()
{
	for (unsigned char i = 0; i < 32; i++)
		encryption_key.push_back(enc_key[i]);
	ResetToDefault();
};

string CConfig::GetModuleFilePath(HMODULE hModule)
{
	string ModuleName = "";
	char szFileName[MAX_PATH] = { 0 };

	if (GetModuleFileNameA(hModule, szFileName, MAX_PATH))
		ModuleName = szFileName;

	return ModuleName;
}

string CConfig::GetModuleBaseDir(HMODULE hModule)
{
	string ModulePath = GetModuleFilePath(hModule);
	return ModulePath.substr(0, ModulePath.find_last_of("\\/"));
}

void CConfig::Save(string cfg_name)
{
	json_t configuration;

	auto& json = configuration["configuration"];
	json["configuration_name"] = cfg_name;

	auto& ragebot = json["main_config"];
	ragebot["enable"] = vars.ragebot.enable;
	ragebot["auto_shoot"] = vars.ragebot.autoshoot;
	ragebot["auto_scope"] = vars.ragebot.autoscope;
	ragebot["backtrack"] = vars.ragebot.posadj;
	ragebot["resolver"] = vars.ragebot.resolver;
	ragebot["doubletap_type"] = vars.ragebot.dttype;

	SaveBind(vars.ragebot.override_dmg, "override_dmg", &ragebot);
	SaveBind(vars.ragebot.double_tap, "double_tap", &ragebot);
	SaveBind(vars.ragebot.hide_shots, "hide_shots", &ragebot);
	SaveBind(vars.ragebot.force_body, "force_body", &ragebot);

	auto& weapon_cfg = ragebot["weapons"];
	for (int i = 0; i < 7; i++) {
		string category = "weapon_" + std::to_string(i);
		weapon_cfg[category]["enable"] = vars.ragebot.weapon[i].enable;
		weapon_cfg[category]["mindamage"] = vars.ragebot.weapon[i].mindamage;
		weapon_cfg[category]["mindamage_override"] = vars.ragebot.weapon[i].mindamage_override;
		weapon_cfg[category]["hitchance"] = vars.ragebot.weapon[i].hitchance;
		weapon_cfg[category]["quickstop"] = vars.ragebot.weapon[i].quickstop;
		weapon_cfg[category]["pointscale_head"] = vars.ragebot.weapon[i].pointscale_head;
		weapon_cfg[category]["pointscale_body"] = vars.ragebot.weapon[i].pointscale_body;
		weapon_cfg[category]["hitscan"] = vars.ragebot.weapon[i].hitscan;
		weapon_cfg[category]["hitscan_baim"] = vars.ragebot.weapon[i].hitscan_baim;
		weapon_cfg[category]["baim"] = vars.ragebot.weapon[i].baim;
		weapon_cfg[category]["baim_under_hp"] = vars.ragebot.weapon[i].baim_under_hp;
		weapon_cfg[category]["adaptive_baim"] = vars.ragebot.weapon[i].adaptive_baim;
	}


	ragebot["enable"] = vars.antiaim.enable;
	ragebot["pitch"] = vars.antiaim.pitch;
	ragebot["yaw"] = vars.antiaim.yaw;
	ragebot["desync_type"] = vars.antiaim.desync_type;
	ragebot["attargets"] = vars.antiaim.attargets;
	SaveBind(vars.antiaim.fakeduck, "fakeduck", &ragebot);
	SaveBind(vars.antiaim.slowwalk, "slowwalk", &ragebot);
	SaveBind(vars.antiaim.inverter, "inverter", &ragebot);

	ragebot["blin"] = vars.antiaim.blin;
	ragebot["inverterblin"] = vars.antiaim.inverterblin;

	ragebot["enable"] = vars.antiaim.aa_override.enable;
	SaveBind(vars.antiaim.aa_override.left, "left", &ragebot);
	SaveBind(vars.antiaim.aa_override.right, "right", &ragebot);
	SaveBind(vars.antiaim.aa_override.back, "back", &ragebot);


	ragebot["type"] = vars.antiaim.fakelag;
	ragebot["factor"] = vars.antiaim.fakelagfactor;
	ragebot["variance"] = vars.antiaim.fakelagvariance;
	ragebot["fakelag_onshot"] = vars.antiaim.fakelag_onshot;



	ragebot["enable"] = vars.visuals.enable;
	ragebot["dormant"] = vars.visuals.dormant;

	ragebot["box"] = vars.visuals.box; {
		ragebot["box_color"]["red"] = vars.visuals.box_color.get_red();
		ragebot["box_color"]["green"] = vars.visuals.box_color.get_green();
		ragebot["box_color"]["blue"] = vars.visuals.box_color.get_blue();
		ragebot["box_color"]["alpha"] = vars.visuals.box_color.get_alpha();
	}

	ragebot["skeleton"] = vars.visuals.skeleton; {
		ragebot["skeleton_color"]["red"] = vars.visuals.skeleton_color.get_red();
		ragebot["skeleton_color"]["green"] = vars.visuals.skeleton_color.get_green();
		ragebot["skeleton_color"]["blue"] = vars.visuals.skeleton_color.get_blue();
		ragebot["skeleton_color"]["alpha"] = vars.visuals.skeleton_color.get_alpha();
	}

	ragebot["healthbar"]["enable"] = vars.visuals.healthbar;

	ragebot["name"] = vars.visuals.name; {
		ragebot["name_color"]["red"] = vars.visuals.name_color.get_red();
		ragebot["name_color"]["green"] = vars.visuals.name_color.get_green();
		ragebot["name_color"]["blue"] = vars.visuals.name_color.get_blue();
		ragebot["name_color"]["alpha"] = vars.visuals.name_color.get_alpha();
	}
	ragebot["weapon_icon"] = vars.visuals.weapon_icon;
	ragebot["weapon"] = vars.visuals.weapon;
	{
		ragebot["weapon_color"]["red"] = vars.visuals.weapon_color.get_red();
		ragebot["weapon_color"]["green"] = vars.visuals.weapon_color.get_green();
		ragebot["weapon_color"]["blue"] = vars.visuals.weapon_color.get_blue();
		ragebot["weapon_color"]["alpha"] = vars.visuals.weapon_color.get_alpha();
	}

	ragebot["ammo"] = vars.visuals.ammo; {
		ragebot["ammo_color"]["red"] = vars.visuals.ammo_color.get_red();
		ragebot["ammo_color"]["green"] = vars.visuals.ammo_color.get_green();
		ragebot["ammo_color"]["blue"] = vars.visuals.ammo_color.get_blue();
		ragebot["ammo_color"]["alpha"] = vars.visuals.ammo_color.get_alpha();
	}

	ragebot["flags"] = vars.visuals.flags; {
		ragebot["flags_color"]["red"] = vars.visuals.flags_color.get_red();
		ragebot["flags_color"]["green"] = vars.visuals.flags_color.get_green();
		ragebot["flags_color"]["blue"] = vars.visuals.flags_color.get_blue();
		ragebot["flags_color"]["alpha"] = vars.visuals.flags_color.get_alpha();
	}
	ragebot["out_of_fov"]["enable"] = vars.visuals.out_of_fov; {
		ragebot["out_of_fov"]["size"] = vars.visuals.out_of_fov_size;
		ragebot["out_of_fov"]["distance"] = vars.visuals.out_of_fov_distance;

		ragebot["out_of_fov"]["color"]["red"] = vars.visuals.out_of_fov_color.get_red();
		ragebot["out_of_fov"]["color"]["green"] = vars.visuals.out_of_fov_color.get_green();
		ragebot["out_of_fov"]["color"]["blue"] = vars.visuals.out_of_fov_color.get_blue();
		ragebot["out_of_fov"]["color"]["alpha"] = vars.visuals.out_of_fov_color.get_alpha();
	}

	ragebot["enemy"]["enable"] = vars.visuals.chams; {
		ragebot["enemy"]["visible_color"]["red"] = vars.visuals.chamscolor.get_red();
		ragebot["enemy"]["visible_color"]["green"] = vars.visuals.chamscolor.get_green();
		ragebot["enemy"]["visible_color"]["blue"] = vars.visuals.chamscolor.get_blue();
		ragebot["enemy"]["visible_color"]["alpha"] = vars.visuals.chamscolor.get_alpha();

		ragebot["enemy"]["through_walls"] = vars.visuals.chamsxqz;
		ragebot["enemy"]["through_walls_color"]["red"] = vars.visuals.chamscolor_xqz.get_red();
		ragebot["enemy"]["through_walls_color"]["green"] = vars.visuals.chamscolor_xqz.get_green();
		ragebot["enemy"]["through_walls_color"]["blue"] = vars.visuals.chamscolor_xqz.get_blue();
		ragebot["enemy"]["through_walls_color"]["alpha"] = vars.visuals.chamscolor_xqz.get_alpha();

		ragebot["enemy"]["overlay_color"]["red"] = vars.visuals.glow_col.get_red();
		ragebot["enemy"]["overlay_color"]["green"] = vars.visuals.glow_col.get_green();
		ragebot["enemy"]["overlay_color"]["blue"] = vars.visuals.glow_col.get_blue();
		ragebot["enemy"]["overlay_color"]["alpha"] = vars.visuals.glow_col.get_alpha();

		ragebot["enemy"]["xqz_overlay_color"]["red"] = vars.visuals.glow_col_xqz.get_red();
		ragebot["enemy"]["xqz_overlay_color"]["green"] = vars.visuals.glow_col_xqz.get_green();
		ragebot["enemy"]["xqz_overlay_color"]["blue"] = vars.visuals.glow_col_xqz.get_blue();
		ragebot["enemy"]["xqz_overlay_color"]["alpha"] = vars.visuals.glow_col_xqz.get_alpha();

		ragebot["enemy"]["material"] = vars.visuals.chamstype;
		ragebot["enemy"]["overlay"] = vars.visuals.overlay;
		ragebot["enemy"]["overlay_xqz"] = vars.visuals.overlay_xqz;
	}
	ragebot["local"]["localmodel"] = vars.visuals.localmodel;

	ragebot["local"]["modelct"] = vars.visuals.modelct;
	ragebot["local"]["modelt"] = vars.visuals.modelt;

	ragebot["local"]["enable"] = vars.visuals.localchams;
	{
		ragebot["local"]["material"] = vars.visuals.localchamstype;

		ragebot["local"]["overlay_color"]["red"] = vars.visuals.local_glow_color.get_red();
		ragebot["local"]["overlay_color"]["green"] = vars.visuals.local_glow_color.get_green();
		ragebot["local"]["overlay_color"]["blue"] = vars.visuals.local_glow_color.get_blue();
		ragebot["local"]["overlay_color"]["alpha"] = vars.visuals.local_glow_color.get_alpha();

		ragebot["local"]["visible_color"]["red"] = vars.visuals.localchams_color.get_red();
		ragebot["local"]["visible_color"]["green"] = vars.visuals.localchams_color.get_green();
		ragebot["local"]["visible_color"]["blue"] = vars.visuals.localchams_color.get_blue();
		ragebot["local"]["visible_color"]["alpha"] = vars.visuals.localchams_color.get_alpha();

		ragebot["local"]["overlay"] = vars.visuals.local_chams.overlay;
	}

	ragebot["glow_grenades"] = vars.visuals.glow_grenades;
	ragebot["glow_grenades_color"]["red"] = vars.visuals.glow_grenades_color.get_red();
	ragebot["glow_grenades_color"]["green"] = vars.visuals.glow_grenades_color.get_green();
	ragebot["glow_grenades_color"]["blue"] = vars.visuals.glow_grenades_color.get_blue();
	ragebot["glow_grenades_color"]["alpha"] = vars.visuals.glow_grenades_color.get_alpha();

	ragebot["glow_bomb"] = vars.visuals.glow_bomb;
	ragebot["glow_bomb_color"]["red"] = vars.visuals.glow_bomb_color.get_red();
	ragebot["glow_bomb_color"]["green"] = vars.visuals.glow_bomb_color.get_green();
	ragebot["glow_bomb_color"]["blue"] = vars.visuals.glow_bomb_color.get_blue();
	ragebot["glow_bomb_color"]["alpha"] = vars.visuals.glow_bomb_color.get_alpha();

	for (int i = 0; i < 4; i++) {
		string category = "misc_chams_" + std::to_string(i);
		ragebot[category]["enable"] = vars.visuals.misc_chams[i].enable;
		ragebot[category]["material"] = vars.visuals.misc_chams[i].material;
		ragebot[category]["overlay"] = vars.visuals.misc_chams[i].overlay;

		ragebot[category]["model_color"]["red"] = vars.visuals.misc_chams[i].clr.get_red();
		ragebot[category]["model_color"]["green"] = vars.visuals.misc_chams[i].clr.get_green();
		ragebot[category]["model_color"]["blue"] = vars.visuals.misc_chams[i].clr.get_blue();
		ragebot[category]["model_color"]["alpha"] = vars.visuals.misc_chams[i].clr.get_alpha();

		ragebot[category]["overlay_color"]["red"] = vars.visuals.misc_chams[i].glow_clr.get_red();
		ragebot[category]["overlay_color"]["green"] = vars.visuals.misc_chams[i].glow_clr.get_green();
		ragebot[category]["overlay_color"]["blue"] = vars.visuals.misc_chams[i].glow_clr.get_blue();
		ragebot[category]["overlay_color"]["alpha"] = vars.visuals.misc_chams[i].glow_clr.get_alpha();
	}


	ragebot["style"] = vars.visuals.glowtype;

	ragebot["enemy"]["enable"] = vars.visuals.glow;
	ragebot["enemy"]["color"]["red"] = vars.visuals.glow_color.get_red();
	ragebot["enemy"]["color"]["green"] = vars.visuals.glow_color.get_green();
	ragebot["enemy"]["color"]["blue"] = vars.visuals.glow_color.get_blue();
	ragebot["enemy"]["color"]["alpha"] = vars.visuals.glow_color.get_alpha();

	ragebot["local_glow"]["enable"] = vars.visuals.local_glow;
	ragebot["local_glow"]["color"]["red"] = vars.visuals.local_glow_clr.get_red();
	ragebot["local_glow"]["color"]["green"] = vars.visuals.local_glow_clr.get_green();
	ragebot["local_glow"]["color"]["blue"] = vars.visuals.local_glow_clr.get_blue();
	ragebot["local_glow"]["color"]["alpha"] = vars.visuals.local_glow_clr.get_alpha();



	ragebot["nadepred"]["enable"] = vars.visuals.nadepred;
	ragebot["watermark"] = vars.visuals.watermark;
	ragebot["water_type"] = vars.visuals.water_type;
	ragebot["speclist"] = vars.visuals.speclist;

	SaveColor(vars.visuals.nadepred_color, "color", &ragebot["nadepred"]);

	ragebot["indicators"] = vars.visuals.indicators;
	ragebot["hitmarkers"] = vars.visuals.hitmarkers;
	ragebot["aspect_ratio"] = vars.visuals.aspect_ratio;
	ragebot["bloom_effect"] = vars.visuals.bloom_effect;
	ragebot["ambient"] = vars.visuals.ambient;
	ragebot["thirdperson"]["enable"] = vars.misc.thirdperson;
	ragebot["thirdperson"]["distance"] = vars.visuals.thirdperson_dist;
	ragebot["asus_props"] = vars.misc.asus_props;
	SaveBind(vars.misc.thirdperson_bind, "bind", &ragebot["thirdperson"]);

	ragebot["eventlog"]["enable"] = vars.visuals.eventlog;

	ragebot["eventlog"]["color"]["red"] = vars.visuals.eventlog_color.get_red();
	ragebot["eventlog"]["color"]["green"] = vars.visuals.eventlog_color.get_green();
	ragebot["eventlog"]["color"]["blue"] = vars.visuals.eventlog_color.get_blue();
	ragebot["eventlog"]["color"]["alpha"] = vars.visuals.eventlog_color.get_alpha();

	ragebot["hitmarker"]["enable"] = vars.visuals.hitmarker;
	ragebot["hitmarker"]["enable_sound"] = vars.visuals.hitmarker_sound;

	ragebot["hitmarker"]["color"]["red"] = vars.visuals.hitmarker_color.get_red();
	ragebot["hitmarker"]["color"]["green"] = vars.visuals.hitmarker_color.get_green();
	ragebot["hitmarker"]["color"]["blue"] = vars.visuals.hitmarker_color.get_blue();
	ragebot["hitmarker"]["color"]["alpha"] = vars.visuals.hitmarker_color.get_alpha();


	ragebot["removals"] = vars.visuals.remove;
	ragebot["force_crosshair"] = vars.visuals.force_crosshair;
	ragebot["world_fov"] = vars.misc.worldfov;
	ragebot["viewmodel_fov"] = vars.misc.viewmodelfov;

	ragebot["nightmode"]["enable"] = vars.visuals.nightmode; {
		ragebot["nightmode"]["color"]["world"]["red"] = vars.visuals.nightmode_color.get_red();
		ragebot["nightmode"]["color"]["world"]["green"] = vars.visuals.nightmode_color.get_green();
		ragebot["nightmode"]["color"]["world"]["blue"] = vars.visuals.nightmode_color.get_blue();
		ragebot["nightmode"]["color"]["world"]["alpha"] = vars.visuals.nightmode_color.get_alpha();

		ragebot["nightmode"]["color"]["skybox"]["red"] = vars.visuals.nightmode_skybox_color.get_red();
		ragebot["nightmode"]["color"]["skybox"]["green"] = vars.visuals.nightmode_skybox_color.get_green();
		ragebot["nightmode"]["color"]["skybox"]["blue"] = vars.visuals.nightmode_skybox_color.get_blue();
		ragebot["nightmode"]["color"]["skybox"]["alpha"] = vars.visuals.nightmode_skybox_color.get_alpha();
	}

	ragebot["bullet_tracer"]["size"] = vars.visuals.impacts_size;
	ragebot["bullet_tracer"]["enable"] = vars.visuals.bullet_tracer; {
		ragebot["bullet_tracer"]["sprite"] = vars.visuals.bullet_tracer_type;
		ragebot["bullet_tracer"]["color"]["red"] = vars.visuals.bullet_tracer_color.get_red();
		ragebot["bullet_tracer"]["color"]["green"] = vars.visuals.bullet_tracer_color.get_green();
		ragebot["bullet_tracer"]["color"]["blue"] = vars.visuals.bullet_tracer_color.get_blue();
		ragebot["bullet_tracer"]["color"]["alpha"] = vars.visuals.bullet_tracer_color.get_alpha();

		ragebot["bullet_tracer"]["local"] = vars.visuals.bullet_tracer_local;
		ragebot["bullet_tracer"]["local_color"]["red"] = vars.visuals.bullet_tracer_local_color.get_red();
		ragebot["bullet_tracer"]["local_color"]["green"] = vars.visuals.bullet_tracer_local_color.get_green();
		ragebot["bullet_tracer"]["local_color"]["blue"] = vars.visuals.bullet_tracer_local_color.get_blue();
		ragebot["bullet_tracer"]["local_color"]["alpha"] = vars.visuals.bullet_tracer_local_color.get_alpha();
	}

	ragebot["bullet_impact"]["enable"] = vars.visuals.bullet_impact;
	{
		ragebot["bullet_impact"]["color"]["red"] = vars.visuals.bullet_impact_color.get_red();
		ragebot["bullet_impact"]["color"]["green"] = vars.visuals.bullet_impact_color.get_green();
		ragebot["bullet_impact"]["color"]["blue"] = vars.visuals.bullet_impact_color.get_blue();
		ragebot["bullet_impact"]["color"]["alpha"] = vars.visuals.bullet_impact_color.get_alpha();

		ragebot["bullet_impact_client"]["color"]["red"] = vars.visuals.client_impact_color.get_red();
		ragebot["bullet_impact_client"]["color"]["green"] = vars.visuals.client_impact_color.get_green();
		ragebot["bullet_impact_client"]["color"]["blue"] = vars.visuals.client_impact_color.get_blue();
		ragebot["bullet_impact_client"]["color"]["alpha"] = vars.visuals.client_impact_color.get_alpha();
	}

	ragebot["autojump"] = vars.misc.bunnyhop;
	ragebot["knifebot"] = vars.misc.knifebot;
	ragebot["clantagspammer"] = vars.visuals.clantagspammer;
	ragebot["clantag"] = vars.misc.clantag;

	ragebot["autobuy"]["enable"] = vars.misc.autobuy.enable;
	ragebot["autobuy"]["main"] = vars.misc.autobuy.main;
	ragebot["autobuy"]["pistol"] = vars.misc.autobuy.pistol;
	ragebot["autobuy"]["misc"] = vars.misc.autobuy.misc;
	ragebot["menu"]["color"]["red"] = vars.misc.accentcolor.get_red();
	ragebot["menu"]["color"]["green"] = vars.misc.accentcolor.get_green();
	ragebot["menu"]["color"]["blue"] = vars.misc.accentcolor.get_blue();
	ragebot["menu"]["color"]["alpha"] = vars.misc.accentcolor.get_alpha();


	static TCHAR path[256];
	std::string folder, file;

	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, path)))
	{
		folder = std::string(path) + ("\\necrozma\\");
		file = std::string(path) + ("\\necrozma\\" + cfg_name);
	}

	CreateDirectory(folder.c_str(), NULL);
	std::ofstream file_out(file);
	if (file_out.good())
		file_out << configuration;
	file_out.close();
}
ByteArray ReadAllBytes(char const* filename)
{
	ifstream ifs(filename, ios::binary | ios::ate);
	ifstream::pos_type pos = ifs.tellg();

	ByteArray result(pos);

	ifs.seekg(0, ios::beg);
	ifs.read((char*)&result[0], pos);

	return result;
}
void CConfig::Load(string cfg_name)
{
	json_t configuration;
	static TCHAR path[MAX_PATH];
	std::string folder, file;

	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, path)))
	{
		folder = std::string(path) + ("\\necrozma\\");
		file = std::string(path) + ("\\necrozma\\" + cfg_name);

	}
	CreateDirectory(folder.c_str(), NULL);
	std::ifstream file_out(file);

	if (file_out.good())
		file_out >> configuration/*in*/;

	if (!configuration.isMember("configuration"))
		return;

	auto& json = configuration["configuration"];
	json["configuration_name"] = cfg_name;

	auto& ragebot = json["main_config"];
	LoadBool(&vars.ragebot.enable, "enable", ragebot);
	LoadBool(&vars.ragebot.autoshoot, "auto_shoot", ragebot);
	LoadBool(&vars.ragebot.autoscope, "auto_scope", ragebot);
	LoadBool(&vars.ragebot.posadj, "backtrack", ragebot);
	LoadBool(&vars.ragebot.resolver, "resolver", ragebot);
	LoadBind(vars.ragebot.override_dmg, "override_dmg", ragebot);
	LoadBind(vars.ragebot.force_body, "force_body", ragebot);
	LoadBind(vars.ragebot.double_tap, "double_tap", ragebot);
	LoadBind(vars.ragebot.hide_shots, "hide_shots", ragebot);
	LoadInt(&vars.ragebot.dttype, "doubletap_type", ragebot);


	auto& weapon_cfg = ragebot["weapons"];
	for (int i = 0; i < 7; i++) {
		string category = "weapon_" + std::to_string(i);
		LoadBool(&vars.ragebot.weapon[i].enable, "enable", weapon_cfg[category]);
		LoadInt(&vars.ragebot.weapon[i].mindamage, "mindamage", weapon_cfg[category]);
		LoadInt(&vars.ragebot.weapon[i].mindamage_override, "mindamage_override", weapon_cfg[category]);
		LoadInt(&vars.ragebot.weapon[i].hitchance, "hitchance", weapon_cfg[category]);
		LoadInt(&vars.ragebot.weapon[i].hitchancetype, "hitchance_type", weapon_cfg[category]);
		LoadBool(&vars.ragebot.weapon[i].quickstop, "quickstop", weapon_cfg[category]);
		LoadInt(&vars.ragebot.weapon[i].pointscale_head, "pointscale_head", weapon_cfg[category]);
		LoadInt(&vars.ragebot.weapon[i].pointscale_body, "pointscale_body", weapon_cfg[category]);
		LoadUInt(&vars.ragebot.weapon[i].hitscan, "hitscan", weapon_cfg[category]);
		LoadUInt(&vars.ragebot.weapon[i].hitscan_baim, "hitscan_baim", weapon_cfg[category]);
		LoadUInt(&vars.ragebot.weapon[i].baim, "baim", weapon_cfg[category]);
		LoadInt(&vars.ragebot.weapon[i].baim_under_hp, "baim_under_hp", weapon_cfg[category]);
		LoadBool(&vars.ragebot.weapon[i].adaptive_baim, "adaptive_baim", weapon_cfg[category]);
	}



	LoadBool(&vars.antiaim.enable, "enable", ragebot);
	LoadInt(&vars.antiaim.yaw, "yaw", ragebot);
	LoadInt(&vars.antiaim.pitch, "pitch", ragebot);
	LoadInt(&vars.antiaim.desync_type, "desync_type", ragebot);
	LoadBind(vars.antiaim.fakeduck, "fakeduck", ragebot);
	LoadBind(vars.antiaim.slowwalk, "slowwalk", ragebot);
	LoadInt(&vars.antiaim.blin, "blin", ragebot);
	LoadInt(&vars.antiaim.inverterblin, "inverterblin", ragebot);
	LoadBind(vars.antiaim.inverter, "inverter", ragebot);
	LoadBool(&vars.antiaim.attargets, "attargets", ragebot);

	LoadBool(&vars.antiaim.aa_override.enable, "enable", ragebot);
	LoadBind(vars.antiaim.aa_override.left, "left", ragebot);
	LoadBind(vars.antiaim.aa_override.right, "right", ragebot);
	LoadBind(vars.antiaim.aa_override.back, "back", ragebot);

	LoadInt(&vars.antiaim.fakelag, "type", ragebot);
	LoadInt(&vars.antiaim.fakelagfactor, "factor", ragebot);
	LoadInt(&vars.antiaim.fakelagvariance, "variance", ragebot);

	LoadBool(&vars.antiaim.fakelag_onshot, "fakelag_onshot", ragebot);

	LoadBool(&vars.visuals.enable, "enable", ragebot);
	LoadBool(&vars.visuals.dormant, "dormant", ragebot);
	LoadBool(&vars.visuals.skeleton, "box", ragebot);
	LoadColor(&vars.visuals.skeleton_color, "box_color", ragebot);
	LoadBool(&vars.visuals.box, "box", ragebot);
	LoadColor(&vars.visuals.box_color, "box_color", ragebot);
	LoadBool(&vars.visuals.healthbar, "enable", ragebot["healthbar"]);
	LoadBool(&vars.visuals.override_hp, "override_hp", ragebot["healthbar"]);
	LoadColor(&vars.visuals.hp_color, "hp_color", ragebot["healthbar"]);

	LoadBool(&vars.visuals.name, "name", ragebot);
	LoadColor(&vars.visuals.name_color, "name_color", ragebot);

	LoadBool(&vars.visuals.weapon, "weapon", ragebot);
	LoadBool(&vars.visuals.weapon_icon, "weapon_icon", ragebot);
	LoadColor(&vars.visuals.weapon_color, "weapon_color", ragebot);

	LoadBool(&vars.visuals.ammo, "ammo", ragebot);
	LoadColor(&vars.visuals.ammo_color, "ammo_color", ragebot);

	LoadUInt(&vars.visuals.flags, "flags", ragebot);
	LoadColor(&vars.visuals.flags_color, "flags_color", ragebot);
	LoadBool(&vars.visuals.shot_multipoint, "show_multipoint", ragebot);
	LoadBool(&vars.visuals.out_of_fov, "enable", ragebot["out_of_fov"]);
	LoadInt(&vars.visuals.out_of_fov_size, "size", ragebot["out_of_fov"]);
	LoadInt(&vars.visuals.out_of_fov_distance, "distance", ragebot["out_of_fov"]);
	LoadColor(&vars.visuals.out_of_fov_color, "color", ragebot["out_of_fov"]);


	LoadBool(&vars.visuals.chams, "enable", ragebot["enemy"]);
	LoadBool(&vars.visuals.chamsxqz, "through_walls", ragebot["enemy"]);
	LoadColor(&vars.visuals.chamscolor, "visible_color", ragebot["enemy"]);
	LoadColor(&vars.visuals.chamscolor_xqz, "through_walls_color", ragebot["enemy"]);
	LoadColor(&vars.visuals.glow_col, "overlay_color", ragebot["enemy"]);
	LoadColor(&vars.visuals.glow_col_xqz, "xqz_overlay_color", ragebot["enemy"]);
	LoadInt(&vars.visuals.overlay, "overlay", ragebot["enemy"]);
	LoadInt(&vars.visuals.overlay_xqz, "overlay_xqz", ragebot["enemy"]);
	LoadInt(&vars.visuals.chamstype, "material", ragebot["enemy"]);

	LoadBool(&vars.visuals.localmodel, "localmodel", ragebot["local"]);
	LoadInt(&vars.visuals.modelct, "modelct", ragebot["local"]);
	LoadInt(&vars.visuals.modelt, "modelt", ragebot["local"]);

	LoadBool(&vars.visuals.localchams, "enable", ragebot["local"]);
	LoadBool(&vars.visuals.blend_on_scope, "blend_on_scope", ragebot["local"]);
	LoadInt(&vars.visuals.blend_value, "blend_value", ragebot["local"]);
	LoadBool(&vars.visuals.localchams, "enable", ragebot["local"]);
	LoadColor(&vars.visuals.localchams_color, "visible_color", ragebot["local"]);
	LoadColor(&vars.visuals.local_glow_color, "overlay_color", ragebot["local"]);
	LoadColor(&vars.visuals.glow_grenades_color, "glow_grenades_color", ragebot["grenades"]);
	LoadColor(&vars.visuals.glow_bomb_color, "glow_bomb_color", ragebot["bomb"]);
	LoadInt(&vars.visuals.localchamstype, "material", ragebot["local"]);
	LoadInt(&vars.visuals.local_chams.overlay, "overlay", ragebot["local"]);


	for (int i = 0; i < 4; i++) {
		string category = "misc_chams_" + std::to_string(i);
		LoadBool(&vars.visuals.misc_chams[i].enable, "enable", ragebot[category]);
		LoadColor(&vars.visuals.misc_chams[i].clr, "model_color", ragebot[category]);
		LoadColor(&vars.visuals.misc_chams[i].glow_clr, "overlay_color", ragebot[category]);
		LoadInt(&vars.visuals.misc_chams[i].material, "material", ragebot[category]);
		LoadInt(&vars.visuals.misc_chams[i].overlay, "overlay", ragebot[category]);
	}


	LoadInt(&vars.visuals.glowtype, "style", ragebot);
	LoadBool(&vars.visuals.glow, "enable", ragebot["enemy"]);

	LoadBool(&vars.visuals.glow_bomb, "glow_bomb", ragebot);
	LoadBool(&vars.visuals.glow_grenades, "glow_grenades", ragebot);

	LoadColor(&vars.visuals.glow_bomb_color, "glow_bomb_color", ragebot);
	LoadColor(&vars.visuals.glow_grenades_color , "glow_grenades_color", ragebot);

	LoadColor(&vars.visuals.glow_color, "color", ragebot["enemy"]);

	LoadBool(&vars.visuals.local_glow, "enable", ragebot["local_glow"]);
	LoadColor(&vars.visuals.local_glow_clr, "color", ragebot["local_glow"]);

	LoadBool(&vars.visuals.speclist, "speclist", ragebot);
	LoadBool(&vars.visuals.watermark, "watermark", ragebot);
	LoadInt(&vars.visuals.water_type, "water_type", ragebot);
	LoadBool(&vars.misc.thirdperson, "enable", ragebot["thirdperson"]);
	LoadInt(&vars.visuals.thirdperson_dist, "distance", ragebot["thirdperson"]);
	LoadFloat(&vars.misc.asus_props, "asus_props", ragebot);
	LoadBind(vars.misc.thirdperson_bind, "bind", ragebot["thirdperson"]);
	LoadUInt(&vars.visuals.indicators, "indicators", ragebot);
	LoadUInt(&vars.visuals.hitmarkers, "hitmarkers", ragebot);


	LoadInt(&vars.visuals.aspect_ratio, "aspect_ratio", ragebot);
	LoadInt(&vars.visuals.bloom_effect, "bloom_effect", ragebot);
	LoadInt(&vars.visuals.ambient, "ambient", ragebot);
	LoadBool(&vars.visuals.eventlog, "enable", ragebot["eventlog"]);
	LoadColor(&vars.visuals.eventlog_color, "color", ragebot["eventlog"]);

	LoadInt(&vars.visuals.hitmarker, "enable", ragebot["hitmarker"]);
	LoadBool(&vars.visuals.hitmarker_sound, "enable_sound", ragebot["hitmarker"]);
	LoadColor(&vars.visuals.hitmarker_color, "color", ragebot["hitmarker"]);

	LoadBool(&vars.visuals.nadepred, "enable", ragebot["nadepred"]);
	LoadColor(&vars.visuals.nadepred_color, "color", ragebot["nadepred"]);

	LoadUInt(&vars.visuals.remove, "removals", ragebot);
	LoadBool(&vars.visuals.force_crosshair, "force_crosshair", ragebot);
	LoadInt(&vars.misc.worldfov, "world_fov", ragebot);
	LoadInt(&vars.misc.viewmodelfov, "viewmodel_fov", ragebot);

	LoadBool(&vars.visuals.nightmode, "enable", ragebot["nightmode"]);
	LoadColor(&vars.visuals.nightmode_color, "world", ragebot["nightmode"]["color"]);
	LoadColor(&vars.visuals.nightmode_skybox_color, "skybox", ragebot["nightmode"]["color"]);

	LoadBool(&vars.visuals.bullet_tracer, "enable", ragebot["bullet_tracer"]);
	LoadFloat(&vars.visuals.impacts_size, "size", ragebot["bullet_tracer"]);
	LoadInt(&vars.visuals.bullet_tracer_type, "sprite", ragebot["bullet_tracer"]);
	LoadColor(&vars.visuals.bullet_tracer_color, "color", ragebot["bullet_tracer"]);
	LoadBool(&vars.visuals.bullet_tracer_local, "local", ragebot["bullet_tracer"]);
	LoadColor(&vars.visuals.bullet_tracer_local_color, "local_color", ragebot["bullet_tracer"]);

	LoadBool(&vars.visuals.bullet_impact, "enable", ragebot["bullet_impact"]);
	LoadColor(&vars.visuals.bullet_impact_color, "color", ragebot["bullet_impact"]);
	LoadColor(&vars.visuals.client_impact_color, "color", ragebot["bullet_impact_client"]);

	LoadBool(&vars.misc.bunnyhop, "autojump", ragebot);
	LoadBool(&vars.misc.knifebot, "knifebot", ragebot);
	LoadBool(&vars.visuals.clantagspammer, "clantagspammer", ragebot);
	LoadInt(&vars.misc.clantag, "clantag", ragebot);
	LoadBool(&vars.misc.hold_firinganims, "hold_firinganims", ragebot);
	LoadBool(&vars.misc.autobuy.enable, "enable", ragebot["autobuy"]);
	LoadInt(&vars.misc.autobuy.pistol, "pistol", ragebot["autobuy"]);
	LoadInt(&vars.misc.autobuy.main, "main", ragebot["autobuy"]);
	LoadUInt(&vars.misc.autobuy.misc, "misc", ragebot["autobuy"]);

	LoadColor(&vars.misc.accentcolor, "color", ragebot["menu"]);

}

CConfig Config;
CGlobalVariables vars;