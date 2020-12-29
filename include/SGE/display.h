#pragma once
#include "api.h"
#include <vector>
//
//  Strange Game Engine Main Namespace
//
namespace SGE::Display
{
    //
    //  Callbacks namespace for callbacks related to Display
    //
    namespace Callbacks
    {
        //
        //  Function to register callbacks
        //
        void Register();
    }

    //
    //  Video namespace contain elements for virtual display used direct by the game
    //
    namespace Video
    {
        //
        //  Pixel data format
        //
        typedef unsigned int pixel;


        //
        //  Maximum Limits for the  System
        //

        // Max Resolution Width
        const int MAX_RESOLUTION_WIDTH = 1920;

        // Max Resolution Height
        const int MAX_RESOLUTION_HEIGHT = 1080;

        // Max Video RAM size, hence the size initialized.
        const int MAX_VIDEO_RAM = MAX_RESOLUTION_WIDTH * MAX_RESOLUTION_HEIGHT;


        //
        //  The virtual video RAM.  Public accessible to allow other components to write to it directly.
        //
        extern pixel RAM[MAX_VIDEO_RAM];

        //
        //  The virtual video horizontal resolution
        //
        extern int X;

        //
        //  The virtual video vertical resolution
        //
        extern int Y;
    }

    //
    //  Flag to indicate the framebuffer window size has changed
    //
    extern bool FrameBufferChanged;

    //
    //  Flag to indicate the game resolution has changed
    //
    extern bool GameResolutionChanged;

    //
    //  Count of frames rendered
    //
    extern unsigned int FrameCount;

    //
    //  Time taken to render the frame in milliseconds
    //
    extern int UploadTime;
    extern int RenderTime;
    extern int UpdateTime;

    //
    //  Video System Initialization
    //
    void Init(int windowX, int windowY, const char* gameTitle);

    //
    //  Block display refresh.  Useful for preventing screen tearing from draw operations happening in the middle of a display refresh
    //
    void BlockRefresh();

    //
    //  Allow the virtual display fresh to allow drawing to continue.
    //
    void AllowRefresh();

    //
    //  Start the Display
    //
    void Open(int newVideoX, int newVideoY);

    //
    //  Stop the Display
    //
    void Close();

    //
    //  Starts the thread to grab data from the virtual video RAM and dumping it to the virtual display
    //
    void StartDrawing();

    //
    //  Stops that thread, and waits for it to join back up.
    //
    void StopDrawing();

    //
    //  Set framebuffer window size
    //
    void SetWindowSize(int width, int height);

    //
    //  Change game resolution
    //
    void ChangeGameResolution(int width, int height);

    //
    //  Hide the mouse cursor from the OS
    //
    void HideOSMouseCursor();

    //
    //  Show the mouse cursor from the OS
    //
    void ShowOSMouseCursor();
}
