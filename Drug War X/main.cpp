#include "main.h"


int main(int argc, char ** argv)
{
	//Start the Strange Game Engine
	SGE::System::Startup();

	//Open Game Window
	SGE::System::OpenGameWindow(640, 480, "Test Title");

	//Initialize Virtual Display
	SGE::System::InitializeVirtualDisplay(320, 240);


	//Launch the actual logic threads, before handling events.





	//Start drawing
	SGE::System::StartDrawing();

	//Handle some events
	//Event handling has to be run from the main thread, hence why we are spawning the game logic off on it's own thread
	//And we are running the event handler on the main thread
	//Same OSes are weird about non-main threads handling keyboard/mouse input
	SGE::System::HandleEvents();

	//Interface handling over, window is closed stop the drawing loop if it already hasn't
	SGE::System::StopDrawing();

	//Shutdown the Strange Game Engine
	SGE::System::Shutdown();


	return 0;
}