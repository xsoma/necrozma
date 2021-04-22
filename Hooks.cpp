#include "Hooks.h"
#include "netvar_manager.h"

// Include hooks
#include "PaintTraverse.h"
#include "Createmove.h"
#include "FrameStageNotify.h"
#include "EndScene.h"
#include "WndProc.h"
#include "SceneEnd.h"
#include "DoPostScreenEffects.h"
#include "FireEventClientSide.h"
#include "BeginFrame.h"
#include "OverRideView.h"
#include "DrawModelExecuted.h"
#include "SetupAnimation.h"
#include "EngineHook.h"
#include "TraceRay.h"
#include "detours.h"
#include "knife_animation.h"
ClMoveFn H::ClMove;
GetForeignFallbackFontNameFn H::GetForeignFallbackFontName;
send_net_msg_fn H::SendNetMsg;
send_datagram_fn H::SendDatagram;
SetupBonesFn H::SetupBones;
temp_entities_fn H::TempEntities;
ClientEffectCallback H::ImpactCallback;
CClientEffectRegistration* H::Effects;
DoExtraBoneProcessingFn H::DoExtraBoneProcessing;
StandardBlendingRulesFn H::StandardBlendingRules;
ShouldSkipAnimFrameFn H::ShouldSkipAnimFrame;
UpdateClientSideAnimationFn H::UpdateClientSideAnimation;
Present_t H::SteamPresent;
Reset_t H::SteamReset;
BuildTransformationsFn H::BuildTransformations;
namespace Index
{
	// client mode interface
	const int CreateMove = 24;
	const int OverrideView = 18;
	const int DoPostScreenEffects = 44;

	// panel interface
	const int PaintTraverse = 41;

	// client interface
	const int FrameStageNotify = 37;

	// render view interface
	const int SceneEnd = 9;

	// studio render interface
	const int DrawModel = 29;
	const int BeginFrame = 9;

	// events
	const int FireEventClientSide = 9;

	// model render interface
	const int DrawModelExecute = 21;

	// predict interface
	const int InPrediction = 14;

	// engine->GetBSPTreeQuery()
	const int ListLeavesInBox = 6;

	// cvar "sv_cheats"
	const int GetBool = 13;

	// cvar "cl_camera_height_restriction_debug"
	const int InCamera = 13;

	// direct interface
	const int Present = 17;
	const int Reset = 16;

	// engine interface
	const int IsBoxVisible = 32;
	const int IsHLTV = 93;

	// renderable
	const int IsRenderableInPVS = 8;
}

cStructManager* csgo = new cStructManager();
features_t* features = new features_t();
c_interfaces interfaces;
std::vector<IGameEventListener2*> g_pGameEventManager;

void cStructManager::updatelocalplayer()
{
	local = interfaces.ent_list->GetClientEntity(interfaces.engine->GetLocalPlayer());
}

void Msg(string str, color_t clr)
{
	if (!vars.visuals.eventlog)
		return;

	features->Eventlog->messages.push_back(CMessage(str, csgo->get_absolute_time(), clr));
	interfaces.cvars->Consolecolor_tPrintf(clr, hs::prefix::s().c_str());
	interfaces.cvars->Consolecolor_tPrintf(color_t(255, 255, 255), "%s\n", str.c_str());
}

template< typename T >
T* Interface(const char* strModule, const char* strInterface)
{
	typedef T* (*CreateInterfaceFn)(const char* szName, int iReturn);
	CreateInterfaceFn CreateInterface = (CreateInterfaceFn)GetProcAddress(GetModuleHandleA(strModule), (hs::create_interface::s().c_str()));

	if (!CreateInterface)
	{
	}

	return CreateInterface(strInterface, 0);
}

void CSignatures::Initialize() {
	Reset = *reinterpret_cast<std::uintptr_t*>(csgo->Utils.FindPatternIDA(GetModuleHandleA("gameoverlayrenderer.dll"), "FF 15 ? ? ? ? 8B F8 85 FF 78 18") + 2);
	Present = *reinterpret_cast<std::uintptr_t*>(csgo->Utils.FindPatternIDA(GetModuleHandleA("gameoverlayrenderer.dll"), "FF 15 ? ? ? ? 8B F8 85 DB") + 2);
}
RecvPropHook* sequence_hooks = new RecvPropHook();
RecvPropHook* viewmodel_hooks = new RecvPropHook();
void I::Setup()
{

	csgo->Utils.FindPatternIDA = [](HMODULE hModule, const char* szSignature) -> uint8_t* {
		static auto pattern_to_byte = [](const char* pattern) {
			auto bytes = std::vector<int>{};
			auto start = const_cast<char*>(pattern);
			auto end = const_cast<char*>(pattern) + strlen(pattern);

			for (auto current = start; current < end; ++current) {
				if (*current == '?') {
					++current;
					if (*current == '?')
						++current;
					bytes.push_back(-1);
				}
				else {
					bytes.push_back(strtoul(current, &current, 16));
				}
			}
			return bytes;
		};

		//auto Module = GetModuleHandleA(szModule);

		auto dosHeader = (PIMAGE_DOS_HEADER)hModule;
		auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)hModule + dosHeader->e_lfanew);

		auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
		auto patternBytes = pattern_to_byte(szSignature);
		auto scanBytes = reinterpret_cast<std::uint8_t*>(hModule);

		auto s = patternBytes.size();
		auto d = patternBytes.data();

		for (auto i = 0ul; i < sizeOfImage - s; ++i) {
			bool found = true;
			for (auto j = 0ul; j < s; ++j) {
				if (scanBytes[i + j] != d[j] && d[j] != -1) {
					found = false;
					break;
				}
			}
			if (found) {
				return &scanBytes[i];
			}
		}
		return nullptr;
	};

	g_Modules[fnv::hash(hs::serverbrowser_dll::s().c_str())] = hs::serverbrowser_dll::s().c_str();
	g_Modules[fnv::hash(hs::client_dll::s().c_str())] = hs::client_dll::s().c_str();
	g_Modules[fnv::hash(hs::engine_dll::s().c_str())] = hs::engine_dll::s().c_str();
	g_Modules[fnv::hash(hs::vstdlib_dll::s().c_str())] = hs::vstdlib_dll::s().c_str();
	g_Modules[fnv::hash(hs::input_system_dll::s().c_str())] = hs::input_system_dll::s().c_str();
	g_Modules[fnv::hash(hs::server_dll::s().c_str())] = hs::server_dll::s().c_str();
	g_Modules[fnv::hash(hs::vgui_mat_surface_dll::s().c_str())] = hs::vgui_mat_surface_dll::s().c_str();
	g_Modules[fnv::hash(hs::vgui2_dll::s().c_str())] = hs::vgui2_dll::s().c_str();
	g_Modules[fnv::hash(hs::mat_sys_dll::s().c_str())] = hs::mat_sys_dll::s().c_str();
	g_Modules[fnv::hash(hs::studio_render_dll::s().c_str())] = hs::studio_render_dll::s().c_str();
	g_Modules[fnv::hash(hs::physics_dll::s().c_str())] = hs::physics_dll::s().c_str();
	g_Modules[fnv::hash(hs::data_cache_dll::s().c_str())] = hs::data_cache_dll::s().c_str();
	g_Modules[fnv::hash(hs::tier0_dll::s().c_str())] = hs::tier0_dll::s().c_str();
	g_Modules[fnv::hash(hs::gameoverlayrenderer_dll::s().c_str())] = hs::gameoverlayrenderer_dll::s().c_str();

	g_Patterns[fnv::hash(hs::client_state::s().c_str())] = hs::client_state_p2::s().c_str();
	g_Patterns[fnv::hash(hs::move_helper::s().c_str())] = hs::move_helper_p::s().c_str();

	g_Patterns[fnv::hash(hs::input::s().c_str())] = hs::input_p::s().c_str();
	g_Patterns[fnv::hash(hs::glow_manager::s().c_str())] = hs::glow_manager_p::s().c_str();
	g_Patterns[fnv::hash(hs::beams::s().c_str())] = hs::beams_p::s().c_str();
	g_Patterns[fnv::hash(hs::update_clientside_anim::s().c_str())] = hs::update_clientside_anim_p::s().c_str();
	g_Patterns[fnv::hash(hs::debp::s().c_str())] = hs::debp_p::s().c_str();
	g_Patterns[fnv::hash(hs::standard_blending_rules::s().c_str())] = hs::standard_blending_rules_p::s().c_str();
	g_Patterns[fnv::hash(hs::should_skip_anim_frame::s().c_str())] = hs::should_skip_anim_frame_p::s().c_str();
	g_Patterns[fnv::hash(hs::get_foreign_fall_back_name::s().c_str())] = hs::get_foreign_fall_back_name_p::s().c_str();
	g_Patterns[fnv::hash(hs::setup_bones::s().c_str())] = hs::setup_bones_p::s().c_str();
	g_Patterns[fnv::hash(hs::build_transformations::s().c_str())] = hs::build_transformations_p::s().c_str();
	g_Patterns[fnv::hash(hs::effects::s().c_str())] = hs::effects_p::s().c_str();

	static auto test = g_Patterns[fnv::hash(hs::effects::s().c_str())];
	test = test;

	interfaces.client = Interface< IBaseClientDll >((
		g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()),
		hs::client::s().c_str());

	interfaces.ent_list = Interface< CEntityList >((
		g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()),
		hs::ent_list::s().c_str());

	interfaces.leaf_system = Interface< IClientLeafSystem >((
		g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()),
		hs::leaf_sys::s().c_str());

	interfaces.cvars = Interface< ICVar >(
		g_Modules[fnv::hash(hs::vstdlib_dll::s().c_str())].c_str(),
		hs::cvar::s().c_str());

	interfaces.engine = Interface< IEngineClient >(((
		g_Modules[fnv::hash(hs::engine_dll::s().c_str())].c_str())),
		hs::engine_client::s().c_str());

	interfaces.trace = Interface< IEngineTrace >(((
		g_Modules[fnv::hash(hs::engine_dll::s().c_str())].c_str())),
		hs::engine_trace::s().c_str());

	interfaces.inputsystem = Interface< IInputSystem >(
		g_Modules[fnv::hash(hs::input_system_dll::s().c_str())].c_str(),
		hs::input_sys::s().c_str());

	interfaces.surfaces = Interface< ISurface >(
		g_Modules[fnv::hash(hs::vgui_mat_surface_dll::s().c_str())].c_str(),
		hs::vgui_surface::s().c_str());

	interfaces.server = GetInterface<IServerGameDLL>(
		g_Modules[fnv::hash(hs::server_dll::s().c_str())].c_str(),
		hs::server_game_dll::s().c_str());

	interfaces.v_panel = Interface< IVPanel >(
		g_Modules[fnv::hash(hs::vgui2_dll::s().c_str())].c_str(),
		hs::vgui_panel::s().c_str());

	interfaces.models.model_render = Interface< IVModelRender >(((
		g_Modules[fnv::hash(hs::engine_dll::s().c_str())].c_str())),
		hs::engine_model::s().c_str());

	interfaces.material_system = Interface< IMaterialSystem >(
		g_Modules[fnv::hash(hs::mat_sys_dll::s().c_str())].c_str(),
		hs::mat_sys::s().c_str());

	interfaces.models.model_info = Interface< IVModelInfo >(((
		g_Modules[fnv::hash(hs::engine_dll::s().c_str())].c_str())),
		hs::model_info::s().c_str());

	interfaces.render_view = Interface< IVRenderView >(((
		g_Modules[fnv::hash(hs::engine_dll::s().c_str())].c_str())),
		hs::render_view::s().c_str());

	interfaces.debug_overlay = Interface< IVDebugOverlay >(((
		g_Modules[fnv::hash(hs::engine_dll::s().c_str())].c_str())),
		hs::debug_overlay::s().c_str());

	interfaces.event_manager = Interface< IGameEventManager2 >(((
		g_Modules[fnv::hash(hs::engine_dll::s().c_str())].c_str())),
		hs::events_manager::s().c_str());

	interfaces.studio_render = Interface< IStudioRender >(
		g_Modules[fnv::hash(hs::studio_render_dll::s().c_str())].c_str(),
		hs::studio_render::s().c_str());

	interfaces.phys_props = Interface< IPhysicsSurfaceProps >(
		g_Modules[fnv::hash(hs::physics_dll::s().c_str())].c_str(),
		hs::phys_props::s().c_str());

	interfaces.game_movement = Interface< CGameMovement >((
		g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()),
		hs::game_movement::s().c_str());

	interfaces.model_cache = Interface< void >(
		g_Modules[fnv::hash(hs::data_cache_dll::s().c_str())].c_str(),
		hs::mdl_cache::s().c_str());

	interfaces.engine_vgui = Interface<IEngineVGui>((
		g_Modules[fnv::hash(hs::engine_dll::s().c_str())].c_str()),
		hs::engine_vgui::s().c_str());

	interfaces.engine_sound = Interface< IEngineSound >((
		g_Modules[fnv::hash(hs::engine_dll::s().c_str())].c_str()),
		hs::engine_sound::s().c_str());

	interfaces.move_helper = **reinterpret_cast<IMoveHelper***>(csgo->Utils.FindPatternIDA(GetModuleHandleA(
		g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()),
		g_Patterns[fnv::hash(hs::move_helper::s().c_str())].c_str()) + 2);

	interfaces.prediction = Interface< CPrediction >((
		g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()),
		hs::client_prediction::s().c_str());

	interfaces.input = reinterpret_cast<CInput*>(*reinterpret_cast<DWORD*>(csgo->Utils.FindPatternIDA(GetModuleHandleA(
		g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()),
		g_Patterns[fnv::hash(hs::input::s().c_str())].c_str()) + 1));

	interfaces.glow_manager = *reinterpret_cast<CGlowObjectManager**>(csgo->Utils.FindPatternIDA(GetModuleHandleA(
		g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()),
		g_Patterns[fnv::hash(hs::glow_manager::s().c_str())].c_str()) + 3);

	interfaces.beams = *reinterpret_cast<IViewRenderBeams**>(csgo->Utils.FindPatternIDA(GetModuleHandleA(
		g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()),
		g_Patterns[fnv::hash(hs::beams::s().c_str())].c_str()) + 1);

	interfaces.memalloc = *reinterpret_cast<IMemAlloc**>(GetProcAddress(GetModuleHandleA(
		g_Modules[fnv::hash(hs::tier0_dll::s().c_str())].c_str()),
		hs::mem_alloc::s().c_str()));

	interfaces.global_vars = **reinterpret_cast<global_vars_t***>((*(DWORD**)interfaces.client)[11] + 10);
	interfaces.client_mode = **reinterpret_cast<IClientMode***>((*reinterpret_cast<uintptr_t**>(interfaces.client))[10] + 0x5u);
	CSignatures::Get().Initialize();
}

float F::KillDelayTime = NULL;
LineGoesThroughSmokeFn F::LineToSmoke = NULL;

bool F::Shooting()
{
	auto btime = []()
	{
		if (!csgo->weapon)
			return false;
		float flNextPrimaryAttack = csgo->weapon->NextPrimaryAttack();

		return flNextPrimaryAttack <= (float)csgo->local->GetTickBase() * interfaces.global_vars->interval_per_tick;
	};

	if (!csgo->weapon)
		return false;
	if (csgo->weapon->IsBomb())
		return false;
	else if (csgo->weapon->IsNade())
	{
		CBaseCSGrenade* csGrenade = (CBaseCSGrenade*)csgo->weapon;
		if (csGrenade->GetThrowTime() > 0.f)
		{
			return true;
		}
	}
	else if (csgo->cmd->buttons & IN_ATTACK && btime())
	{
		return true;
	}
	return false;
}

namespace VMT
{
	std::unique_ptr< VMTHook > g_pDirectXHook = nullptr;
	std::unique_ptr< VMTHook > g_pClientStateAdd = nullptr;
	std::unique_ptr< VMTHook > g_pPanelHook = nullptr;
	std::unique_ptr< VMTHook > g_pClientModeHook = nullptr;
	std::unique_ptr< VMTHook > g_pClientHook = nullptr;
	std::unique_ptr< VMTHook > g_pRenderViewHook = nullptr;
	std::unique_ptr< VMTHook > g_pStudioRenderHook = nullptr;
	std::unique_ptr< VMTHook > g_pModelRenderHook = nullptr;
	std::unique_ptr< VMTHook > g_pPredictHook = nullptr;
	std::unique_ptr< VMTHook > g_pQueryHook = nullptr;
	std::unique_ptr< VMTHook > g_pGetBoolHook = nullptr;
	std::unique_ptr< VMTHook > g_pCameraHook = nullptr;
	std::unique_ptr< VMTHook > g_pEngineHook = nullptr;
	std::unique_ptr< VMTHook > g_pLeafSystemHook = nullptr;
	std::unique_ptr< VMTHook > g_pVGUIHook = nullptr;
	std::unique_ptr< VMTHook > g_pTEFireBullets = nullptr;
	std::unique_ptr< VMTHook > g_pNetShowFragments = nullptr;
	std::unique_ptr< VMTHook > g_pShadow = nullptr;
	std::unique_ptr< VMTHook > g_pD3DX9;
}
std::uint8_t* pattern_scan(const char* module_name, const char* signature) noexcept {
	const auto module_handle = GetModuleHandleA(module_name);

	if (!module_handle)
		return nullptr;

	static auto pattern_to_byte = [](const char* pattern) {
		auto bytes = std::vector<int>{};
		auto start = const_cast<char*>(pattern);
		auto end = const_cast<char*>(pattern) + std::strlen(pattern);

		for (auto current = start; current < end; ++current) {
			if (*current == '?') {
				++current;

				if (*current == '?')
					++current;

				bytes.push_back(-1);
			}
			else {
				bytes.push_back(std::strtoul(current, &current, 16));
			}
		}
		return bytes;
	};

	auto dos_header = reinterpret_cast<PIMAGE_DOS_HEADER>(module_handle);
	auto nt_headers =
		reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<std::uint8_t*>(module_handle) + dos_header->e_lfanew);

	auto size_of_image = nt_headers->OptionalHeader.SizeOfImage;
	auto pattern_bytes = pattern_to_byte(signature);
	auto scan_bytes = reinterpret_cast<std::uint8_t*>(module_handle);

	auto s = pattern_bytes.size();
	auto d = pattern_bytes.data();

	for (auto i = 0ul; i < size_of_image - s; ++i) {
		bool found = true;

		for (auto j = 0ul; j < s; ++j) {
			if (scan_bytes[i + j] != d[j] && d[j] != -1) {
				found = false;
				break;
			}
		}
		if (found)
		{

			return &scan_bytes[i];
		}

	}

	return nullptr;
}

RecvPropHook* sequence_hook = new RecvPropHook();
RecvPropHook* viewmodel_hook = new RecvPropHook();
int item_def_knifess()
{
	switch (vars.skins.skins_knife_model)
	{
	case 0:
		return 500;
		break;
	case 1:
		return 514;
		break;
	case 2:
		return 515;
		break;
	case 3:
		return 512;
		break;
	case 4:
		return 505;
		break;
	case 5:
		return 506;
		break;
	case 6:
		return 509;
		break;
	case 7:
		return 507;
		break;
	case 8:
		return 508;
		break;
	case 9:
		return 516;
		break;
	case 10:
		return 520;
		break;
	case 11:
		return 522;
		break;
	case 12:
		return 519;
		break;
	case 13:
		return 523;
		break;
	case 14:
		return 503;
		break;
	case 15:
		return 525;
		break;
	case 16:
		return 521;
		break;
	case 17:
		return 518;
		break;
	case 18:
		return 517;
		break;
	default:
		break;
	}
}

namespace sParser {
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

	const weapon_info* GetWeaponInfo(int defindex) {
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

}
namespace DetailFnv
{
	template <typename Type, Type OffsetBasis, Type Prime>
	struct size_dependant_data
	{
		using type = Type;
		constexpr static auto k_offset_basis = OffsetBasis;
		constexpr static auto k_prime = Prime;
	};

	template <size_t Bits>
	struct size_selector;

	template <>
	struct size_selector<32>
	{
		using type = size_dependant_data<std::uint32_t, 0x811c9dc5ul, 16777619ul>;
	};

	template <>
	struct size_selector<64>
	{
		using type = size_dependant_data<std::uint64_t, 0xcbf29ce484222325ull, 1099511628211ull>;
	};

	template <std::size_t Size>
	class fnv_hash
	{
	private:
		using data_t = typename size_selector<Size>::type;

	public:
		using hash = typename data_t::type;

	private:
		constexpr static auto k_offset_basis = data_t::k_offset_basis;
		constexpr static auto k_prime = data_t::k_prime;

	public:
		template <std::size_t N>
		static __forceinline constexpr auto hash_constexpr(const char(&str)[N], const std::size_t size = N) -> hash
		{
			return static_cast<hash>(1ull * (size == 1
				? (k_offset_basis ^ str[0])
				: (hash_constexpr(str, size - 1) ^ str[size - 1])) * k_prime);
		}

		static auto __forceinline hash_runtime(const char* str) -> hash
		{
			auto result = k_offset_basis;
			do
			{
				result ^= *str++;
				result *= k_prime;
			} while (*(str - 1) != '\0');

			return result;
		}
	};
}

using new_fnv = ::DetailFnv::fnv_hash<sizeof(void*) * 8>;

#define FNV(str) (std::integral_constant<new_fnv::hash, new_fnv::hash_constexpr(str)>::value)



static int get_new_animation(const new_fnv::hash model, const int sequence) {
	enum ESequence {
		SEQUENCE_DEFAULT_DRAW = 0,
		SEQUENCE_DEFAULT_IDLE1 = 1,
		SEQUENCE_DEFAULT_IDLE2 = 2,
		SEQUENCE_DEFAULT_LIGHT_MISS1 = 3,
		SEQUENCE_DEFAULT_LIGHT_MISS2 = 4,
		SEQUENCE_DEFAULT_HEAVY_MISS1 = 9,
		SEQUENCE_DEFAULT_HEAVY_HIT1 = 10,
		SEQUENCE_DEFAULT_HEAVY_BACKSTAB = 11,
		SEQUENCE_DEFAULT_LOOKAT01 = 12,
	};
	switch (model) {
	case FNV("models/weapons/v_knife_butterfly.mdl"): {
		switch (sequence) {
		case SEQUENCE_DEFAULT_DRAW: return Math::RandomInt(0, 1);
		case SEQUENCE_DEFAULT_LOOKAT01: return Math::RandomInt(13, 15);
		default: return sequence + 1;
		}
	}
	case FNV("models/weapons/v_knife_falchion_advanced.mdl"): {
		switch (sequence) {
		case SEQUENCE_DEFAULT_IDLE2: return 1;
		case SEQUENCE_DEFAULT_HEAVY_MISS1: return Math::RandomInt(8, 9);
		case SEQUENCE_DEFAULT_LOOKAT01: return Math::RandomInt(12, 13);
		case SEQUENCE_DEFAULT_DRAW:
		case SEQUENCE_DEFAULT_IDLE1: return sequence;
		default: return sequence - 1;
		}
	}
	case FNV("models/weapons/v_knife_push.mdl"): {
		switch (sequence) {
		case SEQUENCE_DEFAULT_IDLE2: return 1;
		case SEQUENCE_DEFAULT_LIGHT_MISS1:
		case SEQUENCE_DEFAULT_LIGHT_MISS2: return Math::RandomInt(2, 6);
		case SEQUENCE_DEFAULT_HEAVY_MISS1: return Math::RandomInt(11, 12);
		case SEQUENCE_DEFAULT_HEAVY_HIT1:
		case SEQUENCE_DEFAULT_HEAVY_BACKSTAB:
		case SEQUENCE_DEFAULT_LOOKAT01: return sequence + 3;
		case SEQUENCE_DEFAULT_DRAW:
		case SEQUENCE_DEFAULT_IDLE1: return sequence;
		default: return sequence + 2;
		}
	}
	case FNV("models/weapons/v_knife_survival_bowie.mdl"): {
		switch (sequence) {
		case SEQUENCE_DEFAULT_DRAW:
		case SEQUENCE_DEFAULT_IDLE1: return sequence;
		case SEQUENCE_DEFAULT_IDLE2: return 1;
		default: return sequence - 1;
		}
	}
	case FNV("models/weapons/v_knife_ursus.mdl"): {
		switch (sequence) {
		case SEQUENCE_DEFAULT_DRAW: return Math::RandomInt(0, 1);
		case SEQUENCE_DEFAULT_LOOKAT01: return Math::RandomInt(13, 14);
		default: return sequence + 1;
		}
	}
	case FNV("models/weapons/v_knife_stiletto.mdl"): {
		switch (sequence) {
		case SEQUENCE_DEFAULT_LOOKAT01: return Math::RandomInt(12, 13);
		}
	}
	case FNV("models/weapons/v_knife_widowmaker.mdl"): {
		switch (sequence) {
		case SEQUENCE_DEFAULT_LOOKAT01: return Math::RandomInt(14, 15);
		}
	}
	default: return sequence;
	}
}




void sequence_remap(CRecvProxyData* data, IBaseViewModel* view_model, IBasePlayer* local) {

	if (!vars.skins.override_knife) 
		return;
	const auto owner = interfaces.ent_list->GetClientEntityFromHandle(view_model->m_hOwner());
	if (local && owner && owner == local)
	{
		auto weapon = local->GetWeapon();
		if (!weapon) return;
		const auto weapon_info = sParser::GetWeaponInfo(weapon->GetItemDefinitionIndex());
		if (!weapon_info) return;
		//Replace anim
		auto& m_sequence = data->m_Value.m_Int;
		m_sequence = get_new_animation(new_fnv::hash_runtime(weapon_info->model), m_sequence);
	}
}
void _cdecl sequence_proxy(const CRecvProxyData* data_const, void* p_struct, void* p_out) {
	const auto data = const_cast<CRecvProxyData*>(data_const);
	const auto view_model = static_cast<IBaseViewModel*>(p_struct);
	if (view_model && data && csgo->local)
		sequence_remap(data, view_model, csgo->local);
	sequence_hook->GetOriginalFunction()(data, p_struct, p_out);
}
void Hooked_RecvProxy_Viewmodel(CRecvProxyData* pData, void* pStruct, void* pOut)
{
	// Get the knife view model id's
	int default_t = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_default_t.mdl");
	int default_ct = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_default_ct.mdl");
	int iBayonet = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_bayonet.mdl");
	int iButterfly = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_butterfly.mdl");
	int iFlip = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_flip.mdl");
	int iGut = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_gut.mdl");
	int iKarambit = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_karam.mdl");
	int iM9Bayonet = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_m9_bay.mdl");
	int iHuntsman = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_tactical.mdl");
	int iFalchion = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_falchion_advanced.mdl");
	int iDagger = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_push.mdl");
	int iBowie = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_survival_bowie.mdl");
	int iGunGame = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_gg.mdl");
	int Navaja = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_gypsy_jackknife.mdl");
	int Stiletto = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_stiletto.mdl");
	int Ursus = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_ursus.mdl");
	int Talon = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_widowmaker.mdl");
	int d1 = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_css.mdl");
	int d2 = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_skeleton.mdl");
	int d3 = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_outdoor.mdl");
	int d4 = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_canis.mdl");
	int d5 = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_cord.mdl");


	const auto local = csgo->local;


	if (local && vars.skins.override_knife)
	{
		if (local->isAlive() && (
			pData->m_Value.m_Int == default_t ||
			pData->m_Value.m_Int == default_ct ||
			pData->m_Value.m_Int == iBayonet ||
			pData->m_Value.m_Int == iFlip ||
			pData->m_Value.m_Int == iGunGame ||
			pData->m_Value.m_Int == iGut ||
			pData->m_Value.m_Int == iKarambit ||
			pData->m_Value.m_Int == iM9Bayonet ||
			pData->m_Value.m_Int == iHuntsman ||
			pData->m_Value.m_Int == iBowie ||
			pData->m_Value.m_Int == iButterfly ||
			pData->m_Value.m_Int == iFalchion ||
			pData->m_Value.m_Int == iDagger ||
			pData->m_Value.m_Int == Navaja ||
			pData->m_Value.m_Int == Stiletto ||
			pData->m_Value.m_Int == Ursus ||
			pData->m_Value.m_Int == Talon ||

			pData->m_Value.m_Int == d1 ||
			pData->m_Value.m_Int == d2 ||
			pData->m_Value.m_Int == d3 ||
			pData->m_Value.m_Int == d4 ||
			pData->m_Value.m_Int == d5))
		{



			const auto weapon_info = sParser::GetWeaponInfo(item_def_knifess());
			if (weapon_info)
				pData->m_Value.m_Int = interfaces.models.model_info->GetModelIndex(weapon_info->model);




		}
	}

	viewmodel_hook->GetOriginalFunction()(pData, pStruct, pOut);
}
using sFn = int __fastcall(void*, void*, uint32_t, const void*, uint32_t);
static sFn* send_message_original;
int __fastcall send_message_hooked(void* thisptr, void*, uint32_t un_msg_type, const void* pub_data, uint32_t cub_data) {
	auto send_message = true;
	uint32_t message_type = un_msg_type & 0x7FFFFFFF;
	if (!send_message) return 0;
	send_message_original(thisptr, nullptr, un_msg_type, const_cast<void*>(pub_data), cub_data);
}

void H::Hook()
{
	for (int i = 0; i < 256; i++) {
		csgo->key_pressed[i] = false;
		csgo->key_down[i] = false;
		csgo->key_pressedticks[i] = 0;
	}

	features->Visuals = new CVisuals();
	features->Bunnyhop = new CBunnyhop();
	features->BulletTracer = new CBulletTracer();
	features->Eventlog = new CEventlog();

	ClassId = new ClassIdManager();

	csgo->client_state = **reinterpret_cast<CClientState***>(
		csgo->Utils.FindPatternIDA(
			GetModuleHandleA(g_Modules[fnv::hash(hs::engine_dll::s().c_str())].c_str()),
			g_Patterns[fnv::hash(hs::client_state::s().c_str())].c_str()) + 2);

	DWORD* dwUpdateClientSideAnimation = (DWORD*)(csgo->Utils.FindPatternIDA(
		GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()),
		g_Patterns[fnv::hash(hs::update_clientside_anim::s().c_str())].c_str()));

	DWORD* dwDoExtraBoneProcessing = (DWORD*)(csgo->Utils.FindPatternIDA(
		GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()),
		g_Patterns[fnv::hash(hs::debp::s().c_str())].c_str()));

	DWORD* dwStandardBlendingRules = (DWORD*)(csgo->Utils.FindPatternIDA(
		GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()),
		g_Patterns[fnv::hash(hs::standard_blending_rules::s().c_str())].c_str()));

	DWORD* dwShouldSkipAnimFrame = (DWORD*)(csgo->Utils.FindPatternIDA(
		GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()),
		g_Patterns[fnv::hash(hs::should_skip_anim_frame::s().c_str())].c_str()));

	DWORD* dwGetForeignFallbackFontName = (DWORD*)(csgo->Utils.FindPatternIDA(
		GetModuleHandleA(g_Modules[fnv::hash(hs::vgui_mat_surface_dll::s().c_str())].c_str()),
		g_Patterns[fnv::hash(hs::get_foreign_fall_back_name::s().c_str())].c_str()));

	DWORD* dwSetupBones = (DWORD*)(csgo->Utils.FindPatternIDA(
		GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()),
		g_Patterns[fnv::hash(hs::setup_bones::s().c_str())].c_str()));

	DWORD* dwBuildTransform = (DWORD*)(csgo->Utils.FindPatternIDA(
		GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()),
		g_Patterns[fnv::hash(hs::build_transformations::s().c_str())].c_str()));

	Effects = **reinterpret_cast<CClientEffectRegistration***>(csgo->Utils.FindPatternIDA(
		GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()),
		g_Patterns[fnv::hash(hs::effects::s().c_str())].c_str()) + 2);

	auto sv_cheats = interfaces.cvars->FindVar(hs::sv_cheats::s().c_str());
	auto camera = interfaces.cvars->FindVar(hs::camera::s().c_str());
	auto shadows = interfaces.cvars->FindVar(hs::shadows::s().c_str());

	g_pClientStateAdd = std::make_unique< VMTHook >();
	g_pEngineHook = std::make_unique< VMTHook >();
	g_pPanelHook = std::make_unique< VMTHook >();
	g_pClientModeHook = std::make_unique< VMTHook >();
	g_pShadow = std::make_unique< VMTHook >();
	g_pStudioRenderHook = std::make_unique< VMTHook >();
	g_pModelRenderHook = std::make_unique< VMTHook >();
	g_pPredictHook = std::make_unique< VMTHook >();
	g_pQueryHook = std::make_unique< VMTHook >();
	g_pCameraHook = std::make_unique< VMTHook >();
	g_pGetBoolHook = std::make_unique< VMTHook >();
	g_pDirectXHook = std::make_unique< VMTHook >();
	g_pRenderViewHook = std::make_unique< VMTHook >();
	g_pLeafSystemHook = std::make_unique< VMTHook >();
	g_pVGUIHook = std::make_unique< VMTHook >();
	g_pClientHook = std::make_unique< VMTHook >();


	g_pEngineHook->Setup(interfaces.engine);
	g_pPanelHook->Setup(interfaces.v_panel);
	g_pClientHook->Setup(interfaces.client);
	g_pClientModeHook->Setup(interfaces.client_mode);
	g_pShadow->Setup(shadows);
	g_pStudioRenderHook->Setup(interfaces.studio_render);
	g_pModelRenderHook->Setup(interfaces.models.model_render);
	g_pPredictHook->Setup(interfaces.prediction);
	g_pQueryHook->Setup(interfaces.engine->GetBSPTreeQuery());
	g_pCameraHook->Setup(camera);
	g_pGetBoolHook->Setup(sv_cheats);
	g_pRenderViewHook->Setup(interfaces.render_view);
	g_pLeafSystemHook->Setup(interfaces.leaf_system);
	g_pVGUIHook->Setup(interfaces.engine_vgui);

	for (auto e = Effects; e; e = e->next)
	{
		if (strstr(e->effectName, hs::impact::s().c_str()) && strlen(e->effectName) <= 8) {
			ImpactCallback = e->function;
			e->function = &Hooked_ImpactCallback;
			break;
		}
	}

	g_pClientHook->Hook(37, Hooked_FrameStageNotify);
	g_pClientHook->Hook(24, Hooked_WriteUsercmdDeltaToBuffer);
	g_pEngineHook->Hook(Index::IsBoxVisible, Hooked_IsBoxVisible);
	g_pEngineHook->Hook(Index::IsHLTV, Hooked_IsHLTV);
	g_pEngineHook->Hook(101, Hooked_GetScreenAspectRatio);
	g_pShadow->Hook(13, Hooked_ShouldDrawShadow);
	g_pClientModeHook->Hook(24, Hooked_CreateMove);
	g_pClientModeHook->Hook(17, Hooked_ShouldDrawFog);
	g_pClientModeHook->Hook(Index::OverrideView, Hooked_OverrideView);
	g_pClientModeHook->Hook(35, Hooked_ViewModel);
	g_pStudioRenderHook->Hook(29, Hooked_DrawModel);
	g_pClientModeHook->Hook(Index::DoPostScreenEffects, Hooked_DoPostScreenEffects);
	g_pPanelHook->Hook(Index::PaintTraverse, Hooked_PaintTraverse);
	g_pStudioRenderHook->Hook(Index::BeginFrame, Hooked_BeginFrame);
	g_pPredictHook->Hook(19, Hooked_RunCommand);
	g_pPredictHook->Hook(Index::InPrediction, Hooked_InPrediction);
	g_pQueryHook->Hook(Index::ListLeavesInBox, Hooked_ListLeavesInBox);
	g_pCameraHook->Hook(Index::InCamera, get_bool);
	g_pGetBoolHook->Hook(Index::GetBool, Hooked_GetBool);
	g_pRenderViewHook->Hook(Index::SceneEnd, Hooked_SceneEnd);

	H::UpdateClientSideAnimation = (UpdateClientSideAnimationFn)DetourFunction((PBYTE)dwUpdateClientSideAnimation, (PBYTE)Hooked_UpdateClientSideAnimation);
	H::DoExtraBoneProcessing = (DoExtraBoneProcessingFn)DetourFunction((PBYTE)dwDoExtraBoneProcessing, (PBYTE)Hooked_DoExtraBoneProcessing);
	H::ShouldSkipAnimFrame = (ShouldSkipAnimFrameFn)DetourFunction((PBYTE)dwShouldSkipAnimFrame, (PBYTE)Hooked_ShouldSkipAnimFrame);
	H::StandardBlendingRules = (StandardBlendingRulesFn)DetourFunction((PBYTE)dwStandardBlendingRules, (PBYTE)Hooked_StandardBlendingRules);
	H::GetForeignFallbackFontName = (GetForeignFallbackFontNameFn)DetourFunction((PBYTE)dwGetForeignFallbackFontName, (PBYTE)Hooked_GetForeignFallbackFontName);

	SteamPresent = *reinterpret_cast<decltype(SteamPresent)*>(CSignatures::Get().Present);
	*reinterpret_cast<decltype(::Hooked_Present)**>(CSignatures::Get().Present) = ::Hooked_Present;

	SteamReset = *reinterpret_cast<decltype(SteamReset)*>(CSignatures::Get().Reset);
	*reinterpret_cast<decltype(::Hooked_Reset)**>(CSignatures::Get().Reset) = ::Hooked_Reset;

	//NetvarHook();
	sequence_hook->SetProxyFunction("CBaseViewModel", "m_nSequence", (RecvVarProxyFn)&sequence_proxy);
	viewmodel_hook->SetProxyFunctionFIX("DT_BaseViewModel", "m_nModelIndex", (RecvVarProxyFn)&Hooked_RecvProxy_Viewmodel);
	if (csgo->Init.Window)
		csgo->Init.OldWindow = (WNDPROC)SetWindowLongPtr(csgo->Init.Window, GWL_WNDPROC, (LONG_PTR)Hooked_WndProc);

	RegListeners();
}

void RecvPropHook::SetProxyFunctionFIX(const char* DTable, const char* NetVar, RecvVarProxyFn user_proxy_fn)
{
	auto pClass = interfaces.client->GetAllClasses();
	while (pClass)
	{
		const char* pszName = pClass->m_pRecvTable->m_pNetTableName;
		if (!strcmp(pszName, DTable))
		{
			for (int i = 0; i < pClass->m_pRecvTable->m_nProps; i++)
			{
				auto pProp = &(pClass->m_pRecvTable->m_pProps[i]);
				const char* name = pProp->m_pVarName;
				if (!strcmp(name, NetVar))
				{

					target_property = pProp;
					original_proxy_fn = (RecvVarProxyFn)pProp->m_ProxyFn;
					target_property->m_ProxyFn = user_proxy_fn;
				}
			}
		}
		pClass = pClass->m_pNext;
	}
}

void RecvPropHook::SetProxyFunction(const char* DTable, const char* NetVar, RecvVarProxyFn user_proxy_fn)
{
	for (ClientClass* pClass = interfaces.client->GetAllClasses(); pClass; pClass = pClass->m_pNext)
	{
		if (!strcmp(pClass->m_pNetworkName, DTable))
		{
			RecvTable* pClassTable = pClass->m_pRecvTable;

			for (int nIndex = 0; nIndex < pClassTable->m_nProps; nIndex++)
			{
				RecvProp* pProp = &pClassTable->m_pProps[nIndex];

				if (!pProp)
					continue;

				if (!strcmp(pProp->m_pVarName, NetVar))
				{
					target_property = pProp;
					original_proxy_fn = (RecvVarProxyFn)pProp->m_ProxyFn;
					target_property->m_ProxyFn = user_proxy_fn;

					return;
				}
			}
		}
	}
}

void H::UnHook()
{
	if (interfaces.inputsystem)
		interfaces.inputsystem->EnableInput(true);

	if (g_Animfix->FakeAnimstate)
		interfaces.memalloc->Free(g_Animfix->FakeAnimstate);

	DetourRemove((PBYTE)H::DoExtraBoneProcessing, (PBYTE)Hooked_DoExtraBoneProcessing);
	DetourRemove((PBYTE)H::UpdateClientSideAnimation, (PBYTE)Hooked_UpdateClientSideAnimation);
	DetourRemove((PBYTE)H::StandardBlendingRules, (PBYTE)Hooked_StandardBlendingRules);
	DetourRemove((PBYTE)H::ShouldSkipAnimFrame, (PBYTE)Hooked_ShouldSkipAnimFrame);
	DetourRemove((PBYTE)H::GetForeignFallbackFontName, (PBYTE)Hooked_GetForeignFallbackFontName);
	*reinterpret_cast<void**>(CSignatures::Get().Present) = SteamPresent;
	*reinterpret_cast<void**>(CSignatures::Get().Reset) = SteamReset;

	g_pVGUIHook->Unhook(14);
	g_pClientHook->Unhook(37);
	g_pClientHook->Unhook(24);
	g_pPanelHook->Unhook(Index::PaintTraverse);
	g_pEngineHook->Unhook(Index::IsBoxVisible);
	g_pEngineHook->Unhook(Index::IsHLTV);
	g_pEngineHook->Unhook(101);
	g_pShadow->Unhook(13);
	g_pClientModeHook->Unhook(35);
	g_pClientModeHook->Unhook(17);
	g_pClientModeHook->Unhook(Index::CreateMove);
	g_pClientModeHook->Unhook(Index::OverrideView);
	g_pClientModeHook->Unhook(Index::DoPostScreenEffects);
	g_pStudioRenderHook->Unhook(Index::BeginFrame);
	g_pStudioRenderHook->Unhook(29);
	g_pModelRenderHook->Unhook(Index::DrawModelExecute);
	g_pPredictHook->Unhook(19);
	g_pPredictHook->Unhook(Index::InPrediction);
	g_pRenderViewHook->Unhook(Index::SceneEnd);
	g_pQueryHook->Unhook(Index::ListLeavesInBox);
	g_pCameraHook->Unhook(Index::InCamera);
	g_pGetBoolHook->Unhook(Index::GetBool);

	for (auto listener : g_pGameEventManager)
		interfaces.event_manager->RemoveListener(listener);

	for (auto head = Effects; head; head = head->next)
	{
		if (strstr(head->effectName, hs::impact::s().c_str()) && strlen(head->effectName) <= 8) {
			head->function = H::ImpactCallback;
			break;
		}
	}
}