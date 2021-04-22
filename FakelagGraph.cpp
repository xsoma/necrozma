#include "FakelagGraph.h"

FakelagGraph* Graph = new FakelagGraph();

void FakelagGraph::Render()
{
	static const int width = 320, height = 200;
	int w, h;
	interfaces.engine->GetScreenSize(w, h);
	int base_y = h - 150 - height;
	g_Render->FilledRect(0, base_y, width, height, color_t(35, 35, 35, 130));

	if (csgo->choked_history.size() < 15)
	{
		for (int i = 1; i < csgo->choked_history.size(); i++)
		{
			g_Render->DrawLine((i - 1) * (width / 10), base_y + height - (csgo->choked_history[i - 1] * (height / 14)), i * (width / 10), base_y + height - (csgo->choked_history[i] * (height / 14)),color_t(15, 255, 15, 255)
			);
		}
	}
}


