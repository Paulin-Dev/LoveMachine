#include "Game.h"

const unsigned int TOTAL_QUESTION_NB = 125;
const unsigned int ANSWER_TIME       = 7;     // seconds


unsigned int Game::newQuestion() {
	int question_id;
	do {
		srand(static_cast<unsigned int>(time(NULL)));
		question_id = std::rand() % TOTAL_QUESTION_NB;
	} while (std::find(std::begin(this->questions), std::end(this->questions), question_id) != std::end(this->questions));

	this->questions[this->question_asked] = question_id;
	this->question_asked++;
	return question_id;
}

void Game::startTimer() {
	this->last_question = std::chrono::system_clock::now();
}

unsigned Game::getPoints() {
	return this->points;
}

void Game::addPoint() {
	this->points++;
}

bool Game::isOver() {
	return this->question_asked == this->question_nb;
}

unsigned int Game::getTimeElapsed() {
	return static_cast<unsigned int>((std::chrono::duration<double> (std::chrono::system_clock::now() - this->last_question)).count());
}

unsigned int Game::getTimeLeft() {
	return ANSWER_TIME - this->getTimeElapsed();
}

bool Game::nextQuestion() {
	return this->getTimeElapsed() < 15 && this->getTimeElapsed() >= ANSWER_TIME && !this->ended;
}

void Game::end() {
	this->ended = true;
}

unsigned int Game::getCurrentQuestion() {
	return this->questions[this->question_asked-1];
}

Game::Game() {
	this->question_asked = 0;
	for (unsigned int i = 0; i < this->question_nb; i++) {
		this->questions[i] = TOTAL_QUESTION_NB+1;
	}
	this->points = 0;
	this->ended = false;
}