#include "checkbox.h"
#include "window.h"
#include "child.h"
#include "../Variables.h"
bool c_checkbox::update()
{
	if (should_render)
		if (!should_render())
			return false;
	c_child* c = (c_child*)child;
	if (!c) return false;
	auto wnd = (c_window*)c->get_parent();
	if (!wnd) return false;
	if (wnd->g_active_element != this && wnd->g_active_element != nullptr) return true;
	auto pos = c->get_cursor_position();
	bool h = hovered();

	if (wnd->get_active_tab_index() != this->tab && wnd->get_tabs().size() > 0)
		return false;
	if (h) wnd->g_hovered_element = this;
	if (bind != nullptr) {
		if (binder.active) {
			for (auto i = 0; i < 256; ++i) {
				if (wnd->key_updated(i)) {
					if (i == VK_ESCAPE) {
						bind->key = 0;
						binder.active = false;
						wnd->g_active_element = nullptr;
						return true;
					}
					else {
						bind->key = i;
						binder.active = false;
						wnd->g_active_element = nullptr;
						return true;
					}
				}
			}
			return true;
		}
	}

	// bullshit animations
	if (h) {
		if (animation < 1.f) animation += animation_speed;
	}
	else {
		if (animation > 0.f) animation -= animation_speed;
	}
	animation = clamp(animation, 0.f, 1.f);

	if (h && c->hovered() && wnd->is_click()) {
		*(bool*)value = !(*(bool*)value);
		press_animation = 1.f;
		return true;
	}
	else if (h && c->hovered() && wnd->left_click() && bind != nullptr) {
		binder.open = true;
		wnd->g_active_element = this;
		return true;
	}
	if (bind != nullptr)
	{
		if (binder.open) {
			auto size = Vector2D(g_size, 20);
			pos.y += after_text_offset;
			h = g_mouse.x >= pos.x + 130 && g_mouse.y >= pos.y
				&& g_mouse.x <= pos.x + 200 && g_mouse.y < pos.y + size.y;
			if (wnd->is_click() && h) {
				binder.open = false;
				wnd->g_active_element = nullptr;
				wnd->reset_mouse();
				return true;
			}
			h = g_mouse.x >= pos.x + 130 && g_mouse.y >= pos.y
				&& g_mouse.x <= pos.x + 200 && g_mouse.y < pos.y + size.y * (binder.elements.size() + 1);
			if ((wnd->is_click() || wnd->left_click()) && !h) {
				binder.open = false;
				wnd->g_active_element = nullptr;
				wnd->reset_mouse();
				return true;
			}
			for (size_t i = 0; i < binder.elements.size(); i++) {
				pos.y += 20;
				h = g_mouse.x >= pos.x + 130 && g_mouse.y >= pos.y
					&& g_mouse.x <= pos.x + 200 && g_mouse.y < pos.y + size.y;
				if (h) {
					if (binder.animations[i] < 1.f) binder.animations[i] += animation_speed;
				}
				else
				{
					if (binder.animations[i] > 0.f) binder.animations[i] -= animation_speed;
				}
				if (binder.animations[i] > 1.f) binder.animations[i] = 1.f;
				else if (binder.animations[i] < 0.f) binder.animations[i] = 0.f;
				if (wnd->is_click() && h) {
					bind->type = i;
					binder.open = false;
					wnd->g_active_element = nullptr;
					wnd->reset_mouse();
					return true;
				}
			}
			wnd->g_active_element = this;
		}
		else {
			auto label_size = ImGui::CalcTextSize(label.c_str());
			auto txt_size = ImGui::CalcTextSize(string("[ " + string(KeyStrings[bind->key]) + " ]").c_str());
			auto size = Vector2D(txt_size.x, txt_size.y);
			h = g_mouse.x >= pos.x + 130 && g_mouse.y >= pos.y
				&& g_mouse.x <= pos.x + 200 && g_mouse.y < pos.y + size.y;
			if (h && c->hovered() && wnd->is_click() && bind->type > 0) {
				binder.active = true;
				wnd->g_active_element = this;
				return true;
			}
			if (h) {
				if (binder.animation < 1.f) binder.animation += animation_speed;
			}
			else {
				if (binder.animation > 0.f) binder.animation -= animation_speed;
			}
			if (binder.animation > 1.f) binder.animation = 1.f;
			else if (binder.animation < 0.f) binder.animation = 0.f;
		}
	}
	if (press_animation > 0.f)
		press_animation -= animation_speed;
	press_animation = clamp(press_animation, 0.f, 1.f);
	return true;
}

bool c_checkbox::hovered()
{
	if (should_render)
		if (!should_render())
			return false;
	c_child* c = (c_child*)child;
	if (!c->hovered())
		return false;
	auto pos = c->get_cursor_position();
	auto size = ImGui::CalcTextSize(label.c_str());
	return g_mouse.x > pos.x - 22 && g_mouse.y > pos.y - 2
		&& g_mouse.x < pos.x + 150 && g_mouse.y < pos.y + size.y + 2;
}

void c_checkbox::render() {
	if (should_render)
		if (!should_render())
			return;
	c_child* c = (c_child*)child;
	auto pos = c->get_cursor_position();

	auto wnd = (c_window*)c->get_parent();
	auto size = ImGui::CalcTextSize(label.c_str());
	if (!wnd) return;
	if (wnd->get_active_tab_index() != this->tab 
		&& wnd->get_tabs().size() > 0) return;
	auto alpha = (int)(wnd->get_transparency() * 2.55f);

	g_Render->DrawString(pos.x, pos.y, color_t(200 + 55.f * animation, 200 + 55.f * animation, 200 + 55.f * animation, alpha),render::none, fonts::menu_desc, label.c_str());

	g_Render->FilledRect(pos.x - 20, pos.y + 2, 8, 8, !(*(bool*)value) ? color_t(50, 50, 50, alpha) : color_t(vars.misc.accentcolor.get_red(), vars.misc.accentcolor.get_green(), vars.misc.accentcolor.get_blue(), alpha * (1.f - press_animation)));

	g_Render->Rect(pos.x - 20, pos.y + 2, 8, 8, color_t(0, 0, 0, alpha));

	if (bind != nullptr && bind->type > 0) {
		auto color = bind->key > 0 ? color_t(113, 111, 113, alpha) : color_t(150 + 105.f * binder.animation, 150 + 105.f * binder.animation, 150 + 105.f * binder.animation, alpha);
		std::stringstream text;
		text << "[" << KeyStrings[bind->key].c_str() << "]";
		g_Render->DrawString(pos.x + 150, pos.y, color, render::centered_x, fonts::menu_main, binder.active ? "[-]" : (bind->key > 0 ? text.str().c_str() : "[-]"));
	}
	else
	{
		if (bind != nullptr)
		g_Render->DrawString(pos.x + 150, pos.y, color_t(113, 111, 113, alpha), render::centered_x, fonts::menu_main, "[-]");
	}

	if (bind != nullptr && binder.open) {

		pos.y += after_text_offset;
		auto size = Vector2D(70, 20);
		g_Render->Rect(pos.x + 149, pos.y - 1, 72, (size.y * (binder.elements.size() + 1)) + 2, color_t(0, 0, 0, alpha));
		g_Render->FilledRect(pos.x + 150, pos.y, size.x, size.y,
			color_t(25.f, 25.f, 25.f, alpha));

		for (size_t i = 0; i < binder.elements.size(); i++) {
			pos.y += size.y;
			g_Render->FilledRect(pos.x + 150, pos.y, size.x, size.y, color_t(25.f + 15.f * binder.animations[i], 25.f + 15.f * binder.animations[i], 25.f + 15.f * binder.animations[i], alpha));

			auto clr2 = color_t(200 + binder.animations[i] * 55.f, 200 + binder.animations[i] * 55.f, 200 + binder.animations[i] * 55.f, alpha);
			g_Render->DrawString(pos.x + 160, pos.y, clr2, render::none, fonts::menu_desc, binder.elements[i].c_str());
		}
	}
}

