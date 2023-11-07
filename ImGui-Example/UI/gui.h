#pragma once
#include <d3d9.h>

namespace GUI
{
	constexpr int WIDTH{ 500 };
	constexpr int HEIGHT{ 300 };

	inline bool exit = true;

	//WinAPI window vars

	inline HWND window = nullptr;
	inline WNDCLASSEXA windowClass = { };

	inline HWND console = GetConsoleWindow();

	// Points for window movement
	inline POINTS position = {  };

	// Direct x State Vars
	inline PDIRECT3D9 d3d = nullptr;
	inline LPDIRECT3DDEVICE9 device = nullptr;
	inline D3DPRESENT_PARAMETERS presentParameters = {  };

	//Handle Window
	void CreateHWindow(
		const char* windowName,
		const char* className) noexcept;

	void DestroyHWindow() noexcept;

	//Handle device

	bool CreateDevice() noexcept;
	void ResetDevice() noexcept;
	void DestroyDevice() noexcept;

	//handle ImGUI create & destroy
	void CreateImGUI() noexcept;
	void DestroyImGUI() noexcept;

	void BeginRender() noexcept;
	void EndRender() noexcept;
	void Render() noexcept;
}

