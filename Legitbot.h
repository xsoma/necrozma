//#include "Hooks.h"
//class Legitbot : public Singleton<Legitbot>
//{
//public:
//	void Cleanup();
//	bool IsRcs();
//	void AutoPistol();
//	float GetFovToPlayer(Vector viewAngle, Vector aimAngle);
//	bool IsLineGoesThroughSmoke(Vector vStartPos, Vector vEndPos);
//	bool IsEnabled(CUserCmd* pCmd);
//	float GetSmooth();
//	void Smooth(Vector currentAngle, Vector aimAngle, Vector& angle);
//	void RCS(Vector curang, Vector& angle, IBasePlayer* target, bool should_run);
//	float GetFov();
//	IBasePlayer* GetClosestPlayer(CUserCmd* cmd, int& bestBone);
//	bool IsNotSilent(float fov);
//	void OnMove(CUserCmd* pCmd);
//private:
//	float shot_delay_time = 0;
//	bool shot_delay = false;
//	bool silent_enabled = false;
//	Vector CurrentPunch = { 0,0,0 };
//	Vector RCSLastPunch = { 0,0,0 };
//	bool is_delayed = false;
//	int kill_delay_time = 0;
//	bool kill_delay = false;
//	IBasePlayer* target = NULL;
//};