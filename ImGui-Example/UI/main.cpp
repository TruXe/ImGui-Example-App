#include "gui.h"

#include <thread>

void SleepAsync(int milliseconds) {
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

int main(
	HINSTANCE instance,
	HINSTANCE previousInstance,
	PWSTR arguments,
	int commandShow) 
{
	//Create GUI

	GUI::CreateHWindow("ImGui Example App", "ExampleClass");
	GUI::CreateDevice();
	GUI::CreateImGUI();

	while (GUI::exit)
	{
		GUI::BeginRender();
		GUI::Render();
		GUI::EndRender();

		SleepAsync(10);
	}

	//DESTROY

	GUI::DestroyImGUI();
	GUI::DestroyDevice();
	GUI::DestroyHWindow();

	return EXIT_SUCCESS;
}