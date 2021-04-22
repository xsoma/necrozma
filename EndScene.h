#include "Hooks.h"
#include "Menu.h"
#include "imgui\imgui_impl_dx9.h"
#include "imgui\imgui_impl_win32.h"
#include "imgui\imgui_freetype.h"
#include <d3dx9tex.h>
#include "AntiAims.h"
#include "scripting/java loader/js_loader.h"
#include <chrono>
#include "FakelagGraph.h"
//#include "other.h"

void GUI_Init(IDirect3DDevice9* pDevice)
{
	if (!vars.menu.guiinited)
	{
		ImGui::CreateContext();

		ImGui_ImplWin32_Init(csgo->Init.Window);
		ImGui_ImplDX9_Init(pDevice);

		ImGuiIO& io = ImGui::GetIO();

		ImFontConfig cfg;
		fonts::menu_main = io.Fonts->AddFontFromFileTTF("C:/windows/fonts/verdana.ttf", 13, &cfg, io.Fonts->GetGlyphRangesCyrillic());
		fonts::menu_tab = io.Fonts->AddFontFromFileTTF("C:/windows/fonts/verdana.ttf", 13, &cfg, io.Fonts->GetGlyphRangesCyrillic());
		fonts::menu_desc = io.Fonts->AddFontFromFileTTF("C:/windows/fonts/verdana.ttf", 13, &cfg, io.Fonts->GetGlyphRangesCyrillic());
		fonts::watermark = io.Fonts->AddFontFromFileTTF("C:/windows/fonts/verdana.ttf", 12, &cfg, io.Fonts->GetGlyphRangesCyrillic());
		fonts::lby_indicator = io.Fonts->AddFontFromFileTTF("C:/windows/fonts/calibrib.ttf", 24, &cfg, io.Fonts->GetGlyphRangesCyrillic());
		//fonts::weapon_font = io.Fonts->AddFontFromMemoryTTF(weaponicons, sizeof(weaponicons), 14, &cfg, io.Fonts->GetGlyphRangesCyrillic());
		fonts::verdana12 = io.Fonts->AddFontFromFileTTF("C:/windows/fonts/verdana.ttf", 11, &cfg, io.Fonts->GetGlyphRangesCyrillic());
		fonts::verdana13 = io.Fonts->AddFontFromFileTTF("C:/windows/fonts/verdana.ttf", 13, &cfg, io.Fonts->GetGlyphRangesCyrillic());
		//fonts::event_font = io.Fonts->AddFontFromFileTTF("C:/windows/fonts/lucon.ttf", 10, &cfg, io.Fonts->GetGlyphRangesCyrillic());
		//espfont = io.Fonts->AddFontFromMemoryTTF(VerdanaProLight, sizeof(VerdanaProLight), 15.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
		ImGuiFreeType::BuildFontAtlas(io.Fonts, 0x00);


		vars.menu.guiinited = true;
	}
}

LPDIRECT3DTEXTURE9 img = nullptr;
LPD3DXSPRITE sprite = nullptr;

static HRESULT __stdcall Hooked_Present(IDirect3DDevice9Ex* device, const RECT* src, const RECT* dest, HWND windowOverride, const RGNDATA* dirtyRegion) noexcept
{
	if (csgo->DoUnload)
		return H::SteamPresent(device, src, dest, windowOverride, dirtyRegion);

	GUI_Init(device);
	csgo->render_device = device;
	g_Render->SetupPresent(device);
	static void* dwReturnAddress = _ReturnAddress();

	IDirect3DVertexDeclaration9* decl = nullptr;
	IDirect3DVertexShader9* shader = nullptr;
	IDirect3DStateBlock9* block = nullptr;

	device->GetVertexDeclaration(&decl);
	device->GetVertexShader(&shader);
	device->CreateStateBlock(D3DSBT_PIXELSTATE, &block);

	DWORD colorwrite, srgbwrite;
	device->GetRenderState(D3DRS_COLORWRITEENABLE, &colorwrite);
	device->GetRenderState(D3DRS_SRGBWRITEENABLE, &srgbwrite);

	device->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
	//removes the source engine color correction
	device->SetRenderState(D3DRS_SRGBWRITEENABLE, false);

	device->SetSamplerState(NULL, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	device->SetSamplerState(NULL, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	device->SetSamplerState(NULL, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
	device->SetSamplerState(NULL, D3DSAMP_SRGBTEXTURE, NULL);

	if (dwReturnAddress == _ReturnAddress())
	{
		g_Render->PreRender(device);

		features->Visuals->DrawLocalShit(device);
		g_Hitmarker->Paint();
		g_Render->PostRender(device);

		g_Menu->draw_indicators();

		c_js_loader::execute_scripts(paint_traverse_hk);

		g_Menu->render();

		g_Render->EndPresent(device);
	}
	block->Apply();
	block->Release();
	device->SetVertexShader(shader);
	device->SetVertexDeclaration(decl);
	device->SetRenderState(D3DRS_COLORWRITEENABLE, colorwrite);
	device->SetRenderState(D3DRS_SRGBWRITEENABLE, srgbwrite);
	return H::SteamPresent(device, src, dest, windowOverride, dirtyRegion);//return present here

}

static HRESULT D3DAPI Hooked_Reset(IDirect3DDevice9Ex* device, D3DPRESENT_PARAMETERS* params) noexcept
{
	if (csgo->DoUnload)
		return H::SteamReset(device, params);

	g_Render->InvalidateObjects();

	auto hr = H::SteamReset(device, params);
	if (hr >= 0) {
		g_Render->CreateObjects(device);
	}
	return hr;

}