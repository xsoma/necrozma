#pragma once
#include "Hooks.h"
#include "netvar_manager.h"

unsigned int FindInDataMap(datamap_t* pMap, const char* name);
bool CGameTrace::DidHitWorld() const {
	return m_pEnt == interfaces.ent_list->GetClientEntity(0);
}

bool CGameTrace::DidHitNonWorldEntity() const {
	return m_pEnt != nullptr && !DidHitWorld();
}


CUtlVector<matrix>& IBasePlayer::GetBoneCache() {
	static auto m_CachedBoneData = *(DWORD*)(csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()),
		hs::bone_cache::s().c_str()) + 0x2) + 0x4;
	return *(CUtlVector<matrix>*)(uintptr_t(this) + m_CachedBoneData);
}

void IBasePlayer::SetBoneCache(matrix* m) {
	auto cache = GetBoneCache();
	memcpy(cache.Base(), m, sizeof(matrix) * cache.Count());
}

void IBasePlayer::ShootPos(Vector* vec)
{
	static auto Pattern = csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()),
		hs::shoot_pos::s().c_str());

	typedef float* (__thiscall* ShootPosFn)(void*, Vector*);

	static auto Fn = (ShootPosFn)Pattern;

	if (!Fn)
		return;

	Fn(this, vec);
}


void IBasePlayer::UpdateVisibilityAllEntities()
{
	static DWORD callInstruction = (DWORD)csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()),
		hs::visibility::s().c_str()); // get the instruction address

	static uintptr_t* update_visibility_all_entities = nullptr;
	if (update_visibility_all_entities == nullptr) {
		//static auto relative_call = (std::uintptr_t*)(Memory::Scan(GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())]), "E8 ? ? ? ? 83 7D D8 00 7C 0F"));

		//static auto offset = *(uintptr_t*)(relative_call + 0x1);
		//auto update_visibility_all_entities = (uintptr_t*)(relative_call + 5 + offset);

		static DWORD relativeAddress = *(DWORD*)(callInstruction + 1); // read the rel32
		static DWORD nextInstruction = callInstruction + 5; // get the address of next instruction
		update_visibility_all_entities = (uintptr_t*)(nextInstruction + relativeAddress); // our function address will be nextInstruction + relativeAddress
	}
	else
		reinterpret_cast<void(__thiscall*)(void*)>(update_visibility_all_entities)(this);
}
void IBasePlayer::Weaponshootpos(Vector& pos)
{
	CallVFunc<void>(this, 284, pos);
}
Vector IBasePlayer::GetEyePosition()
{
	if (csgo->fake_duck && this->EntIndex() == interfaces.engine->GetLocalPlayer()) {
		auto origin = GetOrigin();

		auto vDuckHullMin = interfaces.game_movement->GetPlayerMins(true);
		auto vStandHullMin = interfaces.game_movement->GetPlayerMins(false);

		float fMore = (vDuckHullMin.z - vStandHullMin.z);

		auto vecDuckViewOffset = interfaces.game_movement->GetPlayerViewOffset(true);
		auto vecStandViewOffset = interfaces.game_movement->GetPlayerViewOffset(false);
		float duckFraction = GetDuckAmount();

		float tempz = ((vecDuckViewOffset.z - fMore) * duckFraction) +
			(vecStandViewOffset.z * (1 - duckFraction));

		origin.z += tempz;

		return origin;
	}

	return GetOrigin() + GetVecViewOffset();
}

void IBasePlayer::DrawServerHitboxes() {
	auto duration = interfaces.global_vars->interval_per_tick * 2.0f;

	auto serverPlayer = GetServerEdict();
	if (serverPlayer) {
		static auto pCall = (uintptr_t*)(csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnv::hash(hs::server_dll::s().c_str())].c_str()),
			hs::server_hitbox::s().c_str()));
		float fDuration = duration;

		__asm
		{
			pushad
			movss xmm1, fDuration
			push 1 //bool monoColor
			mov ecx, serverPlayer
			call pCall
			popad
		}
	}
}
uint8_t* IBasePlayer::GetServerEdict() {
	static uintptr_t pServerGlobals = **(uintptr_t**)(csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnv::hash(hs::server_dll::s().c_str())].c_str()),
		hs::server_edict::s().c_str()) + 0x2);
	int iMaxClients = *(int*)((uintptr_t)pServerGlobals + 0x18);
	int index = GetIndex();
	if (index > 0 && iMaxClients >= 1) {
		if (index <= iMaxClients) {
			int v10 = index * 16;
			uintptr_t v11 = *(uintptr_t*)(pServerGlobals + 96);
			if (v11) {
				if (!((*(uintptr_t*)(v11 + v10) >> 1) & 1)) {
					uintptr_t v12 = *(uintptr_t*)(v10 + v11 + 12);
					if (v12) {
						uint8_t* pReturn = nullptr;

						// abusing asm is not good
						__asm
						{
							pushad
							mov ecx, v12
							mov eax, dword ptr[ecx]
							call dword ptr[eax + 0x14]
							mov pReturn, eax
							popad
						}

						return pReturn;
					}
				}
			}
		}
	}
	return nullptr;
}
void IBasePlayer::InvalidateBoneCache()
{
	this->get_most_recent_model_bone_counter() = 0;
	this->get_last_bone_setup_time() = -FLT_MAX;
}

bool IBasePlayer::IsValid()
{
	if (this == nullptr
		|| !this->IsPlayer()
		|| this->IsDormant()
		|| !this->isAlive()
		|| this == csgo->local
		|| this->GetTeam() == csgo->local->GetTeam())
		return false;

	return true;
}

Vector& IBasePlayer::GetMins() {
	static int offset = netvars.GetOffset(hs::DT_BaseEntity::s().c_str(), hs::m_vecMins::s().c_str());
	return *(Vector*)((DWORD)this + offset);
}

Vector& IBasePlayer::GetMaxs() {
	static int offset = netvars.GetOffset(hs::DT_BaseEntity::s().c_str(), hs::m_vecMaxs::s().c_str());
	return *(Vector*)((DWORD)this + offset);
}

HANDLE IBasePlayer::GetObserverTargetHandle()
{
	static int m_hObserverTarget = netvars.GetOffset(hs::DT_BasePlayer::s().c_str(), hs::m_hObserverTarget::s().c_str());
	return *(HANDLE*)((uintptr_t)this + m_hObserverTarget);
}

Vector& IBasePlayer::GetAbsVelocity()
{
	static unsigned int offset = FindInDataMap(GetPredDescMap(), hs::m_vecAbsVelocity::s().c_str());
	return *(Vector*)((uintptr_t)this + offset);
}
int& IBasePlayer::GetGroundEntity()
{
	static unsigned int offset = FindInDataMap(GetPredDescMap(), hs::m_hGroundEntity::s().c_str());
	return *(int*)((uintptr_t)this + offset);
}

void IBasePlayer::SetLowerBodyYaw(float value)
{
	static int m_flLowerBodyYawTarget = netvars.GetOffset(hs::DT_CSPlayer::s().c_str(), hs::m_flLowerBodyYawTarget::s().c_str());
	*reinterpret_cast<float*>(uintptr_t(this) + m_flLowerBodyYawTarget) = value;
}

void IBasePlayer::SetVelocity(Vector velocity)
{
	*reinterpret_cast<Vector*>(uintptr_t(this) + 0x110) = velocity;
}

void IBasePlayer::SetupBonesFix()
{
}

player_info_t IBasePlayer::GetPlayerInfo()
{
	player_info_t pinfo;
	interfaces.engine->GetPlayerInfo(GetIndex(), &pinfo);
	return pinfo;
}

void IBasePlayer::SetFlags(int flags)
{
	static int offset = netvars.GetOffset(hs::DT_BasePlayer::s().c_str(), hs::m_fFlags::s().c_str());
	*reinterpret_cast<int*>(uintptr_t(this) + offset) = flags;
}
float& IBasePlayer::HealthShotBoostExpirationTime()
{
	static int offset = netvars.GetOffset(hs::DT_CSPlayer::s().c_str(), hs::m_flHealthShotBoostExpirationTime::s().c_str());
	return *(float*)((DWORD)this + offset);
}
void IBasePlayer::ResetAnimationState(CCSGOPlayerAnimState* state)
{
	if (!state)
		return;

	using ResetAnimState_t = void(__thiscall*)(CCSGOPlayerAnimState*);
	static auto ResetAnimState = (ResetAnimState_t)csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()),
		hs::reset_animstate::s().c_str());
	if (!ResetAnimState)
		return;

	ResetAnimState(state);
}

void IBasePlayer::CreateAnimationState(CCSGOPlayerAnimState* state)
{
	using CreateAnimState_t = void(__thiscall*)(CCSGOPlayerAnimState*, IBasePlayer*);
	static auto CreateAnimState = (CreateAnimState_t)csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()),
		hs::create_animstate::s().c_str());
	if (!CreateAnimState)
		return;

	CreateAnimState(state, this);
}

Vector IBasePlayer::GetBaseVelocity()
{
	static int offset = netvars.GetOffset(hs::DT_CSPlayer::s().c_str(), hs::m_vecBaseVelocity::s().c_str());
	return *reinterpret_cast<Vector*>((DWORD)this + offset);
}

void IBasePlayer::UpdateAnimationState(CCSGOPlayerAnimState* state, Vector angle)
{
	using fn = void(__vectorcall*)(void*, void*, float, float, float, void*);
	static auto ret = reinterpret_cast<fn>(csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()),
		hs::update_animstate::s().c_str()));

	if (!ret)
		return;

	ret(state, NULL, NULL, angle.y, angle.x, NULL);
}

int* IBasePlayer::GetButtons()
{
	static std::uintptr_t m_nButtons = FindInDataMap(this->GetPredDescMap(), hs::m_nButtons::s().c_str());
	return (int*)((std::uintptr_t)this + m_nButtons);
}

int& IBasePlayer::GetButtonLast()
{
	static std::uintptr_t m_afButtonLast = FindInDataMap(this->GetPredDescMap(), (hs::m_afButtonLast::s().c_str()));
	return *(int*)((std::uintptr_t)this + m_afButtonLast);
}

int& IBasePlayer::GetButtonPressed()
{
	static std::uintptr_t m_afButtonPressed = FindInDataMap(this->GetPredDescMap(), (hs::m_afButtonPressed::s().c_str()));
	return *(int*)((std::uintptr_t)this + m_afButtonPressed);
}

int& IBasePlayer::GetButtonReleased()
{
	static std::uintptr_t m_afButtonReleased = FindInDataMap(this->GetPredDescMap(), (hs::m_afButtonReleased::s().c_str()));
	return *(int*)((std::uintptr_t)this + m_afButtonReleased);
}

int IBasePlayer::GetButtonDisabled()
{
	return *(int*)((std::uintptr_t)this + 0x3330);
}

int IBasePlayer::GetButtonForced()
{
	return *(int*)((std::uintptr_t)this + 0x3334);
}

void IBasePlayer::UpdateCollisionBounds()
{
	CallVFunc<void>(this, 339);
}

void IBasePlayer::SetSequence(int iSequence)
{
	CallVFunc<void>(this, 218, iSequence);
}
void IBasePlayer::StudioFrameAdvance()
{
	CallVFunc<void>(this, 219);
}

int IBasePlayer::PostThink()
{
	// @ida postthink: 56 8B 35 ? ? ? ? 57 8B F9 8B CE 8B 06 FF 90 ? ? ? ? 8B 07

	using PostThinkVPhysicsFn = bool(__thiscall*)(IBasePlayer*);
	static auto oPostThinkVPhysics = (PostThinkVPhysicsFn)(csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()),
		hs::post_think::s().c_str()));

	using SimulatePlayerSimulatedEntitiesFn = void(__thiscall*)(IBasePlayer*);
	static auto oSimulatePlayerSimulatedEntities = (SimulatePlayerSimulatedEntitiesFn)(csgo->Utils.FindPatternIDA(GetModuleHandleA(
		g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()),
		hs::simulate_player::s().c_str()));

	// begin lock
	CallVFunc<void>(interfaces.model_cache, 33);

	if (this->isAlive())
	{
		this->UpdateCollisionBounds();

		if (this->GetFlags() & FL_ONGROUND)
			this->GetFallVelocity() = 0.f;

		if (this->GetSequence() == -1)
			this->SetSequence(0);

		this->StudioFrameAdvance();
		oPostThinkVPhysics(this);
	}

	oSimulatePlayerSimulatedEntities(this);
	// end lock
	return CallVFunc<int>(interfaces.model_cache, 34);
}

int* IBasePlayer::GetNextThinkTick()
{
	static int nNextThinkTick = netvars.GetOffset(hs::DT_LocalPlayerExclusive::s().c_str(), hs::nNextThinkTick::s().c_str());
	return (int*)((std::uintptr_t)this + nNextThinkTick);
}

bool IBasePlayer::PhysicsRunThink(int unk01)
{
	using PhysicsRunThinkFn = bool(__thiscall*)(void*, int);
	static auto oPhysicsRunThink = (PhysicsRunThinkFn)csgo->Utils.FindPatternIDA((GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str())),
		hs::physics_run_think::s().c_str());
	return oPhysicsRunThink(this, unk01);
}

void IBasePlayer::SetAbsAngles(const Vector& angles)
{
	using SetAbsAnglesFn = void(__thiscall*)(void*, const Vector& angles);
	static SetAbsAnglesFn SetAbsAngles = (SetAbsAnglesFn)csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()),
		hs::set_abs_angles::s().c_str());
	SetAbsAngles(this, angles);
}

void IBasePlayer::UnkFunc()
{
	static auto fn = reinterpret_cast<void(__thiscall*)(int)>(csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()),
		hs::unk_func::s().c_str()));
	fn(0);
}

void IBasePlayer::SetAbsOrigin(const Vector& origin)
{
	using SetAbsOriginFn = void(__thiscall*)(void*, const Vector& origin);
	static SetAbsOriginFn SetAbsOrigin = (SetAbsOriginFn)csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()),
		hs::set_abs_origin::s().c_str());
	SetAbsOrigin(this, origin);
}

float_t IBasePlayer::m_surfaceFriction()
{
	static unsigned int _m_surfaceFriction = FindInDataMap(GetPredDescMap(), hs::m_surfaceFriction::s().c_str());
	return *(float_t*)((uintptr_t)this + _m_surfaceFriction);
}
void IBasePlayer::SetLocalViewAngles(Vector angle) {
	typedef void(__thiscall* original)(void*, Vector&);
	return getvfunc<original>(this, 372)(this, angle);
}
datamap_t* IBasePlayer::GetDataDescMap()
{
	typedef datamap_t* (__thiscall* o_GetPredDescMap)(void*);
	return getvfunc<o_GetPredDescMap>(this, 15)(this);
}
//VFUNC(set_local_view_angles(QAngle& angle), 372, void(__thiscall*)(void*, QAngle&), angle)
datamap_t* IBasePlayer::GetPredDescMap()
{
	typedef datamap_t* (__thiscall* o_GetPredDescMap)(void*);
	return getvfunc<o_GetPredDescMap>(this, 17)(this);
}

std::array<float, 24>& IBasePlayer::m_flPoseParameter()
{
	static int pos_par_hh = netvars.GetOffset(hs::DT_BaseAnimating::s().c_str(), hs::m_flPoseParameter::s().c_str());
	return *reinterpret_cast<std::array<float, 24>*>(reinterpret_cast<uintptr_t>(this) + pos_par_hh);
}

Vector& IBasePlayer::GetVecViewOffset()
{
	static int iOffset = netvars.GetOffset(hs::DT_BasePlayer::s().c_str(), hs::m_vecViewOffset::s().c_str());
	return *(Vector*)((DWORD)this + iOffset);
}
float& IBasePlayer::GetDuckSpeed()
{
	static auto m_flDuckSpeed = netvars.GetOffset(hs::DT_CSPlayer::s().c_str(), hs::m_flDuckSpeed::s().c_str());
	return *(float*)((DWORD)this + m_flDuckSpeed);
}
float& IBasePlayer::GetDuckAmount()
{
	static int m_flDuckAmount = netvars.GetOffset(hs::DT_CSPlayer::s().c_str(), hs::m_flDuckAmount::s().c_str());
	return *(float*)((DWORD)this + m_flDuckAmount/*0x2FAC*/);
}
void IBasePlayer::SetObserverMode(int value) {
	static int offset = netvars.GetOffset(hs::DT_CSPlayer::s().c_str(), hs::m_iObserverMode::s().c_str());
	*reinterpret_cast<int32_t*>(reinterpret_cast<uintptr_t>(this) + offset) = value;
}

int32_t IBasePlayer::GetObserverMode()
{
	static int offset = netvars.GetOffset(hs::DT_CSPlayer::s().c_str(), hs::m_iObserverMode::s().c_str());
	return *reinterpret_cast<int32_t*>(reinterpret_cast<uintptr_t>(this) + offset);
}

Vector IBasePlayer::GetEyeAngles()
{
	static int offset = netvars.GetOffset(hs::DT_CSPlayer::s().c_str(), hs::m_angEyeAngles::s().c_str());
	return *(Vector*)((DWORD)this + offset);
}
float& IBasePlayer::GetVelocityModifier()
{
	static int offset = netvars.GetOffset(hs::DT_CSPlayer::s().c_str(), hs::m_flVelocityModifier::s().c_str());
	return *(float*)((DWORD)this + offset);
}
Vector* IBasePlayer::GetEyeAnglesPointer()
{
	static int offset = netvars.GetOffset(hs::DT_CSPlayer::s().c_str(), hs::m_angEyeAngles::s().c_str());
	return (Vector*)((DWORD)this + offset);
}
int IBasePlayer::GetSequence()
{
	static int offset = netvars.GetOffset(hs::DT_BaseAnimating::s().c_str(), hs::m_nSequence::s().c_str());
	return *(int*)((DWORD)this + offset);
}
float& IBasePlayer::GetFallVelocity()
{
	static int offset = netvars.GetOffset(hs::DT_BasePlayer::s().c_str(), hs::m_flFallVelocity::s().c_str());
	return *(float*)((DWORD)this + offset);
}
bool IBasePlayer::IsPlayer()
{
	return ((ClientClass*)this->GetClientClass())->m_ClassID == ClassId->CCSPlayer;
}

int IBasePlayer::GetTeam()
{
	if (!this)
		return -1;
	static int offset = netvars.GetOffset(hs::DT_BaseEntity::s().c_str(), hs::m_iTeamNum::s().c_str());
	return *(int*)((DWORD)this + offset);
}

float& IBasePlayer::GetFlashDuration()
{
	return *(float*)((DWORD)this + 0xA420);
}

bool IBasePlayer::IsFlashed() {
	static auto m_flFlashMaxAlpha = netvars.GetOffset(hs::DT_CSPlayer::s().c_str(), hs::m_flFlashMaxAlpha::s().c_str());
	return *(float*)((uintptr_t)this + m_flFlashMaxAlpha - 0x8) > 200.0;
}

int IBasePlayer::GetArmor()
{
	static int offset = netvars.GetOffset(hs::DT_CSPlayer::s().c_str(), hs::m_ArmorValue::s().c_str());
	return *(int*)((DWORD)this + offset);
}

bool IBaseCombatWeapon::m_bReloadVisuallyComplete() {
	static int offset = netvars.GetOffset(hs::DT_WeaponCSBase::s().c_str(), hs::m_bReloadVisuallyComplete::s().c_str());
	return *(bool*)((DWORD)this + offset);
}

int32_t IBaseCombatWeapon::GetZoomLevel() {

	if (!this)
		return -1;
	static int offset = netvars.GetOffset("DT_WeaponCSBaseGun", hs::m_zoomLevel::s().c_str());
	return *(int32_t*)((DWORD)this + offset);
}

int IBaseCombatWeapon::GetAmmo(bool second)
{
	if (!this || this->GetCSWpnData() == nullptr)
		return 0;
	static int offset1 = netvars.GetOffset(hs::DT_BaseCombatWeapon::s().c_str(), hs::m_iClip1::s().c_str());
	static int offset2 = netvars.GetOffset(hs::DT_BaseCombatWeapon::s().c_str(), hs::m_iClip2::s().c_str());
	if (second)
	{
		return this->GetCSWpnData()->m_iMaxAmmo;
	}
	else
		return *(int*)((DWORD)this + offset1);
}
bool IBasePlayer::SelectItem(const char* string, int sub_type) {
	using original = bool(__thiscall*)(void*, const char*, int);
	static auto _func = (original)csgo->Utils.FindPatternIDA((GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str())),
		hs::select_item::s().c_str());
	return _func(this, string, sub_type);
}

void IBasePlayer::StandardBlendingRules(c_studio_hdr* hdr, Vector* pos, Quaternion* q, float curtime, int boneMask)
{

}

void IBasePlayer::BuildTransformations(c_studio_hdr* hdr, Vector* pos, Quaternion* q, const matrix& cameraTransform, int boneMask, BYTE* computed)
{

}

float IBasePlayer::GetLastSeenTime() {
	static float time[65];
	float atime = csgo->get_absolute_time();
	if (!IsDormant()) {
		time[GetIndex()] = atime;
		return 0.f;
	}
	else
		return time[GetIndex()];
}
bool IBasePlayer::BadMatrix() {
	return false;
}
bool IBasePlayer::HasHelmet()
{
	static int m_ArmorValue = netvars.GetOffset(hs::DT_CSPlayer::s().c_str(), hs::m_bHasHelmet::s().c_str());
	return *(bool*)(((DWORD)this + m_ArmorValue));
}

bool IBasePlayer::HeavyArmor()
{
	static int m_bHasHeavyArmor = netvars.GetOffset(hs::DT_CSPlayer::s().c_str(), hs::m_bHasHeavyArmor::s().c_str());
	return *(bool*)(((DWORD)this + m_bHasHeavyArmor));
}

Vector IBasePlayer::GetAbsOriginVec()
{
	static unsigned int offset = FindInDataMap(GetPredDescMap(), hs::m_vecAbsOrigin::s().c_str());
	return *(Vector*)((DWORD)this + offset);
}

Vector IBasePlayer::GetAbsOrigin()
{
	return GetOrigin();
}

float& IBasePlayer::GetSimulationTime()
{
	static int m_flSimulationTime = netvars.GetOffset(hs::DT_BaseEntity::s().c_str(), hs::m_flSimulationTime::s().c_str());
	return *(float*)((DWORD)this + m_flSimulationTime);
}

int& IBasePlayer::GetEFlags()
{
	static unsigned int offset = FindInDataMap(GetPredDescMap(), hs::m_iEFlags::s().c_str());
	return *(int*)((uintptr_t)this + offset);
}

int& IBasePlayer::GetEffects() {
	static unsigned int offset = FindInDataMap(GetPredDescMap(), hs::m_fEffects::s().c_str());
	return *(int*)((uintptr_t)this + offset);
}

float IBaseCombatWeapon::LastShotTime() {
	static int m_fLastShotTime = netvars.GetOffset(hs::DT_WeaponCSBase::s().c_str(), hs::m_fLastShotTime::s().c_str());
	return *(float*)((DWORD)this + m_fLastShotTime);
}

float& IBaseCombatWeapon::GetRecoilIndex() {
	static int m_flRecoilIndex = netvars.GetOffset(hs::DT_WeaponCSBase::s().c_str(), hs::m_flRecoilIndex::s().c_str());
	return *(float*)((DWORD)this + m_flRecoilIndex);
}

float& IBaseCombatWeapon::GetAccuracyPenalty() {
	static int m_fAccuracyPenalty = netvars.GetOffset(hs::DT_WeaponCSBase::s().c_str(), hs::m_fAccuracyPenalty::s().c_str());
	return *(float*)((DWORD)this + m_fAccuracyPenalty);
}
int& IBaseCombatWeapon::m_iWorldModelIndex()
{
	static DWORD tt = netvars.GetOffset("DT_BaseCombatWeapon", "m_hWorldModelIndex");
	return *(int*)((uintptr_t)this + tt);
}


IBaseCombatWeapon* IBaseCombatWeapon::m_hWeaponWorldModel()
{
	static auto tt = ((uintptr_t)this + netvars.GetOffset("DT_BaseCombatWeapon", "m_iWorldModelIndex"));
	return reinterpret_cast<IBaseCombatWeapon*>(interfaces.ent_list->GetClientEntityFromHandle(tt));
}
short& IBaseCombatWeapon::SetItemDefinitionIndex(int pt)
{
	static short null = -1;
	if (!this)
		return null;
	static int offset = netvars.GetOffset(hs::DT_BaseAttributableItem::s().c_str(), hs::m_iItemDefinitionIndex::s().c_str());
	return *(short*)((DWORD)this + offset);
}
short* IBaseCombatWeapon::fixskins()
{
	return (short*)((uintptr_t)this + 0x2FAA);
}

CBaseHandle IBaseViewModel::m_hOwner()
{

	static std::uintptr_t tt = netvars.GetOffset("DT_BaseViewModel", "m_hOwner");
	return *(CBaseHandle*)((std::uintptr_t)this + tt);
}
int& IBaseCombatWeapon::GetEntityQuality()
{
	static int tt = netvars.GetOffset("DT_BaseCombatWeapon", "m_iEntityQuality");
	return *(int*)((uintptr_t)this + tt);
}

int& IBaseCombatWeapon::OwnerXuidLow()
{
	static int tt = netvars.GetOffset("DT_BaseAttributableItem", "m_OriginalOwnerXuidLow");
	return *(int*)((uintptr_t)this + tt);
}
float& IBaseCombatWeapon::FallbackWear()
{
	static int tt = netvars.GetOffset("DT_BaseAttributableItem", "m_flFallbackWear");
	return *(float*)((uintptr_t)this + tt);
}

int& IBaseCombatWeapon::ItemIDHigh()
{
	static int tt = netvars.GetOffset("DT_BaseCombatWeapon", "m_iItemIDHigh");
	return *(int*)((uintptr_t)this + tt);
}

int& IBaseCombatWeapon::OwnerXuidHigh()
{
	static int tt = netvars.GetOffset("DT_BaseAttributableItem", "m_OriginalOwnerXuidHigh");
	return *(int*)((uintptr_t)this + tt);
}
int& IBaseCombatWeapon::ModelIndex()
{
	static int tt = netvars.GetOffset("DT_BaseWeaponWorldModel", "m_nModelIndex");
	return *(int*)((uintptr_t)this + tt);
}

int& IBaseCombatWeapon::ViewModelIndex()
{
	static int tt = netvars.GetOffset("DT_BaseCombatWeapon", "m_iViewModelIndex");
	return *(int*)((uintptr_t)this + tt);
}
int& IBaseCombatWeapon::m_hViewModel()
{
	static int tt = netvars.GetOffset("DT_BaseCombatWeapon", "m_hViewModel");
	return *(int*)((uintptr_t)this + tt);
}
int& IBaseCombatWeapon::FallbackPaintKit()
{
	static int tt = netvars.GetOffset("DT_BaseAttributableItem", "m_nFallbackPaintKit");
	return *(int*)((uintptr_t)this + tt);
}
template<typename T, typename ...Args>
constexpr auto callVirtualMethod123(void* classBase, int index, Args... args) noexcept
{
	return ((*reinterpret_cast<T(__thiscall***)(void*, Args...)>(classBase))[index])(classBase, args...);
}
void IBaseCombatWeapon::setModelIndex(int index)
{
	return callVirtualMethod123<void, int>(this, 75, index);
}

bool IBaseCombatWeapon::StartedArming()
{
	static int offset = netvars.GetOffset(hs::DT_BaseCombatWeapon::s().c_str(), hs::m_bStartedArming::s().c_str());
	return *(bool*)((DWORD)this + offset);
}

bool IBasePlayer::HasGunGameImmunity()
{
	if (!this)
		return false;
	static int offset = netvars.GetOffset(hs::DT_CSPlayer::s().c_str(), hs::m_bGunGameImmunity::s().c_str());
	return *(bool*)((DWORD)this + offset);
}

bool IBasePlayer::SetupBones(matrix* pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime)
{
	void* pRenderable = reinterpret_cast<void*>(uintptr_t(this) + 0x4);
	if (!pRenderable)
		return false;

	typedef bool(__thiscall* Fn)(void*, matrix*, int, int, float);
	return getvfunc<Fn>(pRenderable, 13)(pRenderable, pBoneToWorldOut, nMaxBones, boneMask, currentTime);
}

std::vector< IBaseCombatWeapon* > IBasePlayer::GetWeapons()
{
	int* m_hMyWeapons = reinterpret_cast<int*>((DWORD)this + 0x2DF8);
	std::vector< IBaseCombatWeapon* > list = {};
	for (auto i = 0; i < 64; ++i)
	{
		auto Weapon = interfaces.ent_list->GetClientEntityFromHandle(m_hMyWeapons[i]);
		if (Weapon)
		{
			list.push_back((IBaseCombatWeapon*)Weapon);
		}
	}
	return list;
}

int32_t IBaseCombatWeapon::WeaponMode()
{
	static int m_weaponMode = netvars.GetOffset(hs::DT_WeaponCSBaseGun::s().c_str(), hs::m_weaponMode::s().c_str());
	return *(int32_t*)((DWORD)this + m_weaponMode);
}

CCSWeaponInfo* IBaseCombatWeapon::GetCSWpnData()
{
	if (!this) return nullptr;
	typedef CCSWeaponInfo* (__thiscall* OriginalFn)(void*);
	return getvfunc<OriginalFn>(this, 460)(this);
}

void IBasePlayer::SetCurrentCommand(CUserCmd* cmd)
{
	auto m_hConstraintEntity = netvars.GetOffset(hs::DT_BasePlayer::s().c_str(), hs::m_hConstraintEntity::s().c_str());
	auto m_pCurrentCommand = (m_hConstraintEntity - 0xC);

	if (m_pCurrentCommand)
		*(CUserCmd**)(uintptr_t(this) + m_pCurrentCommand) = cmd;
}

int IBasePlayer::GetTickBase(void)
{
	static int m_nTickBase = netvars.GetOffset(hs::DT_BasePlayer::s().c_str(), hs::m_nTickBase::s().c_str());
	return *(int*)((DWORD)this + m_nTickBase);
}

int* IBasePlayer::GetImpulse()
{
	static std::uintptr_t m_nImpulse = FindInDataMap(GetPredDescMap(), hs::m_nImpulse::s().c_str());
	return (int*)((std::uintptr_t)this + m_nImpulse);
}

float& IBaseViewModel::GetAnimtime()
{
	static std::uintptr_t m_flAnimTime = FindInDataMap(GetPredDescMap(), hs::m_flAnimTime::s().c_str());
	return *(float*)((std::uintptr_t)this + m_flAnimTime);
}
int& IBaseViewModel::ModelIndex()
{
	static int tt = netvars.GetOffset("CBaseCombatWeapon", "m_nModelIndex");
	return *(int*)((uintptr_t)this + tt);
}
float& IBasePlayer::GetAnimtime()
{
	static std::uintptr_t m_flAnimTime = FindInDataMap(GetPredDescMap(), hs::m_flAnimTime::s().c_str());
	return *(float*)((std::uintptr_t)this + m_flAnimTime);
}
float& IBasePlayer::GetCycle()
{
	static std::uintptr_t m_flCycle = FindInDataMap(GetPredDescMap(), hs::m_flCycle::s().c_str());
	return *(float*)((std::uintptr_t)this + m_flCycle);
}
float& IBaseViewModel::GetCycle()
{

	static std::uintptr_t m_flCycle = FindInDataMap(GetPredDescMap(), hs::m_flCycle::s().c_str());
	return *(float*)((std::uintptr_t)this + m_flCycle);
}

int& IBasePlayer::GetTickBasePtr(void)
{
	static int m_nTickBase = netvars.GetOffset(hs::DT_BasePlayer::s().c_str(), hs::m_nTickBase::s().c_str());
	return *(int*)((DWORD)this + m_nTickBase);
}

float& IBasePlayer::GetLBY()
{
	float f = 0.f;
	if (!this)
		return f;
	static int offset = netvars.GetOffset(hs::DT_CSPlayer::s().c_str(), hs::m_flLowerBodyYawTarget::s().c_str());
	return *(float*)((DWORD)this + offset);
}

std::string IBasePlayer::GetName()
{
	return GetPlayerInfo().name;
}
char* IBaseCombatWeapon::get_weapon_name()
{
	if (!this)
		return "UNKNOWN";

	switch (GetItemDefinitionIndex())
	{
	case weapon_bayonet:
		return "BAYONET";
	case weapon_knife_survival_bowie:
		return "BOWIE";
	case weapon_knife_butterfly:
		return "BUTTERFLY";
	case weapon_knife:
		return "KNIFE";
	case weapon_knife_falchion:
		return "FALCHION";
	case weapon_knife_flip:
		return "FLIP";
	case weapon_knife_gut:
		return "GUT";
	case weapon_knife_karambit:
		return "KARAMBIT";
	case weapon_knife_m9_bayonet:
		return "M9 BAYONET";
	case weapon_knife_t:
		return "KNIFE";
	case weapon_knife_tactical:
		return "TACTICAL KNIFE";
	case weapon_knife_push:
		return "PUSH KNIFE";
	case weapon_deagle:
		return "DEAGLE";
	case weapon_elite:
		return "DUAL BERETTAS";
	case weapon_fiveseven:
		return "FIVE SEVEN";
	case weapon_glock:
		return "GLOCK";
	case weapon_hkp2000:
		return "P2000";
	case weapon_p250:
		return "P250";
	case weapon_usp_silencer:
		return "USP SILENCER";
	case weapon_tec9:
		return "TEC9";
	case weapon_revolver:
		return "REVOLVER";
	case weapon_mac10:
		return "MAC10";
	case weapon_ump:
		return "UMP";
	case weapon_bizon:
		return "BIZON";
	case weapon_mp7:
		return "MP7";
	case weapon_mp5sd:
		return "MP5";
	case weapon_mp9:
		return "MP9";
	case weapon_p90:
		return "P228";
	case weapon_galilar:
		return "GALILAR";
	case weapon_famas:
		return "FAMAS";
	case weapon_m4a1_silencer:
		return "M4A4 SILENCER";
	case weapon_m4a1:
		return "M4A4";
	case weapon_aug:
		return "AUG";
	case weapon_sg556:
		return "SG 556";
	case weapon_ak47:
		return "AK 47";
	case weapon_g3sg1:
		return "G3SG1";
	case weapon_scar20:
		return "SCAR 20";
	case weapon_awp:
		return "AWP";
	case weapon_ssg08:
		return "SCOUT";
	case weapon_xm1014:
		return "XM1014";
	case weapon_sawedoff:
		return "SAWED OFF";
	case weapon_mag7:
		return "MAG 7";
	case weapon_nova:
		return "NOVA";
	case weapon_negev:
		return "NEGEV";
	case weapon_m249:
		return "M249";
	case weapon_taser:
		return "ZEUS";
	case weapon_flashbang:
		return "FLASHBANG";
	case weapon_hegrenade:
		return "HE GRENADE";
	case weapon_smokegrenade:
		return "SMOKE GRENADE";
	case weapon_molotov:
		return "FIRE GRENADE";
	case weapon_decoy:
		return "DECOY";
	case weapon_incgrenade:
		return "FIRE GRENADE";
	case weapon_c4:
		return "BOMB";
	case weapon_cz75a:
		return "CZ 75A";
	default:
		return "UNKNOWN";
	}
}

char* IBaseCombatWeapon::get_weapon_name_icon()
{
	if (!this)
		return "";

	/*switch (GetItemDefinitionIndex())
	{
	case weapon_bayonet:
		return "1";
	case weapon_knife_survival_bowie:
		return "7";
	case weapon_knife_butterfly:
		return "8";
	case weapon_knife:
		return "1";
	case weapon_knife_falchion:
		return "0";
	case weapon_knife_flip:
		return "2";
	case weapon_knife_gut:
		return "3";
	case weapon_knife_karambit:
		return "4";
	case weapon_knife_m9_bayonet:
		return "5";
	case weapon_knife_t:
		return "1";
	case weapon_knife_tactical:
		return "6";
	case weapon_knife_push:
		return "9";
	case weapon_deagle:
		return "A";
	case weapon_elite:
		return "B";
	case weapon_fiveseven:
		return "C";
	case weapon_glock:
		return "D";
	case weapon_hkp2000:
		return "E";
	case weapon_p250:
		return "F";
	case weapon_usp_silencer:
		return "G";
	case weapon_tec9:
		return "H";
	case weapon_revolver:
		return "J";
	case weapon_mac10:
		return "K";
	case weapon_ump:
		return "L";
	case weapon_bizon:
		return "M";
	case weapon_mp7:
		return "N";
	case weapon_mp5sd:
		return "N";
	case weapon_mp9:
		return "O";
	case weapon_p90:
		return "P";
	case weapon_galilar:
		return "Q";
	case weapon_famas:
		return "R";
	case weapon_m4a1_silencer:
		return "T";
	case weapon_m4a1:
		return "S";
	case weapon_aug:
		return "U";
	case weapon_sg556:
		return "V";
	case weapon_ak47:
		return "W";
	case weapon_g3sg1:
		return "X";
	case weapon_scar20:
		return "Y";
	case weapon_awp:
		return "Z";
	case weapon_ssg08:
		return "a";
	case weapon_xm1014:
		return "b";
	case weapon_sawedoff:
		return "c";
	case weapon_mag7:
		return "d";
	case weapon_nova:
		return "e";
	case weapon_negev:
		return "f";
	case weapon_m249:
		return "g";
	case weapon_taser:
		return "h";
	case weapon_flashbang:
		return "l";
	case weapon_hegrenade:
		return "i";
	case weapon_smokegrenade:
		return "m";
	case weapon_molotov:
		return "n";
	case weapon_decoy:
		return "o";
	case weapon_incgrenade:
		return "p";
	case weapon_c4:
		return "q";
	case weapon_cz75a:
		return "I";
	default:
		return "1";
	}*/

	switch (GetItemDefinitionIndex()) {
	case weapon_knife:
	case WEAPON_KNIFE_GG:
	case weapon_knife_butterfly:
	case weapon_knife_falchion:
	case weapon_knife_flip:
	case weapon_knife_gut:
	case WEAPON_KNIFE_GYPSY_JACKKNIFE:
	case weapon_knife_karambit:
	case weapon_knife_m9_bayonet:
	case weapon_knife_push:
	case 525:
	case 522:
	case weapon_knife_survival_bowie:
	case weapon_knife_t:
	case weapon_knife_tactical:
	case 519:
	case 523:
		return "J";
	case weapon_deagle:
		return "F";
	case weapon_elite:
		return "S";
	case weapon_fiveseven:
		return "U";
	case weapon_glock:
		return "C";
	case weapon_hkp2000:
		return "Y";
	case weapon_p250:
		return "Y";
	case weapon_usp_silencer:
		return "Y";
	case weapon_tec9:
		return "C";
	case weapon_cz75a:
		return "Y";
	case weapon_revolver:
		return "F";
	case weapon_mac10:
		return "L";
	case weapon_ump:
		return "Q";
	case weapon_bizon:
		return "D";
	case weapon_mp7:
		return "X";
	case weapon_mp9:
	case weapon_mp5sd:
		return "D";
	case weapon_p90:
		return "M";
	case weapon_galilar:
		return "V";
	case weapon_famas:
		return "T";
	case weapon_m4a1_silencer:
		return "W";
	case weapon_m4a1:
		return "W";
	case weapon_aug:
		return "U";
	case weapon_sg556:
		return "E";
	case weapon_ak47:
		return "B";
	case weapon_g3sg1:
		return "I";
	case weapon_scar20:
		return "I";
	case weapon_awp:
		return "R";
	case weapon_ssg08:
		return "N";
	case weapon_xm1014:
		return "]";
	case weapon_sawedoff:
		return "K";
	case weapon_mag7:
		return "K";
	case weapon_nova:
		return "K";
	case weapon_negev:
		return "Z";
	case weapon_m249:
		return "Z";
	case weapon_taser:
		return "Y";
	case weapon_flashbang:
		return "G";
	case weapon_hegrenade:
		return "H";
	case weapon_smokegrenade:
		return "P";
	case weapon_molotov:
		return "H";
	case weapon_decoy:
		return "G";
	case weapon_incgrenade:
		return "H";
	case weapon_c4:
		return "\\";
	default:
		return " ";
	}
}
float IBaseCombatWeapon::GetPostponeFireReadyTime()
{
	static int offset = netvars.GetOffset(hs::DT_WeaponCSBase::s().c_str(), hs::m_flPostponeFireReadyTime::s().c_str());
	return *(float*)((uintptr_t)this + offset);
}

void IBaseCombatWeapon::SetPostPoneTime(float asdasdasd) {
	static int offset = netvars.GetOffset(hs::DT_WeaponCSBase::s().c_str(), hs::m_flPostponeFireReadyTime::s().c_str());
	*(float*)((uintptr_t)this + offset) = asdasdasd;
}

bool IBasePlayer::IsScoped()
{
	if (!this)
		return false;
	static int offset = netvars.GetOffset(hs::DT_CSPlayer::s().c_str(), hs::m_bIsScoped::s().c_str());
	return *(bool*)((uintptr_t)this + offset);
}

int IBasePlayer::GetSequenceActivity(int sequence)
{
	auto model = this->GetModel();
	if (!model)
		return -1;
	auto hdr = interfaces.models.model_info->GetStudioModel(model);

	if (!hdr)
		return -1;

	// c_csplayer vfunc 242, follow calls to find the function.
	static DWORD fn = NULL;

	if (!fn) // 55 8B EC 83 7D 08 FF 56 8B F1 74
		fn = (DWORD)csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()),
			hs::seq_activity::s().c_str());

	static auto GetSequenceActivity = reinterpret_cast<int(__fastcall*)(void*, studiohdr_t*, int)>(fn);

	return GetSequenceActivity(this, hdr, sequence);
}

matrix& IBasePlayer::GetrgflCoordinateFrame()
{
	static int offset = netvars.GetOffset(hs::DT_BaseEntity::s().c_str(), hs::m_CollisionGroup::s().c_str()) - 48;
	return *(matrix*)(this + offset);
}

IBaseViewModel* IBasePlayer::GetViewModel()
{
	static int offset = netvars.GetOffset(hs::DT_BasePlayer::s().c_str(), hs::m_hViewModel::s().c_str());
	DWORD ViewModelData = *(DWORD*)((DWORD)this + offset);
	return (IBaseViewModel*)interfaces.ent_list->GetClientEntityFromHandle(ViewModelData);
}

IBaseCombatWeapon* IBasePlayer::GetWeapon()
{
	static int offset = netvars.GetOffset(hs::DT_BasePlayer::s().c_str(), hs::m_hActiveWeapon::s().c_str());
	DWORD weaponData = *(DWORD*)((DWORD)this + offset);
	return (IBaseCombatWeapon*)interfaces.ent_list->GetClientEntityFromHandle(weaponData);
}

Vector IBasePlayer::GetPunchAngle()
{
	static int offset = netvars.GetOffset(hs::DT_CSPlayer::s().c_str(), hs::m_aimPunchAngle::s().c_str());
	return *(Vector*)((DWORD)this + offset);
}
Vector& IBasePlayer::GetPunchAngleVel()
{
	static int offset = netvars.GetOffset(hs::DT_CSPlayer::s().c_str(), hs::m_aimPunchAngleVel::s().c_str());
	return *(Vector*)((DWORD)this + offset);
}

Vector* IBasePlayer::GetPunchAnglePtr()
{
	static int offset = netvars.GetOffset(hs::DT_CSPlayer::s().c_str(), hs::m_aimPunchAngle::s().c_str());
	return (Vector*)((DWORD)this + offset);
}

Vector IBasePlayer::GetViewPunchAngle()
{
	static int offset = netvars.GetOffset(hs::DT_BasePlayer::s().c_str(), hs::m_viewPunchAngle::s().c_str());
	return *(Vector*)((DWORD)this + offset);
}

Vector* IBasePlayer::GetViewPunchAnglePtr()
{
	static int offset = netvars.GetOffset(hs::DT_BasePlayer::s().c_str(), hs::m_viewPunchAngle::s().c_str());
	return (Vector*)((DWORD)this + offset);
}

bool IBasePlayer::IsChicken()
{
	if (!this)
	{
		return false;
	}
	ClientClass* cClass = (ClientClass*)this->GetClientClass();

	return cClass->m_ClassID == ClassId->CChicken;
}

Vector& IBasePlayer::GetAbsAngles()
{
	typedef Vector& (__thiscall* Fn)(void*);
	return (getvfunc<Fn>(this, 11))(this);
}

float IBasePlayer::m_angAbsRotation()
{
	static int offset = netvars.GetOffset(hs::DT_CSPlayer::s().c_str(), hs::m_angAbsRotation::s().c_str());
	return *(float*)((DWORD)this + offset);
}

bool IBasePlayer::IsHostage()
{
	if (!this)
	{
		return false;
	}
	ClientClass* cClass = (ClientClass*)this->GetClientClass();

	return cClass->m_ClassID == ClassId->CHostage;
}

Vector IBasePlayer::GetRagdollPos()
{
	static int offset = netvars.GetOffset(hs::DT_Ragdoll::s().c_str(), hs::m_ragPos::s().c_str());
	return *reinterpret_cast<Vector*>(uintptr_t(this) + offset);
}
int IBasePlayer::ragdoll()
{
	static int ragdoll = netvars.GetOffset("DT_CSPlayer", "m_hRagdoll");
	return GetValue<int>(ragdoll);
}
int IBasePlayer::m_nModelIndex()
{
	return *reinterpret_cast<int*>(uintptr_t(this) + netvars.GetOffset("DT_BaseEntity", "m_nModelIndex"));
}
bool& IBasePlayer::GetClientSideAnims()
{
	static int offset = netvars.GetOffset(hs::DT_BaseAnimating::s().c_str(), hs::m_bClientSideAnimation::s().c_str());
	return *reinterpret_cast<bool*>(uintptr_t(this) + offset);
}

float IBasePlayer::GetDSYDelta()
{
	uintptr_t animstate = uintptr_t(this->GetPlayerAnimState());

	float duckammount = *(float*)(animstate + 0xA4);
	float speedfraction = max(0, min(*reinterpret_cast<float*>(animstate + 0xF8), 1));

	float speedfactor = max(0, min(1, *reinterpret_cast<float*> (animstate + 0xFC)));

	float unk1 = ((*reinterpret_cast<float*> (animstate + 0x11C) * -0.30000001) - 0.19999999) * speedfraction;
	float unk2 = unk1 + 1.1f;
	float unk3;

	if (duckammount > 0) {
		unk2 += ((duckammount * speedfactor) * (0.5f - unk2));
	}

	unk3 = *(float*)(animstate + 0x334) * unk2;

	return clamp<float>(unk3, 0.f, 58.f);
}

CCSGOPlayerAnimState* IBasePlayer::GetPlayerAnimState()
{
	if (!this || !this->isAlive())
		return nullptr;
	return *reinterpret_cast<CCSGOPlayerAnimState**>(uintptr_t(this) + 0x3914);
}
//0x325c

int& IBasePlayer::CollusionVph() {
	return *reinterpret_cast<int*>(uintptr_t(this) + 0x325c);
}
void IBasePlayer::InvalidatePhysicsRecursive(int32_t flags) {
	static const auto invalidate_physics_recursive = reinterpret_cast<void(__thiscall*)(IBasePlayer*, int32_t)>(
		csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()), hs::physics_recursive::s().c_str()));
	invalidate_physics_recursive(this, flags);
}

void IBasePlayer::SetAnimState(CCSGOPlayerAnimState* anims) {
	*reinterpret_cast<CCSGOPlayerAnimState**>(uintptr_t(this) + 0x3914) = anims;
}

int IBaseCombatWeapon::GetItemDefinitionIndex()
{
	if (!this)
		return -1;
	static int offset = netvars.GetOffset(hs::DT_BaseAttributableItem::s().c_str(), hs::m_iItemDefinitionIndex::s().c_str());
	return *(short*)((DWORD)this + offset);
}
int& IBaseCombatWeapon::GetItemDefinitionIndex1()
{
	int null = -1;
	if (!this)
		return null;
	static int offset = netvars.GetOffset(hs::DT_BaseAttributableItem::s().c_str(), hs::m_iItemDefinitionIndex::s().c_str());
	return *(int*)((DWORD)this + offset);
}
float& IBasePlayer::m_flNextAttack()
{
	return *(float*)((uintptr_t)this + 0x2D70);
}

bool IBaseCombatWeapon::InReload()
{
	return *(bool*)(this + 0x32A5);
}

float& IBaseCombatWeapon::NextSecondaryAttack()
{
	static int offset = netvars.GetOffset(hs::DT_BaseCombatWeapon::s().c_str(), hs::m_flNextSecondaryAttack::s().c_str());
	return *(float*)((DWORD)this + offset);
}

float IBaseCombatWeapon::NextPrimaryAttack()
{
	static int offset = netvars.GetOffset(hs::DT_BaseCombatWeapon::s().c_str(), hs::m_flNextPrimaryAttack::s().c_str());
	return *(float*)((DWORD)this + offset);
}

float IBaseCombatWeapon::GetLastShotTime()
{
	static int offset = netvars.GetOffset(hs::DT_WeaponCSBase::s().c_str(), hs::m_fLastShotTime::s().c_str());
	return *(float*)((DWORD)this + offset);
}

float CBaseCSGrenade::GetThrowTime()
{
	static int m_fThrowTime = netvars.GetOffset((hs::DT_BaseCSGrenade::s().c_str()), hs::m_fThrowTime::s().c_str());
	return *(float*)((uintptr_t)this + m_fThrowTime);
}

bool IBaseCombatWeapon::CanFire()
{
	if (!this)return false;

	float nextPrimAttack = csgo->weapon->NextPrimaryAttack();
	float serverTime = TICKS_TO_TIME(csgo->local->GetTickBase());// + I::pGlobals->frametime;absoluteframetime

	if (csgo->weapon->GetAmmo(false) < 1 || csgo->weapon->InReload())
		return false;

	/*if (cfg.Rage.AutoFire.autoRevolver)
	{
	if (!aimbotV2->IsRevolverAbleToShoot())
	return false;
	}*/

	if (/*nextPrimAttack <= 0 || */nextPrimAttack < serverTime)
		return true;

}

#pragma region meme

unsigned int FindInDataMap(datamap_t* pMap, const char* name)
{
	while (pMap)
	{
		for (int i = 0; i < pMap->dataNumFields; i++)
		{
			if (pMap->dataDesc[i].fieldName == NULL)
				continue;

			if (strcmp(name, pMap->dataDesc[i].fieldName) == 0)
				return pMap->dataDesc[i].fieldOffset[TD_OFFSET_NORMAL];

			if (pMap->dataDesc[i].fieldType == 10)
			{
				if (pMap->dataDesc[i].td)
				{
					unsigned int offset;

					if ((offset = FindInDataMap(pMap->dataDesc[i].td, name)) != 0)
						return offset;
				}
			}
		}
		pMap = pMap->baseMap;
	}

	return 0;
}
#pragma endregion

ClassIdManager* ClassId = nullptr;

inline int ClassIdManager::GetClassID(const char* classname)
{
	ServerClass* serverclass = interfaces.server->GetAllServerClasses();
	int id = 0;
	while (serverclass)
	{
		if (!strcmp(serverclass->m_pNetworkName, classname))
			return id;
		serverclass = serverclass->m_pNext, id++;
	}
	return -1;
}

ClassIdManager::ClassIdManager()
{
	CAI_BaseNPC = GetClassID("CAI_BaseNPC");
	CAK47 = GetClassID("CAK47");
	CBaseAnimating = GetClassID("CBaseAnimating");
	CBaseAnimatingOverlay = GetClassID("CBaseAnimatingOverlay");
	CBaseAttributableItem = GetClassID("CBaseAttributableItem");
	CBaseButton = GetClassID("CBaseButton");
	CBaseCombatCharacter = GetClassID("CBaseCombatCharacter");
	CBaseCombatWeapon = GetClassID("CBaseCombatWeapon");
	CBaseCSGrenade = GetClassID("CBaseCSGrenade");
	CBaseCSGrenadeProjectile = GetClassID("CBaseCSGrenadeProjectile");
	CBaseDoor = GetClassID("CBaseDoor");
	CBaseEntity = GetClassID("CBaseEntity");
	CBaseFlex = GetClassID("CBaseFlex");
	CBaseGrenade = GetClassID("CBaseGrenade");
	CBaseParticleEntity = GetClassID("CBaseParticleEntity");
	CBasePlayer = GetClassID("CBasePlayer");
	CBasePropDoor = GetClassID("CBasePropDoor");
	CBaseTeamObjectiveResource = GetClassID("CBaseTeamObjectiveResource");
	CBaseTempEntity = GetClassID("CBaseTempEntity");
	CBaseToggle = GetClassID("CBaseToggle");
	CBaseTrigger = GetClassID("CBaseTrigger");
	CBaseViewModel = GetClassID("CBaseViewModel");
	CBaseVPhysicsTrigger = GetClassID("CBaseVPhysicsTrigger");
	CBaseWeaponWorldModel = GetClassID("CBaseWeaponWorldModel");
	CBeam = GetClassID("CBeam");
	CBeamSpotlight = GetClassID("CBeamSpotlight");
	CBoneFollower = GetClassID("CBoneFollower");
	CBRC4Target = GetClassID("CBRC4Target");
	CBreachCharge = GetClassID("CBreachCharge");
	CBreachChargeProjectile = GetClassID("CBreachChargeProjectile");
	CBreakableProp = GetClassID("CBreakableProp");
	CBreakableSurface = GetClassID("CBreakableSurface");
	CC4 = GetClassID("CC4");
	CCascadeLight = GetClassID("CCascadeLight");
	CChicken = GetClassID("CChicken");
	Ccolor_tCorrection = GetClassID("Ccolor_tCorrection");
	Ccolor_tCorrectionVolume = GetClassID("Ccolor_tCorrectionVolume");
	CCSGameRulesProxy = GetClassID("CCSGameRulesProxy");
	CCSPlayer = GetClassID(hs::CCSPlayer::s().c_str());
	CCSPlayerResource = GetClassID("CCSPlayerResource");
	CCSRagdoll = GetClassID("CCSRagdoll");
	CCSTeam = GetClassID("CCSTeam");
	CDangerZone = GetClassID("CDangerZone");
	CDangerZoneController = GetClassID("CDangerZoneController");
	CDEagle = GetClassID("CDEagle");
	CDecoyGrenade = GetClassID("CDecoyGrenade");
	CDecoyProjectile = GetClassID("CDecoyProjectile");
	CDrone = GetClassID("CDrone");
	CDronegun = GetClassID("CDronegun");
	CDynamicLight = GetClassID("CDynamicLight");
	CDynamicProp = GetClassID("CDynamicProp");
	CEconEntity = GetClassID("CEconEntity");
	CEconWearable = GetClassID("CEconWearable");
	CEmbers = GetClassID("CEmbers");
	CEntityDissolve = GetClassID("CEntityDissolve");
	CEntityFlame = GetClassID("CEntityFlame");
	CEntityFreezing = GetClassID("CEntityFreezing");
	CEntityParticleTrail = GetClassID("CEntityParticleTrail");
	CEnvAmbientLight = GetClassID("CEnvAmbientLight");
	CEnvDetailController = GetClassID("CEnvDetailController");
	CEnvDOFController = GetClassID("CEnvDOFController");
	CEnvGasCanister = GetClassID("CEnvGasCanister");
	CEnvParticleScript = GetClassID("CEnvParticleScript");
	CEnvProjectedTexture = GetClassID("CEnvProjectedTexture");
	CEnvQuadraticBeam = GetClassID("CEnvQuadraticBeam");
	CEnvScreenEffect = GetClassID("CEnvScreenEffect");
	CEnvScreenOverlay = GetClassID("CEnvScreenOverlay");
	CEnvTonemapController = GetClassID("CEnvTonemapController");
	CEnvWind = GetClassID("CEnvWind");
	CFEPlayerDecal = GetClassID("CFEPlayerDecal");
	CFireCrackerBlast = GetClassID("CFireCrackerBlast");
	CFireSmoke = GetClassID("CFireSmoke");
	CFireTrail = GetClassID("CFireTrail");
	CFish = GetClassID("CFish");
	CFists = GetClassID("CFists");
	CFlashbang = GetClassID("CFlashbang");
	CFogController = GetClassID("CFogController");
	CFootstepControl = GetClassID("CFootstepControl");
	CFunc_Dust = GetClassID("CFunc_Dust");
	CFunc_LOD = GetClassID("CFunc_LOD");
	CFuncAreaPortalWindow = GetClassID("CFuncAreaPortalWindow");
	CFuncBrush = GetClassID("CFuncBrush");
	CFuncConveyor = GetClassID("CFuncConveyor");
	CFuncLadder = GetClassID("CFuncLadder");
	CFuncMonitor = GetClassID("CFuncMonitor");
	CFuncMoveLinear = GetClassID("CFuncMoveLinear");
	CFuncOccluder = GetClassID("CFuncOccluder");
	CFuncReflectiveGlass = GetClassID("CFuncReflectiveGlass");
	CFuncRotating = GetClassID("CFuncRotating");
	CFuncSmokeVolume = GetClassID("CFuncSmokeVolume");
	CFuncTrackTrain = GetClassID("CFuncTrackTrain");
	CGameRulesProxy = GetClassID("CGameRulesProxy");
	CGrassBurn = GetClassID("CGrassBurn");
	CHandleTest = GetClassID("CHandleTest");
	CHEGrenade = GetClassID("CHEGrenade");
	CHostage = GetClassID("CHostage");
	CHostageCarriableProp = GetClassID("CHostageCarriableProp");
	CIncendiaryGrenade = GetClassID("CIncendiaryGrenade");
	CInferno = GetClassID("CInferno");
	CInfoLadderDismount = GetClassID("CInfoLadderDismount");
	CInfoMapRegion = GetClassID("CInfoMapRegion");
	CInfoOverlayAccessor = GetClassID("CInfoOverlayAccessor");
	CItem_Healthshot = GetClassID("CItem_Healthshot");
	CItemCash = GetClassID("CItemCash");
	CItemDogtags = GetClassID("CItemDogtags");
	CKnife = GetClassID("CKnife");
	CKnifeGG = GetClassID("CKnifeGG");
	CLightGlow = GetClassID("CLightGlow");
	CMaterialModifyControl = GetClassID("CMaterialModifyControl");
	CMelee = GetClassID("CMelee");
	CMolotovGrenade = GetClassID("CMolotovGrenade");
	CMolotovProjectile = GetClassID("CMolotovProjectile");
	CMovieDisplay = GetClassID("CMovieDisplay");
	CParadropChopper = GetClassID("CParadropChopper");
	CParticleFire = GetClassID("CParticleFire");
	CParticlePerformanceMonitor = GetClassID("CParticlePerformanceMonitor");
	CParticleSystem = GetClassID("CParticleSystem");
	CPhysBox = GetClassID("CPhysBox");
	CPhysBoxMultiplayer = GetClassID("CPhysBoxMultiplayer");
	CPhysicsProp = GetClassID("CPhysicsProp");
	CPhysicsPropMultiplayer = GetClassID("CPhysicsPropMultiplayer");
	CPhysMagnet = GetClassID("CPhysMagnet");
	CPhysPropAmmoBox = GetClassID("CPhysPropAmmoBox");
	CPhysPropLootCrate = GetClassID("CPhysPropLootCrate");
	CPhysPropRadarJammer = GetClassID("CPhysPropRadarJammer");
	CPhysPropWeaponUpgrade = GetClassID("CPhysPropWeaponUpgrade");
	CPlantedC4 = GetClassID("CPlantedC4");
	CPlasma = GetClassID("CPlasma");
	CPlayerResource = GetClassID("CPlayerResource");
	CPointCamera = GetClassID("CPointCamera");
	CPointCommentaryNode = GetClassID("CPointCommentaryNode");
	CPointWorldText = GetClassID("CPointWorldText");
	CPoseController = GetClassID("CPoseController");
	CPostProcessController = GetClassID("CPostProcessController");
	CPrecipitation = GetClassID("CPrecipitation");
	CPrecipitationBlocker = GetClassID("CPrecipitationBlocker");
	CPredictedViewModel = GetClassID("CPredictedViewModel");
	CProp_Hallucination = GetClassID("CProp_Hallucination");
	CPropCounter = GetClassID("CPropCounter");
	CPropDoorRotating = GetClassID("CPropDoorRotating");
	CPropJeep = GetClassID("CPropJeep");
	CPropVehicleDriveable = GetClassID("CPropVehicleDriveable");
	CRagdollManager = GetClassID("CRagdollManager");
	CRagdollProp = GetClassID("CRagdollProp");
	CRagdollPropAttached = GetClassID("CRagdollPropAttached");
	CRopeKeyframe = GetClassID("CRopeKeyframe");
	CSCAR17 = GetClassID("CSCAR17");
	CSceneEntity = GetClassID("CSceneEntity");
	CSensorGrenade = GetClassID("CSensorGrenade");
	CSensorGrenadeProjectile = GetClassID("CSensorGrenadeProjectile");
	CShadowControl = GetClassID("CShadowControl");
	CSlideshowDisplay = GetClassID("CSlideshowDisplay");
	CSmokeGrenade = GetClassID("CSmokeGrenade");
	CSmokeGrenadeProjectile = GetClassID("CSmokeGrenadeProjectile");
	CSmokeStack = GetClassID("CSmokeStack");
	CSnowball = GetClassID("CSnowball");
	CSnowballPile = GetClassID("CSnowballPile");
	CSnowballProjectile = GetClassID("CSnowballProjectile");
	CSpatialEntity = GetClassID("CSpatialEntity");
	CSpotlightEnd = GetClassID("CSpotlightEnd");
	CSprite = GetClassID("CSprite");
	CSpriteOriented = GetClassID("CSpriteOriented");
	CSpriteTrail = GetClassID("CSpriteTrail");
	CStatueProp = GetClassID("CStatueProp");
	CSteamJet = GetClassID("CSteamJet");
	CSun = GetClassID("CSun");
	CSunlightShadowControl = GetClassID("CSunlightShadowControl");
	CSurvivalSpawnChopper = GetClassID("CSurvivalSpawnChopper");
	CTablet = GetClassID("CTablet");
	CTeam = GetClassID("CTeam");
	CTeamplayRoundBasedRulesProxy = GetClassID("CTeamplayRoundBasedRulesProxy");
	CTEArmorRicochet = GetClassID("CTEArmorRicochet");
	CTEBaseBeam = GetClassID("CTEBaseBeam");
	CTEBeamEntPoint = GetClassID("CTEBeamEntPoint");
	CTEBeamEnts = GetClassID("CTEBeamEnts");
	CTEBeamFollow = GetClassID("CTEBeamFollow");
	CTEBeamLaser = GetClassID("CTEBeamLaser");
	CTEBeamPoints = GetClassID("CTEBeamPoints");
	CTEBeamRing = GetClassID("CTEBeamRing");
	CTEBeamRingPoint = GetClassID("CTEBeamRingPoint");
	CTEBeamSpline = GetClassID("CTEBeamSpline");
	CTEBloodSprite = GetClassID("CTEBloodSprite");
	CTEBloodStream = GetClassID("CTEBloodStream");
	CTEBreakModel = GetClassID("CTEBreakModel");
	CTEBSPDecal = GetClassID("CTEBSPDecal");
	CTEBubbles = GetClassID("CTEBubbles");
	CTEBubbleTrail = GetClassID("CTEBubbleTrail");
	CTEClientProjectile = GetClassID("CTEClientProjectile");
	CTEDecal = GetClassID("CTEDecal");
	CTEDust = GetClassID("CTEDust");
	CTEDynamicLight = GetClassID("CTEDynamicLight");
	CTEEffectDispatch = GetClassID("CTEEffectDispatch");
	CTEEnergySplash = GetClassID("CTEEnergySplash");
	CTEExplosion = GetClassID("CTEExplosion");
	CTEFireBullets = GetClassID("CTEFireBullets");
	CTEFizz = GetClassID("CTEFizz");
	CTEFootprintDecal = GetClassID("CTEFootprintDecal");
	CTEFoundryHelpers = GetClassID("CTEFoundryHelpers");
	CTEGaussExplosion = GetClassID("CTEGaussExplosion");
	CTEGlowSprite = GetClassID("CTEGlowSprite");
	CTEImpact = GetClassID("CTEImpact");
	CTEKillPlayerAttachments = GetClassID("CTEKillPlayerAttachments");
	CTELargeFunnel = GetClassID("CTELargeFunnel");
	CTEMetalSparks = GetClassID("CTEMetalSparks");
	CTEMuzzleFlash = GetClassID("CTEMuzzleFlash");
	CTEParticleSystem = GetClassID("CTEParticleSystem");
	CTEPhysicsProp = GetClassID("CTEPhysicsProp");
	CTEPlantBomb = GetClassID("CTEPlantBomb");
	CTEPlayerAnimEvent = GetClassID("CTEPlayerAnimEvent");
	CTEPlayerDecal = GetClassID("CTEPlayerDecal");
	CTEProjectedDecal = GetClassID("CTEProjectedDecal");
	CTERadioIcon = GetClassID("CTERadioIcon");
	CTEShatterSurface = GetClassID("CTEShatterSurface");
	CTEShowLine = GetClassID("CTEShowLine");
	CTesla = GetClassID("CTesla");
	CTESmoke = GetClassID("CTESmoke");
	CTESparks = GetClassID("CTESparks");
	CTESprite = GetClassID("CTESprite");
	CTESpriteSpray = GetClassID("CTESpriteSpray");
	CTest_ProxyToggle_Networkable = GetClassID("CTest_ProxyToggle_Networkable");
	CTestTraceline = GetClassID("CTestTraceline");
	CTEWorldDecal = GetClassID("CTEWorldDecal");
	CTriggerPlayerMovement = GetClassID("CTriggerPlayerMovement");
	CTriggerSoundOperator = GetClassID("CTriggerSoundOperator");
	CVGuiScreen = GetClassID("CVGuiScreen");
	CVoteController = GetClassID("CVoteController");
	CWaterBullet = GetClassID("CWaterBullet");
	CWaterLODControl = GetClassID("CWaterLODControl");
	CWeaponAug = GetClassID("CWeaponAug");
	CWeaponAWP = GetClassID("CWeaponAWP");
	CWeaponBaseItem = GetClassID("CWeaponBaseItem");
	CWeaponBizon = GetClassID("CWeaponBizon");
	CWeaponCSBase = GetClassID("CWeaponCSBase");
	CWeaponCSBaseGun = GetClassID("CWeaponCSBaseGun");
	CWeaponCycler = GetClassID("CWeaponCycler");
	CWeaponElite = GetClassID("CWeaponElite");
	CWeaponFamas = GetClassID("CWeaponFamas");
	CWeaponFiveSeven = GetClassID("CWeaponFiveSeven");
	CWeaponG3SG1 = GetClassID("CWeaponG3SG1");
	CWeaponGalil = GetClassID("CWeaponGalil");
	CWeaponGalilAR = GetClassID("CWeaponGalilAR");
	CWeaponGlock = GetClassID("CWeaponGlock");
	CWeaponHKP2000 = GetClassID("CWeaponHKP2000");
	CWeaponM249 = GetClassID("CWeaponM249");
	CWeaponM3 = GetClassID("CWeaponM3");
	CWeaponM4A1 = GetClassID("CWeaponM4A1");
	CWeaponMAC10 = GetClassID("CWeaponMAC10");
	CWeaponMag7 = GetClassID("CWeaponMag7");
	CWeaponMP5Navy = GetClassID("CWeaponMP5Navy");
	CWeaponMP7 = GetClassID("CWeaponMP7");
	CWeaponMP9 = GetClassID("CWeaponMP9");
	CWeaponNegev = GetClassID("CWeaponNegev");
	CWeaponNOVA = GetClassID("CWeaponNOVA");
	CWeaponP228 = GetClassID("CWeaponP228");
	CWeaponP250 = GetClassID("CWeaponP250");
	CWeaponP90 = GetClassID("CWeaponP90");
	CWeaponSawedoff = GetClassID("CWeaponSawedoff");
	CWeaponSCAR20 = GetClassID("CWeaponSCAR20");
	CWeaponScout = GetClassID("CWeaponScout");
	CWeaponSG550 = GetClassID("CWeaponSG550");
	CWeaponSG552 = GetClassID("CWeaponSG552");
	CWeaponSG556 = GetClassID("CWeaponSG556");
	CWeaponSSG08 = GetClassID("CWeaponSSG08");
	CWeaponTaser = GetClassID("CWeaponTaser");
	CWeaponTec9 = GetClassID("CWeaponTec9");
	CWeaponTMP = GetClassID("CWeaponTMP");
	CWeaponUMP45 = GetClassID("CWeaponUMP45");
	CWeaponUSP = GetClassID("CWeaponUSP");
	CWeaponXM1014 = GetClassID("CWeaponXM1014");
	CWorld = GetClassID("CWorld");
	CWorldVguiText = GetClassID("CWorldVguiText");
	DustTrail = GetClassID("DustTrail");
	MovieExplosion = GetClassID("MovieExplosion");
	ParticleSmokeGrenade = GetClassID("ParticleSmokeGrenade");
	RocketTrail = GetClassID("RocketTrail");
	SmokeTrail = GetClassID("SmokeTrail");
	SporeExplosion = GetClassID("SporeExplosion");
	SporeTrail = GetClassID("SporeTrail");
}