//#include <algorithm>
//#include "skinchanger.h"
//typedef void(*RecvVarProxyFn)(const CRecvProxyData* pData, void* pStruct, void* pOut);
//
//#define SEQUENCE_DEFAULT_DRAW 0
//#define SEQUENCE_DEFAULT_IDLE1 1
//#define SEQUENCE_DEFAULT_IDLE2 2
//#define SEQUENCE_DEFAULT_LIGHT_MISS1 3
//#define SEQUENCE_DEFAULT_LIGHT_MISS2 4
//#define SEQUENCE_DEFAULT_HEAVY_MISS1 9
//#define SEQUENCE_DEFAULT_HEAVY_HIT1 10
//#define SEQUENCE_DEFAULT_HEAVY_BACKSTAB 11
//#define SEQUENCE_DEFAULT_LOOKAT01 12
//
//#define SEQUENCE_BUTTERFLY_DRAW 0
//#define SEQUENCE_BUTTERFLY_DRAW2 1
//#define SEQUENCE_BUTTERFLY_LOOKAT01 13
//#define SEQUENCE_BUTTERFLY_LOOKAT03 15
//
//#define SEQUENCE_FALCHION_IDLE1 1
//#define SEQUENCE_FALCHION_HEAVY_MISS1 8
//#define SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP 9
//#define SEQUENCE_FALCHION_LOOKAT01 12
//#define SEQUENCE_FALCHION_LOOKAT02 13
//
//#define SEQUENCE_DAGGERS_IDLE1 1
//#define SEQUENCE_DAGGERS_LIGHT_MISS1 2
//#define SEQUENCE_DAGGERS_LIGHT_MISS5 6
//#define SEQUENCE_DAGGERS_HEAVY_MISS2 11
//#define SEQUENCE_DAGGERS_HEAVY_MISS1 12
//
//#define SEQUENCE_BOWIE_IDLE1 1
//
//#define SEQUENCE_CSS1 13
//#define SEQUENCE_CSS2 14
//
//using namespace std;
//inline int RandomSequence(int low, int high) {
//	return (rand() % (high - low + 1) + low);
//}
//
//#define	LIFE_ALIVE 0
//#define RandomInt(nMin, nMax) (rand() % (nMax - nMin + 1) + nMin);
//
//RecvVarProxyFn fnSequenceProxyFn = nullptr;
//RecvVarProxyFn oRecvnModelIndex;
//
//void Hooked_RecvProxy_Viewmodel(CRecvProxyData* pData, void* pStruct, void* pOut)
//{
//	// Get the knife view model id's
//	int default_t = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_default_t.mdl");
//	int default_ct = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_default_ct.mdl");
//	int iBayonet = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_bayonet.mdl");
//	int iButterfly = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_butterfly.mdl");
//	int iFlip = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_flip.mdl");
//	int iGut = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_gut.mdl");
//	int iKarambit = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_karam.mdl");
//	int iM9Bayonet = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_m9_bay.mdl");
//	int iHuntsman = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_tactical.mdl");
//	int iFalchion = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_falchion_advanced.mdl");
//	int iDagger = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_push.mdl");
//	int iBowie = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_survival_bowie.mdl");
//	int iGunGame = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_gg.mdl");
//	int Navaja = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_gypsy_jackknife.mdl");
//	int Stiletto = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_stiletto.mdl");
//	int Ursus = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_ursus.mdl");
//	int Talon = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_widowmaker.mdl");
//	int css = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_css.mdl");
//	int skeleton = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_skeleton.mdl");
//	int classic = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_css.mdl");
//	int outdoor = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_outdoor.mdl");
//
//
//	int canis = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_canis.mdl");
//	int cord = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_cord.mdl");
//	// Get local player (just to stop replacing spectators knives)
//	auto pLocal = csgo->local;
//
//	if (!pLocal)
//		return;
//
//	if (vars.skins.skins_enable && pLocal)
//	{
//		// If we are alive and holding a default knife (if we already have a knife don't worry about changing)
//		if (pLocal->isAlive() && (
//			pData->m_Value.m_Int == default_t ||
//			pData->m_Value.m_Int == default_ct ||
//			pData->m_Value.m_Int == iBayonet ||
//			pData->m_Value.m_Int == iFlip ||
//			pData->m_Value.m_Int == iGunGame ||
//			pData->m_Value.m_Int == iGut ||
//			pData->m_Value.m_Int == iKarambit ||
//			pData->m_Value.m_Int == iM9Bayonet ||
//			pData->m_Value.m_Int == iHuntsman ||
//			pData->m_Value.m_Int == iBowie ||
//			pData->m_Value.m_Int == iButterfly ||
//			pData->m_Value.m_Int == iFalchion ||
//			pData->m_Value.m_Int == iDagger ||
//			pData->m_Value.m_Int == Navaja ||
//			pData->m_Value.m_Int == Stiletto ||
//			pData->m_Value.m_Int == Ursus ||
//			pData->m_Value.m_Int == Talon ||
//			pData->m_Value.m_Int == css))
//		{
//			int Model = vars.skins.skins_knife_model;
//
//			// Set whatever knife we want
//			if (Model == 0) // Bayonet
//			{
//				pData->m_Value.m_Int = iBayonet;
//
//
//			}
//			else if (Model == 1) // Bowie Knife
//			{
//				pData->m_Value.m_Int = iBowie;
//
//			}
//			else if (Model == 2) // Butterfly Knife
//			{
//				pData->m_Value.m_Int = iButterfly;
//
//			}
//			else if (Model == 3) // Falchion Knife
//			{
//				pData->m_Value.m_Int = iFalchion;
//
//			}
//			else if (Model == 4) // Flip Knife
//			{
//				pData->m_Value.m_Int = iFlip;
//
//			}
//			else if (Model == 5) // Gut Knife
//			{
//				pData->m_Value.m_Int = iGut;
//
//			}
//			else if (Model == 6) // Huntsman Knife
//			{
//				pData->m_Value.m_Int = iHuntsman;
//
//			}
//			else if (Model == 7) // Karambit
//			{
//				pData->m_Value.m_Int = iKarambit;
//
//			}
//			else if (Model == 8) // M9 Bayonet
//			{
//				pData->m_Value.m_Int = iM9Bayonet;
//
//			}
//			else if (Model == 9)
//			{
//				pData->m_Value.m_Int = iDagger;
//
//			}
//			else if (Model == 10)
//			{
//				pData->m_Value.m_Int = Navaja;
//
//			}
//			else if (Model == 11) // Shadow Daggers
//			{
//				pData->m_Value.m_Int = Stiletto;
//
//			}
//			else if (Model == 12)
//			{
//				pData->m_Value.m_Int = Ursus;
//
//			}
//			else if (Model == 13)
//			{
//				pData->m_Value.m_Int = Talon;
//
//			}
//			else if (Model == 14)
//			{
//				pData->m_Value.m_Int = skeleton;
//
//			}
//			else if (Model == 15)
//			{
//
//				pData->m_Value.m_Int = outdoor;
//
//
//			}
//			else if (Model == 16)
//			{
//				pData->m_Value.m_Int = canis;
//
//			}
//			else if (Model == 17)
//			{
//				pData->m_Value.m_Int = cord;
//			}
//			else if (Model == 18)
//			{
//				pData->m_Value.m_Int = classic;
//			}
//		}
//	}
//	// Carry on the to original proxy
//	oRecvnModelIndex(pData, pStruct, pOut);
//}
//
//void SetViewModelSequence2(const CRecvProxyData* pDataConst, void* pStruct, void* pOut)
//{
//	// Make the incoming data editable.
//	CRecvProxyData* pData = const_cast<CRecvProxyData*>(pDataConst);
//	if (!pData)
//		return;
//	// Confirm that we are replacing our view model and not someone elses.
//	IBaseViewModel* pViewModel = static_cast<IBaseViewModel*>(pStruct);
//	if (!pViewModel)
//		return;
//	if (pViewModel)
//	{
//		IBasePlayer* pOwner = static_cast<IBasePlayer*>(interfaces.ent_list->GetClientEntity(pViewModel->GetOwner() & 0xFFF));
//		if (!pOwner)
//			return;
//		// Compare the owner entity of this view model to the local player entity.
//		if (pOwner && pOwner->GetIndex() == csgo->local->GetIndex())
//		{
//			// Get the filename of the current view model.
//			std::string szModel = interfaces.models.model_info->GetModelName(interfaces.models.model_info->GetModel(pViewModel->ModelIndex()));
//
//			// Store the current sequence.
//			int m_nSequence = pData->m_Value.m_Int;
//			if (szModel == "models/weapons/v_knife_butterfly.mdl")
//			{
//				// Fix animations for the Butterfly Knife.
//				switch (m_nSequence)
//				{
//				case SEQUENCE_DEFAULT_DRAW:
//					m_nSequence = RandomInt(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
//					break;
//				case SEQUENCE_DEFAULT_LOOKAT01:
//					m_nSequence = RandomInt(SEQUENCE_BUTTERFLY_LOOKAT01, SEQUENCE_BUTTERFLY_LOOKAT03);
//					break;
//				default:
//					m_nSequence++;
//				}
//			}
//			else if (szModel == "models/weapons/v_knife_falchion_advanced.mdl")
//			{
//				// Fix animations for the Falchion Knife.
//				switch (m_nSequence)
//				{
//				case SEQUENCE_DEFAULT_IDLE2:
//					m_nSequence = SEQUENCE_FALCHION_IDLE1; break;
//				case SEQUENCE_DEFAULT_HEAVY_MISS1:
//					m_nSequence = RandomInt(SEQUENCE_FALCHION_HEAVY_MISS1, SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP);
//					break;
//				case SEQUENCE_DEFAULT_LOOKAT01:
//					m_nSequence = RandomInt(SEQUENCE_FALCHION_LOOKAT01, SEQUENCE_FALCHION_LOOKAT02);
//					break;
//				case SEQUENCE_DEFAULT_DRAW:
//				case SEQUENCE_DEFAULT_IDLE1:
//					break;
//				default:
//					m_nSequence--;
//				}
//			}
//			else if (szModel == "models/weapons/v_knife_push.mdl")
//			{
//				// Fix animations for the Shadow Daggers.
//				switch (m_nSequence)
//				{
//				case SEQUENCE_DEFAULT_IDLE2:
//					m_nSequence = SEQUENCE_DAGGERS_IDLE1; break;
//				case SEQUENCE_DEFAULT_LIGHT_MISS1:
//				case SEQUENCE_DEFAULT_LIGHT_MISS2:
//					m_nSequence = RandomInt(SEQUENCE_DAGGERS_LIGHT_MISS1, SEQUENCE_DAGGERS_LIGHT_MISS5);
//					break;
//				case SEQUENCE_DEFAULT_HEAVY_MISS1:
//					m_nSequence = RandomInt(SEQUENCE_DAGGERS_HEAVY_MISS2, SEQUENCE_DAGGERS_HEAVY_MISS1);
//					break;
//				case SEQUENCE_DEFAULT_HEAVY_HIT1:
//				case SEQUENCE_DEFAULT_HEAVY_BACKSTAB:
//				case SEQUENCE_DEFAULT_LOOKAT01:
//					m_nSequence += 3; break;
//				case SEQUENCE_DEFAULT_DRAW:
//				case SEQUENCE_DEFAULT_IDLE1:
//					break;
//				default:
//					m_nSequence += 2;
//				}
//			}
//			else if (szModel == "models/weapons/v_knife_survival_bowie.mdl")
//			{
//				// Fix animations for the Bowie Knife.
//				switch (m_nSequence)
//				{
//				case SEQUENCE_DEFAULT_DRAW:
//				case SEQUENCE_DEFAULT_IDLE1:
//					break;
//				case SEQUENCE_DEFAULT_IDLE2:
//					m_nSequence = SEQUENCE_BOWIE_IDLE1;
//					break;
//				default:
//					m_nSequence--;
//				}
//			}
//			else if (szModel == "models/weapons/v_knife_ursus.mdl")
//			{
//				// Fix animations for the Bowie Knife.
//				switch (m_nSequence)
//				{
//				case SEQUENCE_DEFAULT_DRAW:
//					m_nSequence = RandomInt(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
//					break;
//				case SEQUENCE_DEFAULT_LOOKAT01:
//					m_nSequence = RandomInt(SEQUENCE_BUTTERFLY_LOOKAT01, SEQUENCE_BUTTERFLY_LOOKAT03);
//					break;
//				default:
//					m_nSequence++;
//					break;
//				}
//			}
//			else if (szModel == "models/weapons/v_knife_stiletto.mdl")
//			{
//				// Fix animations for the Bowie Knife.
//				switch (m_nSequence)
//				{
//				case SEQUENCE_DEFAULT_LOOKAT01:
//					m_nSequence = RandomInt(12, 13);
//					break;
//				}
//			}
//			else if (szModel == "models/weapons/v_knife_widowmaker.mdl")
//			{
//				// Fix animations for the Bowie Knife.
//				switch (m_nSequence)
//				{
//				case SEQUENCE_DEFAULT_LOOKAT01:
//					m_nSequence = RandomInt(14, 15);
//					break;
//				}
//			}
//			else if (szModel == "models/weapons/v_knife_css.mdl")
//			{
//				// Fix animations for the Bowie Knife.
//				switch (m_nSequence)
//				{
//				case SEQUENCE_CSS1:
//					m_nSequence = RandomInt(13, 14);
//					break;
//				case SEQUENCE_CSS2:
//					m_nSequence = RandomInt(13, 14);
//					break;
//				}
//			}
//			pData->m_Value.m_Int = m_nSequence;
//		}
//	}
//	fnSequenceProxyFn(pData, pStruct, pOut);
//}
//
//void AnimationFixHook()
//{
//	for (ClientClass* pClass = interfaces.client->GetAllClasses(); pClass; pClass = pClass->m_pNext)
//	{
//		if (!pClass)
//			return;
//		if (!strcmp(pClass->m_pNetworkName, "CBaseViewModel")) {
//			// Search for the 'm_nModelIndex' property.
//			RecvTable* pClassTable = pClass->m_pRecvTable;
//
//			for (int nIndex = 0; nIndex < pClassTable->m_nProps; nIndex++) {
//				RecvProp* pProp = &pClassTable->m_pProps[nIndex];
//
//				if (!pProp || strcmp(pProp->m_pVarName, "m_nSequence"))
//					continue;
//
//				fnSequenceProxyFn = (RecvVarProxyFn)pProp->m_ProxyFn;
//				pProp->m_ProxyFn = (RecvVarProxyFn)SetViewModelSequence2;
//				break;
//			}
//
//			break;
//		}
//	}
//}
//
//void NetvarHook()
//{
//	AnimationFixHook();
//
//	ClientClass* pClass = interfaces.client->GetAllClasses();
//	if (!pClass)return;
//
//	while (pClass)
//	{
//		const char* pszName = pClass->m_pRecvTable->m_pNetTableName;
//
//		for (int i = 0; i < pClass->m_pRecvTable->m_nProps; i++)
//		{
//			RecvProp* pProp = &(pClass->m_pRecvTable->m_pProps[i]);
//			const char* name = pProp->m_pVarName;
//
//			if (!strcmp(pszName, "DT_BaseViewModel"))
//			{
//				// Knives
//				if (!strcmp(name, "m_nModelIndex"))
//				{
//					oRecvnModelIndex = (RecvVarProxyFn)pProp->m_ProxyFn;
//					pProp->m_ProxyFn = (RecvVarProxyFn)Hooked_RecvProxy_Viewmodel;
//				}
//			}
//		}
//		pClass = pClass->m_pNext;
//	}
//}