#include "EnginePrediction.h"
#include "checksum_md5.h"
#include "AntiAims.h"

void CEnginePrediction::Start(CUserCmd* cmd, IBasePlayer* local) {
	old_vars.curtime = interfaces.global_vars->curtime;
	old_vars.frametime = interfaces.global_vars->frametime;
	old_vars.tickcount = interfaces.global_vars->tickcount;
	auto old_in_prediction = interfaces.prediction->bInPrediction;
	auto old_first_prediction = interfaces.prediction->bIsFirstTimePredicted;

	interfaces.global_vars->curtime = TICKS_TO_TIME(local->GetTickBase());
	interfaces.global_vars->frametime = interfaces.global_vars->interval_per_tick;
	interfaces.global_vars->tickcount = TIME_TO_TICKS(interfaces.global_vars->curtime);


	//*reinterpret_cast<CUserCmd**>(uint32_t(local) + 0x3334) = cmd;
	//*reinterpret_cast<CUserCmd**>(uint32_t(local) + 0x3288) = cmd;

	interfaces.prediction->bIsFirstTimePredicted = false;
	interfaces.prediction->bInPrediction = true;

	if (cmd->impulse)
		*reinterpret_cast<uint32_t*>(uint32_t(local) + 0x31FC) = cmd->impulse;

	cmd->buttons |= *reinterpret_cast<uint32_t*>(uint32_t(local) + 0x3330);

	auto v16 = cmd->buttons;
	auto* unk02 = reinterpret_cast<int*>(uint32_t(local) + 0x31F8);
	auto v17 = v16 ^ *unk02;

	*reinterpret_cast<int*>(uint32_t(local) + 0x31EC) = *unk02;
	*reinterpret_cast<int*>(uint32_t(local) + 0x31F8) = v16;
	*reinterpret_cast<int*>(uint32_t(local) + 0x31F0) = v16 & v17;
	*reinterpret_cast<int*>(uint32_t(local) + 0x31F4) = v17 & ~v16;

	interfaces.prediction->CheckMovingGround(local, interfaces.global_vars->frametime);

	interfaces.move_helper->SetHost(local);

	interfaces.prediction->SetupMove(local, cmd, interfaces.move_helper, &data);

	interfaces.game_movement->ProcessMovement(local, (CMoveData*)&data);

	interfaces.prediction->FinishMove(local, cmd, &data);

	interfaces.move_helper->ProcessImpacts();
}

void CEnginePrediction::Finish(IBasePlayer* local) {
	//interfaces.game_movement->FinishTrackPredictionErrors(local);
	//interfaces.move_helper->SetHost(nullptr);

	//interfaces.global_vars->curtime = old_vars.curtime;
	//interfaces.global_vars->frametime = old_vars.frametime;


	interfaces.game_movement->FinishTrackPredictionErrors(local);
	interfaces.move_helper->SetHost(nullptr);

	interfaces.global_vars->curtime = old_vars.curtime;
	interfaces.global_vars->frametime = old_vars.frametime;
	interfaces.global_vars->tickcount = old_vars.tickcount;

	//	CPrediction::FinishCommand
	/*{
		*reinterpret_cast<int*>(m_prediction_seed) = static_cast <uint32_t>(-1);
		*reinterpret_cast<int*>(m_prediction_player) = 0;

		*reinterpret_cast<uint32_t*>(reinterpret_cast <uint32_t>(local) + 0x3314) = 0;
	}*/

	interfaces.game_movement->Reset();
}