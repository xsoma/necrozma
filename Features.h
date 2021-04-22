#pragma once
#include "Visuals.h"
#include "Bunnyhop.h"
#include "Tracer.h"
#include "Eventlog.h"
#include "skinchanger.h"
class features_t
{
public:
	CVisuals* Visuals;
	CBunnyhop* Bunnyhop;
	CBulletTracer* BulletTracer;
	CEventlog* Eventlog;
	CIndic* indicator;
	//c_skins* skinchanger;
};

extern features_t* features;