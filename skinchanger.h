#pragma once
#include "Hooks.h"
class c_skins
{
public:

	struct weapon_info {
		constexpr weapon_info(const char* model, const char* icon = nullptr, int animindex = -1) : model(model), icon(icon), animindex(animindex) {}
		const char* model;
		const char* icon;
		int animindex;
	};

	struct weapon_name {
		constexpr weapon_name(const int definition_index, const char* name) : definition_index(definition_index), name(name) {}
		int definition_index = 0;
		const char* name = nullptr;
	};

	const static weapon_info* GetWeaponInfo(int defindex) {
		const static std::map<int, weapon_info> Info = {
			{ WEAPON_KNIFE,{ "models/weapons/v_knife_default_ct.mdl", "knife_default_ct", 2 } },
			{ WEAPON_KNIFE_T,{ "models/weapons/v_knife_default_t.mdl", "knife_t", 12 } },
			{ WEAPON_KNIFE_BAYONET,{ "models/weapons/v_knife_bayonet.mdl", "bayonet", 0 } },
			{ WEAPON_KNIFE_FLIP,{ "models/weapons/v_knife_flip.mdl", "knife_flip", 4 } },
			{ WEAPON_KNIFE_GUT,{ "models/weapons/v_knife_gut.mdl", "knife_gut", 5 } },
			{ WEAPON_KNIFE_KARAMBIT,{ "models/weapons/v_knife_karam.mdl", "knife_karambit", 7 } },
			{ WEAPON_KNIFE_M9_BAYONET,{ "models/weapons/v_knife_m9_bay.mdl", "knife_m9_bayonet", 8 } },
			{ WEAPON_KNIFE_TACTICAL,{ "models/weapons/v_knife_tactical.mdl", "knife_tactical" } },
			{ WEAPON_KNIFE_FALCHION,{ "models/weapons/v_knife_falchion_advanced.mdl", "knife_falchion", 3 } },
			{ WEAPON_KNIFE_SURVIVAL_BOWIE,{ "models/weapons/v_knife_survival_bowie.mdl", "knife_survival_bowie", 11 } },
			{ WEAPON_KNIFE_BUTTERFLY,{ "models/weapons/v_knife_butterfly.mdl", "knife_butterfly", 1 } },
			{ WEAPON_KNIFE_PUSH,{ "models/weapons/v_knife_push.mdl", "knife_push", 9 } },
			{ WEAPON_KNIFE_URSUS,{ "models/weapons/v_knife_ursus.mdl", "knife_ursus", 13 } },
			{ WEAPON_KNIFE_GYPSY_JACKKNIFE,{ "models/weapons/v_knife_gypsy_jackknife.mdl", "knife_gypsy_jackknife", 6 } },
			{ WEAPON_KNIFE_STILETTO,{ "models/weapons/v_knife_stiletto.mdl", "knife_stiletto", 10 } },
			{ WEAPON_KNIFE_WIDOWMAKER,{ "models/weapons/v_knife_widowmaker.mdl", "knife_widowmaker", 14 } },
			{ GLOVE_STUDDED_BLOODHOUND,{ "models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound.mdl" } },
			{ GLOVE_T_SIDE,{ "models/weapons/v_models/arms/glove_fingerless/v_glove_fingerless.mdl" } },
			{ GLOVE_CT_SIDE,{ "models/weapons/v_models/arms/glove_hardknuckle/v_glove_hardknuckle.mdl" } },
			{ GLOVE_SPORTY,{ "models/weapons/v_models/arms/glove_sporty/v_glove_sporty.mdl" } },
			{ GLOVE_SLICK,{ "models/weapons/v_models/arms/glove_slick/v_glove_slick.mdl" } },
			{ GLOVE_LEATHER_WRAP,{ "models/weapons/v_models/arms/glove_handwrap_leathery/v_glove_handwrap_leathery.mdl" } },
			{ GLOVE_MOTORCYCLE,{ "models/weapons/v_models/arms/glove_motorcycle/v_glove_motorcycle.mdl" } },
			{ GLOVE_SPECIALIST,{ "models/weapons/v_models/arms/glove_specialist/v_glove_specialist.mdl" } },
			{ GLOVE_HYDRA,{ "models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound_hydra.mdl" } },
			{  521, { "models/weapons/v_knife_outdoor.mdl", "knife_outdoor", 14}  },
			{  518 ,  {"models/weapons/v_knife_canis.mdl", "knife_canis",14}},
			{  517 ,  {"models/weapons/v_knife_cord.mdl", "knife_cord", 14}},
			{  525 ,  {"models/weapons/v_knife_skeleton.mdl", "knife_skeleton" ,14}},
			{  503 ,  {"models/weapons/v_knife_css.mdl", "knife_css", 14}}


		};
		const auto entry = Info.find(defindex);
		return entry == end(Info) ? nullptr : &entry->second;
	}

	const std::vector<weapon_name> WeaponNames = {
		{ 1, "Desert Eagle" },
		{ 2, "Dual Berettas" },
		{ 3, "Five-SeveN" },
		{ 4, "Glock-18" },
		{ 30, "Tec-9" },
		{ 32, "P2000" },
		{ 36, "P250" },
		{ 61, "USP-S" },
		{ 63, "CZ75 Auto" },
		{ 64, "R8 Revolver" },
		{ 7, "AK-47" },
		{ 8, "AUG" },
		{ 10, "FAMAS" },
		{ 13, "Galil AR" },
		{ 16, "M4A4" },
		{ 39, "SG 553" },
		{ 60, "M4A1-S" },
		{ 17, "MAC-10" },
		{ 19, "P90" },
		{ 23, "MP5-SD" },
		{ 24, "UMP-45" },
		{ 26, "PP-Bizon" },
		{ 33, "MP7" },
		{ 34, "MP9" },
		{ 14, "M249" },
		{ 25, "XM1014" },
		{ 27, "MAG-7" },
		{ 28, "Negev" },
		{ 29, "Sawed-Off" },
		{ 35, "Nova" },
		{ 9, "AWP" },
		{ 11, "G3SG1" },
		{ 38, "SCAR-20" },
		{ 40, "SSG 08" },

		{ WEAPON_KNIFE_BAYONET, "Bayonet" },
		{ WEAPON_KNIFE_FLIP, "Flip Knife" },
		{ WEAPON_KNIFE_GUT, "Gut Knife" },
		{ WEAPON_KNIFE_KARAMBIT, "Karambit" },
		{ WEAPON_KNIFE_M9_BAYONET, "M9 Bayonet" },
		{ WEAPON_KNIFE_TACTICAL, "Huntsman Knife" },
		{ WEAPON_KNIFE_FALCHION, "Falchion Knife" },
		{ WEAPON_KNIFE_SURVIVAL_BOWIE, "Bowie Knife" },
		{ WEAPON_KNIFE_BUTTERFLY, "Butterfly Knife" },
		{ WEAPON_KNIFE_PUSH, "Shadow Daggers" },
		{ WEAPON_KNIFE_URSUS, "Ursus Knife" },
		{ WEAPON_KNIFE_GYPSY_JACKKNIFE, "Navaja Knife" },
		{ WEAPON_KNIFE_STILETTO, "Stiletto Knife" },
		{ WEAPON_KNIFE_WIDOWMAKER, "Talon Knife" },

		{ GLOVE_STUDDED_BLOODHOUND, "Bloodhound" },
		{ GLOVE_SPORTY, "Sporty" },
		{ GLOVE_SLICK, "Slick" },
		{ GLOVE_LEATHER_WRAP, "Handwrap" },
		{ GLOVE_MOTORCYCLE, "Motorcycle" },
		{ GLOVE_SPECIALIST, "Specialist" },
		{ GLOVE_HYDRA, "Hydra" }
	};


	static void skinchager();
	static void glovechanger();
};