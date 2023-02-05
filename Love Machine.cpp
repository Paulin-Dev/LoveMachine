
/*
THE CONSOLE :
 
Change Linker > System > SubSystem > Window
Change Linked > Advanced > Entry Point > mainCRTStartup
*/


#include <iostream>
#include "Window.h"
#include <opencv2/highgui.hpp>

//#pragma comment(lib, "winmm.lib")

const unsigned int CAMERA_ID   = 1;
const std::string WINDOW_TITLE = "Love Machine";

const std::string SONG = "C:/Users/Paulin/Documents/IA/Love Machine/songs/Cool-Chill-Lofi-Background-Music-For-Videos.wav";


int main(int argc, char** argv) {

	/*
	std::wstring stemp = std::wstring(SONG.begin(), SONG.end());
	LPCWSTR sw = stemp.c_str();

	PlaySound(sw, NULL, SND_FILENAME | SND_ASYNC);
	*/


	Window window(WINDOW_TITLE, CAMERA_ID);
	window.run();
	return 0;

}