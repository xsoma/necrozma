#pragma once
#define TIME_TO_TICKS(dt) ((int)( 0.5f + (float)(dt) / interfaces.global_vars->interval_per_tick))
#define TICKS_TO_TIME(t) (interfaces.global_vars->interval_per_tick * (t) )

extern Vector old_origin;
extern float old_simtime;
extern Vector new_origin;
extern float new_simtime;

class CMAntiAim : public Singleton<CMAntiAim>
{
private:
	void Yaw(bool& send_packet);
	void Pitch();

public:
	
	bool did_shot = false;
	void Fakelag(bool& send_packet);
	void Sidemove();
	float corrected_tickbase();
	void Run(bool& send_packet);
	bool shouldAA;
	bool shot_dt;
	int cmd_tick;
	int base_tick;
	bool can_exploit(int tickbase_shift);

	float last_real;
	float last_fake;
};
extern bool CanDT();
extern bool CanHD();