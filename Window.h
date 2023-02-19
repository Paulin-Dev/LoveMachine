#pragma once

#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include "Camera.h"
#include "Game.h"
#include "nlohmann/json.hpp"
using json = nlohmann::json;


class Window {
private:
	bool running;

	std::string title;
	Camera camera;
	std::vector<FaceInfo> backup;
	Game game;

	unsigned int marker_index;

	bool game_started;

	bool play_transition_before;
	bool play_transition_after;
	bool play_countdown;
	bool play_answers;
	bool play_results;

	cv::Mat middle_bar;
	
	json answers;

	unsigned int gif_index;

public:	
	void run();
	void wakeUpScreen();
	void startAnalyse(cv::Mat);
	void limitFaces();
	void drawMarker(cv::Mat);
	void playAnimations(cv::Mat);
	bool displayGif(cv::Mat, std::string, std::string);
	bool displayEmoji(cv::Mat);
	void displayQuestion();
	void displayAnswers(cv::Mat);
	void newGame();
	bool sameSide();
	void addPoints();

	Window(std::string, unsigned int);
};
