
/*
REMOVE THE CONSOLE (VS) :
 
Change Linker > System > SubSystem > Window
Change Linked > Advanced > Entry Point > mainCRTStartup
*/


#include <iostream>
#include "Window.h"
#include <opencv2/highgui.hpp>


const unsigned int CAMERA_ID   = 0;
const std::string WINDOW_TITLE = "Love Machine";

int main(int argc, char** argv) {
	Window window(WINDOW_TITLE, CAMERA_ID);
	window.run();
	return 0;
}