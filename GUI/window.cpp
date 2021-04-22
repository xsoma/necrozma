#include "tab.h"
#include "child.h"
#include "../Menu.h"

#define logo_width 0

float animation_speed = 0.f;
void c_window::set_title(string title) {
	this->title = title;
}
void c_window::set_position(Vector2D pos) {
	this->pos = pos;
}
void c_window::set_size(Vector2D size) {
	this->size = size;
}
string c_window::get_title() {
	return this->title;
}
std::vector<c_tab*> c_window::get_tabs() {
	return this->tabs;
}
Vector2D c_window::get_position() {
	return this->pos;
}
Vector2D c_window::get_size() {
	return this->size;
}
void c_window::add_element(c_element* e) {
	this->elements.push_back(e);
}
void c_window::add_tab(c_tab* t) {
	this->tabs.push_back(t);
}
void c_window::set_cursor_position(Vector2D cursor) {
	this->cursor = cursor;
}
Vector2D c_window::get_cursor_position() {
	return this->cursor;
}
void c_window::apply_element(int offset) {
	this->cursor.y += offset;
}
float c_window::get_transparency() {
	transparency = clamp(transparency, 0.f, 100.f);
	return transparency;
}
void c_window::set_transparency(float transparency) {
	transparency = clamp(transparency, 0.f, 100.f);
}
void c_window::increase_transparency(float add) {
	transparency += add;
	transparency = clamp(transparency, 0.f, 100.f);
}
void c_window::decrease_transparency(float subtract)
{
	transparency -= subtract;
	transparency = clamp(transparency, 0.f, 100.f);
}
bool c_window::is_click() {
	return is_pressed;
}
bool c_window::left_click() {
	return is_pressed_left;
}
bool c_window::reset_mouse() {
	is_pressed = false;
	holding = false;
	needs_reset = true;
	return true;
}
void c_window::set_active_tab_index(int tab) {
	this->active_tab_index = tab;
}
int c_window::get_active_tab_index() {
	return this->active_tab_index;
}
bool c_window::is_holding() {
	return holding;
}
bool c_window::get_clicked()
{
	if (hovered()) {
		if (!holding) {
			drag.x = g_mouse.x - pos.x;
			drag.y = g_mouse.y - pos.y;
		}
		return true;
	}
	return false;
}
void c_window::lock_bounds() {
	auto pos = this->get_position();
	auto size = this->get_size();
	auto m = Vector2D(pos.x + size.x, pos.y + size.y);
	ImGui::PushClipRect(ImVec2(pos.x, pos.y), ImVec2(m.x, m.y), true);
}
void c_window::unlock_bounds() {
	ImGui::PopClipRect();
}
bool c_window::hovered() {
	return g_mouse.x > pos.x && g_mouse.y > pos.y
		&& g_mouse.x < pos.x + size.x && g_mouse.y < pos.y + size.y;
}
bool c_window::key_updated(int key)
{
	return (keystates[key] && !oldstates[key]);
}
bool c_window::update() {
	if (ImGui::GetIO().MouseDownDuration[0] >= 0.f && needs_reset)
		return false;
	else if (needs_reset)
		needs_reset = false;

	is_pressed = ImGui::GetIO().MouseDownDuration[0] == 0.f;
	holding = ImGui::GetIO().MouseDownDuration[0] > 0.f;

	is_pressed_left = ImGui::GetIO().MouseDownDuration[1] == 0.f;
	holding_left = ImGui::GetIO().MouseDownDuration[1] > 0.f;
	if (hovered()) {
		if (b_switch) {
			if (pulsating >= 0.f)
				pulsating += animation_speed / 2.f;
			if (pulsating >= 1.f)
				b_switch = false;
		}
		else {
			if (pulsating <= 1.f)
				pulsating -= animation_speed / 2.f;
			if (pulsating <= 0.f)
				b_switch = true;
		}
	}
	else {
		if (pulsating > 0)
			pulsating -= animation_speed / 2.f;
		b_switch = false;
	}
	pulsating = clamp(pulsating, 0.f, 1.f);
	g_hovered_element = nullptr;
	return false;
}
bool c_window::is_holding_menu()
{
	return holding_menu && is_holding();
}
c_element* c_window::get_parent() {
	return nullptr;
}
void c_window::render_tabs() {
	int t_size = tabs.size();
	Vector2D tab_size = Vector2D(100, (size.y - 350.f) / 8);
	Vector2D pos_render = Vector2D(pos.x - 25, pos.y + 20);
	auto alpha = (int)(get_transparency() * 2.55f);
	
	for (auto& t : tabs) {
		t->draw(pos_render, tab_size);
		pos_render.y += tab_size.y;
	}
	pos_render.x -= tab_size.x * tabs.size();
	bool g_hovered = !g_active_element && g_mouse.x > pos.x - 30.f && g_mouse.y > pos.y + 20.f && g_mouse.x < pos.x + 100 && g_mouse.y < pos.y + 55.f;
	if (g_hovered) {
		animation228 += animation_speed;
		if (left_click()) {
			active_tab_index = -1;
			//reset_mouse();
		}
	}
	else
		animation228 -= animation_speed;
	animation228 = clamp(animation228, 0.f, 1.f);	
}
void c_window::update_animation() {
	if (last_time_updated == -1.f)
		last_time_updated = csgo->get_absolute_time();
	animation_speed = fabsf(last_time_updated - csgo->get_absolute_time()) * 5.f;
	last_time_updated = csgo->get_absolute_time();
}
void c_window::update_keystates() {
	std::copy(keystates, keystates + 255, oldstates);
	for (auto i = 0; i < 255; i++)
		keystates[i] = GetAsyncKeyState(i);
}
void c_window::render() {
	if (transparency <= 0.f)
		return;

	int a = clamp((int)(transparency * 2.55f - 50.f + 25.f * pulsating), 0, 255);


	auto alpha = (int)(get_transparency() * 2.55f);
	auto draw = g_Render;
	//draw->FilledRect(0, 0, 4000, 4000, color_t(10, 10, 10, alpha - (alpha / 5)));//background

	//draw->FilledRect(pos.x, pos.y, size.x, size.y, color_t(32, 32, 32, 255));
	//draw->FilledRect(pos.x + 15, pos.y + 15, size.x - 30, 40, color_t(27, 27, 27));
	//draw->Rect(pos.x + 15, pos.y + 15, size.x - 30, 40, color_t(47, 47, 47));
	//draw->filled_rect_gradient(pos.x, pos.y, size.x, 2, color_t(255, 80, 100), color_t(47, 47, 47), color_t(90, 70, 245), color_t(245, 70, 90));

	//draw->FilledRect(pos.x, pos.y, size.x, size.y, color_t(18, 18, 18, alpha));
	//draw->filled_rect_gradient(pos.x + 2, pos.y + 2, size.x / 2 - 2, 2, color_t(67, 133, 180, alpha), color_t(120, 38, 172, alpha), color_t(88, 7, 137, alpha), color_t(21, 84, 130, alpha));
	//draw->filled_rect_gradient(pos.x + size.x / 2, pos.y + 2, size.x / 2 - 2, 2, color_t(120, 38, 172, alpha), color_t(235, 248, 136, alpha), color_t(149, 162, 55, alpha), color_t(88, 7, 137, alpha));//149, 162, 55, alpha   
	//draw->filled_rect_gradient(pos.x + 2, pos.y + 2, size.x - 4, 2, color_t(47, 47, 47, alpha), color_t(47, 47, 47, alpha), color_t(74, 74, 174, alpha), color_t(74, 74, 174, alpha));//149, 162, 55, alpha   

	
	draw->FilledRect(pos.x, pos.y, size.x, size.y, color_t(5, 5, 5, alpha));

	draw->FilledRect(pos.x + 15, pos.y + 15, 100, 458, color_t(18, 18, 18, alpha));
	draw->Rect(pos.x + 15, pos.y + 15, 100, 458, color_t(47, 47, 47, alpha));

	draw->filled_rect_gradient(pos.x + 2, pos.y + 2, size.x / 2 - 2, 2, color_t(65, 165, 194), color_t(180, 80, 165), color_t(160, 50, 146), color_t(23, 134, 166));
	draw->filled_rect_gradient(pos.x + size.x / 2, pos.y + 2, size.x / 2 - 2, 2, color_t(180, 80, 165), color_t(195, 207, 78), color_t(153, 166, 21), color_t(160, 50, 146));

	draw->Rect(pos.x, pos.y, size.x, size.y, color_t(58, 58, 58, alpha));
	draw->Rect(pos.x - 1, pos.y - 1, size.x + 2, size.y + 2, color_t(38, 34, 36, alpha));
	draw->Rect(pos.x - 2, pos.y - 2, size.x + 4, size.y + 4, color_t(38, 34, 36, alpha));
	draw->Rect(pos.x - 3, pos.y - 3, size.x + 6, size.y + 6, color_t(38, 34, 36, alpha));
	draw->Rect(pos.x - 4, pos.y - 4, size.x + 8, size.y + 8, color_t(0, 0, 0, alpha));

	update();

	if (tabs.size() > 0) {
		render_tabs();
		cursor = Vector2D(pos.x + 15, pos.y + 65);
	}
	else
		cursor = Vector2D(pos.x + 15, pos.y + 25);
	
	Vector2D backup_cursor_pos = cursor;
	update_elements();
	cursor = backup_cursor_pos;
	render_elements();

	if (g_active_element) {
		if (g_active_element->type != c_elementtype::child)
			((c_child*)active_element_parent)->set_cursor_position(g_active_element_pos);
		g_active_element->render();
	}

	if (!holding)
		holding_menu = !g_hovered_element && get_clicked();
	
	if (holding_menu)
		this->pos = Vector2D(g_mouse.x - drag.x, g_mouse.y - drag.y);
}

void c_window::update_elements()
{
	for (auto e : elements) {
		if (e->tab != active_tab_index)
			continue;
		e->update();
		if (e->type != c_elementtype::child) {
			int offset = e->get_total_offset();
			if (offset > 0)
				apply_element(offset + 3);
		}
	}
}
void c_window::render_elements()
{
	for (auto e : elements) {
		if (e->tab != active_tab_index)
			continue;
		e->render();
		if (e->type != c_elementtype::child) {
			int offset = e->get_total_offset();
			if (offset > 0)
				apply_element(offset + 3);
		}
	}
};

const vector<string> KeyStrings = {
	"-", "lm", "rm", "control+break", "mm", "m4", "m5",
	"unk", "backspace", "tab", "unk", "unk", "unk", "enter", "unk", "unk", "shift", "ctrl", "alt", "pause",
	"caps lock", "unk", "unk", "unk", "unk", "unk", "unk", "esc", "unk", "unk", "unk", "unk", "space",
	"pg up", "pg down", "end", "home", "left", "up", "right", "down", "unk", "print", "unk", "print screen", "insert",
	"delete", "unk", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "unk", "unk", "unk", "unk", "unk", "unk",
	"unk", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x",
	"y", "z", "left windows", "right windows", "unk", "unk", "unk", "num 0", "num 1", "num 2", "num 3", "num 4", "num 5", "num 6",
	"num 7", "num 8", "num 9", "*", "+", "_", "-", ".", "/", "f1", "f2", "f3", "f4", "f5", "f6", "f7", "f8", "f9", "f10", "f11", "f12",
	"f13", "f14", "f15", "f16", "f17", "f18", "f19", "f20", "f21", "f22", "f23", "f24", "unk", "unk", "unk", "unk", "unk",
	"unk", "unk", "unk", "num lock", "scroll lock", "unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk",
	"unk", "unk", "unk", "unk", "unk", "lshift", "rshift", "lcontrol", "rcontrol", "lmenu", "rmenu", "unk", "unk", "unk",
	"unk", "unk", "unk", "unk", "unk", "unk", "unk", "next track", "previous track", "stop", "play/pause", "unk", "unk",
	"unk", "unk", "unk", "unk", ";", "+", ",", "-", ".", "/?", "~", "unk", "unk", "unk", "unk",
	"unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk",
	"unk", "unk", "unk", "unk", "unk", "unk", "unk", "[{", "\\|", "}]", "'\"", "unk", "unk", "unk", "unk",
	"unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk",
	"unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk","unk", "unk", "unk"
};