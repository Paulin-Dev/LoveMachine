#pragma once

#include <iostream>
#include <chrono>

const unsigned int QUESTION_ASKED_NB = 5;


class Game {
private:
	unsigned int question_nb = QUESTION_ASKED_NB;
	unsigned int question_asked;
	unsigned int questions[QUESTION_ASKED_NB];

	unsigned int points;
	std::chrono::system_clock::time_point last_question;
public:
	unsigned int newQuestion();
	void startTimer();
	unsigned int getPoints();
	void addPoint();
	bool isOver();
	unsigned int getTimeElapsed();
	unsigned int getTimeLeft();
	bool nextQuestion();
	unsigned int getCurrentQuestion();

	Game();
};