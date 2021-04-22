#pragma once
#include "Hooks.h"
#include "AntiAims.h"
int showtime = 5.f;
string prefix = "[gamesense] ";


class CMessage
{
public:
	CMessage(string str, float time, color_t clr)
	{
		this->str = str;
		this->time = csgo->get_absolute_time();
		this->clr = clr;
		alpha = 255;
	}
	std::string str;
	float time;
	color_t clr;
	int alpha;
};
class CIndic
{
public:
	
	void Draw()
	{
		if (!interfaces.engine->IsConnected() || !interfaces.engine->IsInGame()) {
			return;
		}
	}
};
class CEventlog
{
public:
	std::vector <CMessage> messages;
	void Draw()
	{
		if (!interfaces.engine->IsConnected() || !interfaces.engine->IsInGame()) {
			messages.clear();
			return;
		}

		if (!vars.visuals.eventlog)
			return;

		//interfaces.cvars->FindVar("developer")->SetValue(1);

		while (messages.size() > 10)
			messages.erase(messages.begin());
		for (int i = messages.size() - 1; i >= 0; i--)
		{
			if (messages[i].time + showtime <= csgo->get_absolute_time())
				messages[i].alpha -= 3;

			//g_Render->DrawString(10, 10 + (15 * i) - (255 - messages[i].alpha), color_t(255, 255, 255, messages[i].alpha), 0, fonts::event_font, messages[i].str.c_str());

			Drawing::DrawString(fonts::event_font, 10, 10 + (15 * i) - (255 - messages[i].alpha), color_t(255, 255, 255, messages[i].alpha), 0, messages[i].str.c_str());

			if (messages[i].alpha <= 0)
				messages.erase(messages.begin() + i);
		}

	}
};