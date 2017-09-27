#include "main.h"


int main(int argc, char ** argv)
{
	//Start the Strange Game Engine
	SGE::Startup(640, 480, "Test Title");

	//Initialize Virtual Display
	SGE::Display::Open(320, 240);


	//Launch the actual logic threads, before handling events.





	//Start drawing
	SGE::Display::StartDrawing();

	//Handle some events
	//Event handling has to be run from the main thread, hence why we are spawning the game logic off on it's own thread
	//And we are running the event handler on the main thread
	//Same OSes are weird about non-main threads handling keyboard/mouse input
	SGE::Controls::HandleEvents();

	//Interface handling over, window is closed stop the drawing loop if it already hasn't
	SGE::Display::StopDrawing();

	//Shutdown the Strange Game Engine
	SGE::Shutdown();


	return 0;
}