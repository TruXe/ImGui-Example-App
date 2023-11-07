#include "gui.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_dx9.h"
#include "../imgui/imgui_impl_win32.h"


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND window, UINT message, WPARAM wideParameter, LPARAM longParameter);

long __stdcall WindowProcess(HWND window, UINT message, WPARAM wideParameter, LPARAM longParameter) {

	if (ImGui_ImplWin32_WndProcHandler(window, message, wideParameter, longParameter)) {

		return true;

	}

	switch (message)
	{
	case WM_SIZE: {
		if (GUI::device && wideParameter != SIZE_MINIMIZED) {
			GUI::presentParameters.BackBufferWidth = LOWORD(longParameter);
			GUI::presentParameters.BackBufferHeight = HIWORD(longParameter);
			GUI::ResetDevice();
		}
	}return 0;

	case WM_SYSCOMMAND: {

		if ((wideParameter & 0xfff0) == SC_KEYMENU) //Disable ALT App menu
		{
			return 0;
		}
	}break;

	case WM_DESTROY: {
		PostQuitMessage(0);
	}return 0;

	case WM_LBUTTONDOWN: {
		GUI::position = MAKEPOINTS(longParameter); // Click
	}return 0;

	case WM_MOUSEMOVE: {

		if (wideParameter == MK_LBUTTON)
		{
			const auto points = MAKEPOINTS(longParameter);
			auto rect = ::RECT{ };

			GetWindowRect(GUI::window, &rect);

			rect.left += points.x - GUI::position.x;
			rect.top += points.y - GUI::position.y;

			if (GUI::position.x >= 0 && GUI::position.x <= GUI::WIDTH && GUI::position.y >= 0 && GUI::position.y <= 19) {
				SetWindowPos(GUI::window, HWND_TOPMOST, rect.left, rect.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER);
			}
		}

	}return 0;


	}

	return DefWindowProcW(window, message, wideParameter, longParameter);

}

void GUI::CreateHWindow(
	const char* windowName,
	const char* className) noexcept {

	windowClass.cbSize = sizeof(WNDCLASSEXA);
	windowClass.style = CS_CLASSDC;
	windowClass.lpfnWndProc = WindowProcess;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = GetModuleHandleA(0);
	windowClass.hIcon = 0;
	windowClass.hCursor = 0;
	windowClass.hbrBackground = 0;
	windowClass.lpszClassName = className;
	windowClass.hIconSm = 0;

	RegisterClassExA(&windowClass);

	static int X_POS{ 100 }; //Start position ( Width of screen )
	static int Y_POS{ 100 }; //Start position ( Height of screen )
	window = CreateWindowA(className, windowName, WS_POPUP, X_POS, Y_POS, WIDTH, HEIGHT, 0, 0, windowClass.hInstance, 0);

	ShowWindow(window, SW_SHOWDEFAULT);
	UpdateWindow(window);

}

void GUI::DestroyHWindow() noexcept {

	DestroyWindow(window);
	UnregisterClassA(windowClass.lpszClassName, windowClass.hInstance);

}

//Handle Device

bool GUI::CreateDevice() noexcept {

	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	if (!d3d)
	{
		return false;
	}

	ZeroMemory(&presentParameters, sizeof(presentParameters));

	presentParameters.Windowed = TRUE;
	presentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	presentParameters.BackBufferFormat = D3DFMT_UNKNOWN;
	presentParameters.EnableAutoDepthStencil = TRUE;
	presentParameters.AutoDepthStencilFormat = D3DFMT_D16;
	presentParameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	if (d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window, D3DCREATE_HARDWARE_VERTEXPROCESSING, &presentParameters, &device) < 0)
	{
		return false;
	}
	return true;

}

void GUI::ResetDevice() noexcept {

	ImGui_ImplDX9_InvalidateDeviceObjects();

	const auto result = device->Reset(&presentParameters);

	if (result == D3DERR_INVALIDCALL)
	{
		IM_ASSERT(0);
	}
	ImGui_ImplDX9_CreateDeviceObjects();

}

void GUI::DestroyDevice() noexcept {

	if (device)
	{
		device->Release();
		device = nullptr;
	}

	if (d3d)
	{
		d3d->Release();
		d3d = nullptr;
	}

}

//Handle ImGUI Create & Destroy

void GUI::CreateImGUI() noexcept {

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ::ImGui::GetIO();

	io.IniFilename = NULL;

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX9_Init(device);

}

void GUI::DestroyImGUI() noexcept {

	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

}

void GUI::BeginRender() noexcept {
	MSG message;
	while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void GUI::EndRender() noexcept {
	ImGui::EndFrame();

	device->SetRenderState(D3DRS_ZENABLE, FALSE);
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

	device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);

	if (device->BeginScene() >= 0)
	{
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		device->EndScene();
	}

	const auto result = device->Present(0, 0, 0, 0);

	if (result == D3DERR_DEVICELOST && device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
	{
		ResetDevice();
	}
}

//MENU HERE

void GUI::Render() noexcept {
	ImGui::SetNextWindowPos({ 0,0 });
	ImGui::SetNextWindowSize({ WIDTH, HEIGHT });
	ImGui::Begin("ImGui Example App", &exit, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);
	{
		ImGui::Button("Example Button");
		ImGui::Separator();
		ImGui::Text("This is sample text.");
	}
	ImGui::End();
}