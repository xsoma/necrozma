#include "skinchanger.h"

int item_def_knifes()
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
inline  static IBaseCombatWeapon* worldmodel;

void c_skins::glovechanger()
{
	player_info_t localPlayerInfo;

	if (csgo->local)
	{
		if (interfaces.engine->GetPlayerInfo(interfaces.engine->GetLocalPlayer(), &localPlayerInfo))
		{
			DWORD* hMyWearables = (DWORD*)((size_t)csgo->local + 0x2F04);//fixed

			if (hMyWearables)
			{
				if (!interfaces.ent_list->GetClientEntityFromHandle((DWORD)hMyWearables[0]))
				{
					static ClientClass* pClass = nullptr;

					if (!pClass)
						pClass = interfaces.client->GetAllClasses();
					while (pClass)
					{
						if (pClass->m_ClassID == 54)
							break;
						pClass = pClass->m_pNext;
					}

					int iEntry = interfaces.ent_list->GetHighestEntityIndex() + 1;
					int iSerial = Math::RandomInt(0x0, 0xFFF);

					pClass->m_pCreateFn(iEntry, iSerial);
					hMyWearables[0] = iEntry | (iSerial << 16);

					IBaseCombatWeapon* pEnt = (IBaseCombatWeapon*)interfaces.ent_list->GetClientEntityFromHandle((DWORD)hMyWearables[0]);
					IClientNetworkable* pNet = (IClientNetworkable*)pEnt;
					if (pEnt)
					{
						int modelindex = 0;

						if (vars.skins.skins_glove_model == 1)
							modelindex = interfaces.models.model_info->GetModelIndex(("models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound.mdl"));
						else if (vars.skins.skins_glove_model == 2)
							modelindex = interfaces.models.model_info->GetModelIndex(("models/weapons/v_models/arms/glove_sporty/v_glove_sporty.mdl"));
						else if (vars.skins.skins_glove_model == 3)
							modelindex = interfaces.models.model_info->GetModelIndex(("models/weapons/v_models/arms/glove_slick/v_glove_slick.mdl"));
						else if (vars.skins.skins_glove_model == 4)
							modelindex = interfaces.models.model_info->GetModelIndex(("models/weapons/v_models/arms/glove_handwrap_leathery/v_glove_handwrap_leathery.mdl"));
						else if (vars.skins.skins_glove_model == 5)
							modelindex = interfaces.models.model_info->GetModelIndex(("models/weapons/v_models/arms/glove_motorcycle/v_glove_motorcycle.mdl"));
						else if (vars.skins.skins_glove_model == 6)
							modelindex = interfaces.models.model_info->GetModelIndex(("models/weapons/v_models/arms/glove_specialist/v_glove_specialist.mdl"));
						else if (vars.skins.skins_glove_model == 7)
							modelindex = interfaces.models.model_info->GetModelIndex(("models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound_hydra.mdl"));

						int ItemDefinitionIndex;
						if (vars.skins.skins_glove_model == 1)
						{
							ItemDefinitionIndex = 5027;
						}
						else if (vars.skins.skins_glove_model == 2)
						{
							ItemDefinitionIndex = 5030;
						}
						else if (vars.skins.skins_glove_model == 3)
						{
							ItemDefinitionIndex = 5031;
						}
						else if (vars.skins.skins_glove_model == 4)
						{
							ItemDefinitionIndex = 5032;
						}
						else if (vars.skins.skins_glove_model == 5)
						{
							ItemDefinitionIndex = 5033;
						}
						else if (vars.skins.skins_glove_model == 6)
						{
							ItemDefinitionIndex = 5034;
						}
						else if (vars.skins.skins_glove_model == 7)
						{
							ItemDefinitionIndex = 5035;
						}
						else
							ItemDefinitionIndex = 0;

						int paintkit = 1;
						if (vars.skins.skins_glove_model == 1)
						{

							switch (vars.skins.skins_glove_skin)
							{
							case 0:
								paintkit = 10006;
								break;
							case 1:
								paintkit = 10007;
								break;
							case 2:
								paintkit = 10008;
								break;
							case 3:
								paintkit = 10039;
								break;
							default:
								paintkit = 0; break;
							}
						}
						else if (vars.skins.skins_glove_model == 2)
						{
							switch (vars.skins.skins_glove_skin)
							{
							case 0:
								paintkit = 10038; break;
							case 1:
								paintkit = 10037; break;
							case 2:
								paintkit = 10018; break;
							case 3:
								paintkit = 10019; break;
							case 4:
								paintkit = 10048; break;
							case 5:
								paintkit = 10047; break;
							case 6:
								paintkit = 10045; break;
							case 7:
								paintkit = 10046; break;
							default:
								paintkit = 0; break;
							}
						}
						else if (vars.skins.skins_glove_model == 3)
						{
							switch (vars.skins.skins_glove_skin)
							{
							case 0:
								paintkit = 10013; break;
							case 1:
								paintkit = 10015; break;
							case 2:
								paintkit = 10016; break;
							case 3:
								paintkit = 10040; break;
							case 4:
								paintkit = 10043; break;
							case 5:
								paintkit = 10044; break;
							case 6:
								paintkit = 10041; break;
							case 7:
								paintkit = 10042; break;
							default:
								paintkit = 0; break;
							}
						}
						else if (vars.skins.skins_glove_model == 4)
						{
							switch (vars.skins.skins_glove_skin)
							{
							case 0:
								paintkit = 10009; break;
							case 1:
								paintkit = 10010; break;
							case 2:
								paintkit = 10021; break;
							case 3:
								paintkit = 10036; break;
							case 4:
								paintkit = 10053; break;
							case 5:
								paintkit = 10054; break;
							case 6:
								paintkit = 10055; break;
							case 7:
								paintkit = 10056; break;
							default:
								paintkit = 0; break;
							}
						}
						else if (vars.skins.skins_glove_model == 5)
						{
							switch (vars.skins.skins_glove_skin)
							{
							case 0:
								paintkit = 10024; break;
							case 1:
								paintkit = 10026; break;
							case 2:
								paintkit = 10027; break;
							case 3:
								paintkit = 10028; break;
							case 4:
								paintkit = 10050; break;
							case 5:
								paintkit = 10051; break;
							case 6:
								paintkit = 10052; break;
							case 7:
								paintkit = 10049; break;
							default:
								paintkit = 0; break;
							}
						}
						else if (vars.skins.skins_glove_model == 6)
						{
							switch (vars.skins.skins_glove_skin)
							{
							case 0:
								paintkit = 10030; break;
							case 1:
								paintkit = 10033; break;
							case 2:
								paintkit = 10034; break;
							case 3:
								paintkit = 10035; break;
							case 4:
								paintkit = 10061; break;
							case 5:
								paintkit = 10062; break;
							case 6:
								paintkit = 10063; break;
							case 7:
								paintkit = 10064; break;
							default:
								paintkit = 0; break;
							}
						}
						else if (vars.skins.skins_glove_model == 7)
						{
							switch (vars.skins.skins_glove_skin)
							{
							case 0:
								paintkit = 10057; break;
							case 1:
								paintkit = 10058; break;
							case 2:
								paintkit = 10059; break;
							case 3:
								paintkit = 10060; break;
							default:
								paintkit = 0; break;
							}
						}
						*pEnt->fixskins() = ItemDefinitionIndex;
						pEnt->ItemIDHigh() = -1;
						pEnt->GetEntityQuality() = 4;
						pEnt->OwnerXuidLow() = 0;
						pEnt->OwnerXuidHigh() = 0;
						pEnt->FallbackWear() = 0.00000001f;
						pEnt->FallbackPaintKit() = paintkit;
						pEnt->setModelIndex(modelindex);
						//pNet->PreDataUpdate(0);
					}
				}
			}

		}
	}
}
void c_skins::skinchager()
{
	if (interfaces.engine->IsInGame() && interfaces.engine->IsConnected())
	{
		if (csgo->local && csgo->local->isAlive())
		{
			auto weapons = csgo->local->GetWeapons();
			for (auto weapon : weapons)
			{
				if (weapon->GetItemDefinitionIndex() == WEAPON_M4A1S) weapon->FallbackPaintKit() = 361;
				if (weapon->GetItemDefinitionIndex() == WEAPON_AK47) weapon->FallbackPaintKit() = 361;
				if (weapon->GetItemDefinitionIndex() == WEAPON_SCAR20) weapon->FallbackPaintKit() = 361;
				if (weapon->GetItemDefinitionIndex() == WEAPON_G3SG1) weapon->FallbackPaintKit() = 361;
				if (weapon->GetItemDefinitionIndex() == WEAPON_USPS) weapon->FallbackPaintKit() = 361;
				if (weapon->GetItemDefinitionIndex() == WEAPON_DEAGLE) weapon->FallbackPaintKit() = 361;

				if (((IBasePlayer*)weapon)->GetClientClass()->m_ClassID == ClassId->CKnife || ((IBasePlayer*)weapon)->GetClientClass()->m_ClassID == ClassId->CKnifeGG)
				{
					if (!vars.skins.override_knife)
						return;

					weapon->SetItemDefinitionIndex(1) = item_def_knifes();
					const auto weapon_info = c_skins::GetWeaponInfo(item_def_knifes());
					if (!weapon_info) continue;
					int varr = interfaces.models.model_info->GetModelIndex(weapon_info->model);
					weapon->ModelIndex() = varr;
					weapon->ViewModelIndex() = varr;

					if (::worldmodel)
					{
						weapon->m_hViewModel() = varr + 1;
						weapon->ModelIndex() = varr + 1;
					}

					weapon->FallbackPaintKit() = 361;
					weapon->GetEntityQuality() = 3;
				}
				weapon->OwnerXuidLow() = 0;
				weapon->OwnerXuidHigh() = 0;
				weapon->FallbackWear() = 0.001f;
				weapon->ItemIDHigh() = 1;
			}
		}
	}
}