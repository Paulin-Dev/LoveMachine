#define _CRT_SECURE_NO_WARNINGS
#include "Window.h"


const std::string MODEL_PATH                 = "C:/Users/Paulin/Documents/IA/Love Machine/models/centerface.onnx";

const std::string MARKER_DIR_PATH            = "C:/Users/Paulin/Documents/IA/Love Machine/assets/marker2/";
const std::string TRANSITION_BEFORE_DIR_PATH = "C:/Users/Paulin/Documents/IA/Love Machine/assets/transition_before/";
const std::string TRANSITION_AFTER_DIR_PATH  = "C:/Users/Paulin/Documents/IA/Love Machine/assets/transition_after/";
const std::string QUESTIONS_DIR_PATH         = "C:/Users/Paulin/Documents/IA/Love Machine/assets/questions/";
const std::string COUNTDOWN_3_DIR_PATH       = "C:/Users/Paulin/Documents/IA/Love Machine/assets/countdown_3/";
const std::string SAVES_DIR_PATH             = "C:/Users/Paulin/Documents/IA/Love Machine/assets/saves/";
const char* SORT_SCRIPT                      = "python C:/Users/Paulin/Documents/IA/\"Love Machine\"/assets/saves/sort.py";
const std::string MIDDLE_BAR_PATH            = "C:/Users/Paulin/Documents/IA/Love Machine/assets/middle_bar.png";
const std::string RESULTS_DIR_PATH           = "C:/Users/Paulin/Documents/IA/Love Machine/assets/results/";
const std::string ANSWERS_JSON_PATH          = "C:/Users/Paulin/Documents/IA/Love Machine/answers.json";

const cv::Vec3b MAIN_COLOR					 = cv::Vec3b(99, 35, 251);
const cv::Vec3b TOP_BAR_COLOR                = cv::Vec3b(214, 200, 251);
const cv::Vec3b MIDDLE_BAR_COLOR             = cv::Vec3b(140, 136, 203);
const cv::Vec3b TEXT_COLOR                   = cv::Vec3b(140, 136, 203);
const cv::Vec3b VIDEO_BACKGROUND_COLOR       = cv::Vec3b(214, 200, 251);
const cv::Vec3b VIDEO_TEXT_COLOR             = cv::Vec3b(140, 136, 203);
const cv::Vec3b TRANSITION_COLOR             = cv::Vec3b(214, 200, 251);
const cv::Vec3b COUNTDOWN_COLOR              = cv::Vec3b(214, 200, 251);
const cv::Vec3b MARKER_COLOR                 = cv::Vec3b(140, 136, 203);

// https://colorpalettes.net/color-palette-1902/


void Window::run() {
	cv::namedWindow(this->title, cv::WND_PROP_FULLSCREEN);
	cv::setWindowProperty(this->title, cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);

	std::thread wth(&Window::wakeUpScreen, this);
	wth.detach();

	cv::Mat frame;
	this->running = true;

	while (this->running) {

		this->camera.read(frame);

		switch (cv::waitKey(1))
		{
		case 27:  // escape
			this->running = false;
			break;
		case 32: // space
			if (!this->game_started) {
				std::thread gth(&Window::newGame, this);
				gth.detach();
			}
			break;
		default:
			break;
		}

		if (!frame.empty()) {
			if (!this->camera.analysing) {
			this->camera.analysing = true;
			std::thread th(&Window::startAnalyse, this, frame);
			th.detach();
		}

			this->limitFaces();
			this->drawMarker(frame);
			this->playAnimations(frame);

			if (this->save_frame) {
				this->saved_frame = frame.clone();
				this->save_frame = false;
			}

			cv::imshow(this->title, frame);
		}
	}

	this->camera.release();
	cv::destroyAllWindows();
}

void Window::wakeUpScreen() {
	while (this->running) {
		SetThreadExecutionState(ES_DISPLAY_REQUIRED);
		Sleep(30000);
	}
}

void Window::startAnalyse(cv::Mat frame) {
	this->camera.analyse(frame, this->backup);
}

void Window::limitFaces() {
	if (this->backup.size() > 2) {
		int height;
		int first = 0;
		int first_index = 0;
		int second = 0;
		int second_index = 0;

		for (int i = 0; i < this->backup.size(); i++) {
			height = this->backup[i].y2 - this->backup[i].y1;
			if (height > first) {
				second = first;
				second_index = first_index;
				first = height;
				first_index = i;
			}
			else if (height > second) {
				second = height;
				second_index = i;
			}
		}
		for (int i = 0; i < this->backup.size(); i++) {
			if (i != first_index && i != second_index) {
				this->backup.erase(this->backup.begin() + i);
			}
		}
	}
}

void Window::drawMarker(cv::Mat image) {
	// cols = width, rows = height
	cv::Mat marker;

	for (int i = 0; i < backup.size(); i++) {	
		marker = cv::imread(MARKER_DIR_PATH + std::to_string(this->marker_index) + ".png");
		if (marker.empty()) {
			this->marker_index = 0;
			marker = cv::imread(MARKER_DIR_PATH + std::to_string(this->marker_index) + ".png");
		}

		cv::resize(marker, marker, cv::Size(static_cast<int>((((this->backup[i].y2 - this->backup[i].y1) / 3) * marker.cols) / marker.rows), static_cast<int>((this->backup[i].y2 - this->backup[i].y1) / 3)));

		int x = static_cast<int>(((this->backup[i].x1 + this->backup[i].x2) / 2) - (marker.cols / 2.0));
		int y = static_cast<int>(this->backup[i].y1 - ((this->backup[i].y2 - this->backup[i].y1) / 2));

		for (int col = 0; col < marker.cols; col++) {
			for (int row = 0; row < marker.rows; row++) {
				if ((y + row) >= 0) {
					cv::Vec3b pixel = marker.at<cv::Vec3b>(cv::Point(col, row));
					if (pixel != cv::Vec3b(211, 235, 251)) {
						image.at<cv::Vec3b>(cv::Point(col + x, row + y)) = MARKER_COLOR;
					}
				}
			}
		}
	}
	if (this->backup.size() > 0) {
		this->marker_index++;
	}
}

void Window::playAnimations(cv::Mat frame) {

	if (this->play_answers) {
		this->displayAnswers(frame);
	}

	if (this->play_transition_before) {
		if (this->displayGif(frame, TRANSITION_BEFORE_DIR_PATH, "transition")) {
			this->play_transition_before = false;
			if (!this->play_results) {
				this->displayQuestion();
			}
		}
	}

	if (this->play_transition_after && !this->play_transition_before) {
		if (this->displayGif(frame, TRANSITION_AFTER_DIR_PATH, "transition")) {
			this->play_transition_after = false;
		}
		return;
	}

	if (this->play_countdown) {
		if (this->displayGif(frame, COUNTDOWN_3_DIR_PATH, "countdown")) {
			this->play_countdown = false;
		}
		return;
	}

	if (this->play_results && !this->play_transition_after) {
		if (this->displayEmoji(frame)) {
			this->play_results = false;
			this->game_started = false;
			system(SORT_SCRIPT);
			this->game.end();
		}
		return;
	}

	if (this->game_started && !this->play_transition_before && !this->play_transition_after && !this->play_countdown && !this->play_results) {

		if (this->game.nextQuestion()) {
			std::cout << "YES " << this->game.nextQuestion() << std::endl;
			this->addPoints();

			if (this->game.isOver()) {
				this->play_answers = false;
				this->play_transition_before = true;
				this->play_transition_after = true;
				this->play_results = true;
				this->takePicture(5.5);
			}
			else {
				this->play_transition_before = true;
			}
		}
	}
}

bool Window::displayGif(cv::Mat frame, std::string gif_directory, std::string type) {
	cv::Mat gif_frame = cv::imread(gif_directory + std::to_string(this->gif_index) + ".png");
	if (gif_frame.empty()) {
		this->gif_index = 0;
		return true;
	}
	cv::Vec3b color;

	if (type == "transition") {
		cv::resize(gif_frame, gif_frame, cv::Size(frame.cols, frame.rows));
		color = TRANSITION_COLOR;
	}
	else if (type == "countdown") {
		cv::resize(gif_frame, gif_frame, cv::Size((frame.rows * gif_frame.cols) / gif_frame.rows, frame.rows));
		color = COUNTDOWN_COLOR;
		Sleep(10);
	}
	int x = (frame.cols / 2) - (gif_frame.cols / 2);
	int y = (frame.rows / 2) - (gif_frame.rows / 2);
	for (int col = 0; col < gif_frame.cols; col++) {
		for (int row = 0; row < gif_frame.rows; row++) {
			cv::Vec3b pixel = gif_frame.at<cv::Vec3b>(cv::Point(col, row));
			if (pixel != cv::Vec3b(255, 255, 255)) {
				frame.at<cv::Vec3b>(cv::Point(col + x, row + y)) = color;
			}
		}
	}
	this->gif_index++;
	return false;
}

bool Window::displayEmoji(cv::Mat frame) {
	cv::Mat gif_frame = cv::imread(RESULTS_DIR_PATH + std::to_string(this->game.getPoints()) + "/" + std::to_string(this->gif_index) + ".png");
	if (gif_frame.empty()) {
		this->gif_index = 0;
		return true;
	}

	for (int i = 0; i < this->backup.size(); i++) {

		int size = (backup[i].x2-this->backup[i].x1)*1.5;
		cv::resize(gif_frame, gif_frame, cv::Size(size, size));
		
		int x = ((this->backup[i].x1 + this->backup[i].x2) / 2) - gif_frame.cols/2;
		int y = ((this->backup[i].y1 + this->backup[i].y2) / 2) - gif_frame.rows/2.3;

		for (int col = 0; col < gif_frame.cols; col++) {
			for (int row = 0; row < gif_frame.rows; row++) {
				if ((row + y) >= 0) {
					cv::Vec3b pixel = gif_frame.at<cv::Vec3b>(cv::Point(col, row));
					if (pixel != cv::Vec3b(144, 120, 254) && pixel != gif_frame.at<cv::Vec3b>(cv::Point(0, 0)) && (200 < pixel[0] || pixel[0] < 110)) {
						frame.at<cv::Vec3b>(cv::Point(col + x, row + y)) = pixel;
					}
				}
			}
		}
	}

	this->gif_index++;
	Sleep(10);
	return false;
}

void Window::displayQuestion() {

	int question_id = this->game.newQuestion();
	question_id = 0;

	cv::VideoCapture cap(QUESTIONS_DIR_PATH + std::to_string(question_id) + ".mp4");
	if (cap.isOpened()) {
		cv::Mat frame;
		while (true) {
			cap.read(frame);
			if (frame.empty()) {
				break;
			}
			for (int col = 0; col < frame.cols; col++) {
				for (int row = 0; row < frame.rows; row++) {
					cv::Vec3b pixel = frame.at<cv::Vec3b>(cv::Point(col, row));
					if (pixel[0] > 20) {
						frame.at<cv::Vec3b>(cv::Point(col, row)) = VIDEO_BACKGROUND_COLOR;
					}
					else {
						frame.at<cv::Vec3b>(cv::Point(col, row)) = VIDEO_TEXT_COLOR;
					}
				}
			}
			cv::imshow(this->title, frame);
			cv::waitKey(15);
		}
		cap.release();
	}
	this->game.startTimer();
	this->play_transition_after = true;
	this->play_answers = true;

	this->takePicture(3.5);

	this->middle_bar = cv::imread(MIDDLE_BAR_PATH);
	cv::resize(this->middle_bar, this->middle_bar, cv::Size((this->middle_bar.cols * this->camera.getHeight() - (this->camera.getHeight() - this->camera.getHeight() / 8)) / this->middle_bar.rows, this->camera.getHeight() - this->camera.getHeight() / 8));
}

void Window::takePicture(double seconds) {
	std::thread sth(&Window::saveFrame, this, seconds);
	sth.detach();
}

void Window::saveFrame(double seconds) {
	Sleep(seconds*1000);
	time_t t;
	time(&t);
	struct tm* tmp;
	tmp = localtime(&t);
	char output[20];
	strftime(output, sizeof(output), "%d-%m-%Y %H-%M-%S", tmp);
	this->save_frame = true;
	Sleep(100);
	cv::imwrite(SAVES_DIR_PATH + output + ".png", this->saved_frame);
	std::cout << "saved as " << SAVES_DIR_PATH + output + ".png" << std::endl;
}

void Window::displayAnswers(cv::Mat frame) {
	if (!this->middle_bar.empty()) {
		unsigned int x = frame.cols / 2 - this->middle_bar.cols / 2;
		unsigned int y = frame.rows / 8;
		for (int col = 0; col < this->middle_bar.cols; col++) {
			for (int row = 0; row < this->middle_bar.rows; row++) {
				cv::Vec3b pixel = this->middle_bar.at<cv::Vec3b>(cv::Point(col, row));
				if (pixel != cv::Vec3b(255, 255, 255)) {
					frame.at<cv::Vec3b>(cv::Point(col + x, row + y)) = MIDDLE_BAR_COLOR;
				}
			}
		}
	}

	frame.rowRange(cv::Range(0, frame.rows / 9)).setTo(TOP_BAR_COLOR);

	int question_id = this->game.getCurrentQuestion();
	question_id = 0;

	std::string a1 = this->answers[std::to_string(question_id)][0];
	std::string a2 = this->answers[std::to_string(question_id)][1];

	size_t pos = a1.find('\n');

	if (pos < a1.size()) {
		std::string a1_1 = a1.substr(0, pos);
		std::string a1_2 = a1.substr(pos + 1);
		cv::putText(frame, a1_1, cv::Point(frame.cols / 4 - (a1_1.size() / 2) * 30, 50), cv::FONT_HERSHEY_DUPLEX, 2, TEXT_COLOR, 5);
		cv::putText(frame, a1_2, cv::Point(frame.cols / 4 - (a1_2.size() / 2) * 30, 100), cv::FONT_HERSHEY_DUPLEX, 2, TEXT_COLOR, 5);
	}
	else {
		cv::putText(frame, a1, cv::Point(frame.cols / 4 - (a1.size() / 2) * 30, 75), cv::FONT_HERSHEY_DUPLEX, 2, TEXT_COLOR, 5);
	}

	pos = a2.find('\n');
	if (pos < a2.size()) {
		std::string a2_1 = a2.substr(0, pos);
		std::string a2_2 = a2.substr(pos +1);
		cv::putText(frame, a2_1, cv::Point(3 * (frame.cols / 4) - (a2_1.size() / 2) * 30, 50), cv::FONT_HERSHEY_DUPLEX, 2, TEXT_COLOR, 5);
		cv::putText(frame, a2_2, cv::Point(3 * (frame.cols / 4) - (a2_2.size() / 2) * 30, 100), cv::FONT_HERSHEY_DUPLEX, 2, TEXT_COLOR, 5);
	}
	else {
		cv::putText(frame, a2, cv::Point(3 * (frame.cols / 4) - (a2.size() / 2) * 30, 75), cv::FONT_HERSHEY_DUPLEX, 2, TEXT_COLOR, 5);
	}
}

void Window::newGame() {
	this->play_countdown = true;
	this->game_started = true;
	this->gif_index = 0;
	while (this->play_countdown) {
		Sleep(500);
	}
	this->game = Game();
	this->play_transition_before = true;
	this->gif_index = 0;
}

bool Window::sameSide() {
	int first = 0; int second = 0;
	for (int i = 0; i < this->backup.size(); i++) {
		if (i == 0) {
			first = static_cast<int>(backup[i].x2 - backup[i].x1);
		}
		else {
			second = static_cast<int>(backup[i].x2 - backup[i].x1);
		}
	}
	if (first != 0 && second != 0) {
		return (first < this->camera.getWidth() / 2 && second < this->camera.getWidth() / 2) || (first > this->camera.getWidth() / 2 && second > this->camera.getWidth() / 2);
	}
	return false;
}

void Window::addPoints() {
	if (this->sameSide()) {
		this->game.addPoint();
	}
}

/*
void Window::displayResults(cv::Mat frame) {
	//cv::putText(frame, std::to_string(this->game.getPoints()), cv::Point(frame.cols/2 - 50, frame.rows/2 - 50), cv::FONT_HERSHEY_DUPLEX, 3, cv::Vec3b(99, 35, 251), 5);
	if (this->displayGif(frame, RESULTS_DIR_PATH + std::to_string(this->game.getPoints()) + "/", cv::Vec3b(144, 120, 254), "", false)) {
		this->play_results = false;
		this->game_started = false;
	}
}*/

Window::Window(std::string title, unsigned int camera_id) {

	this->camera = Camera(camera_id, MODEL_PATH);

	if (!this->camera.isOpened()) {
		this->camera = Camera(0, MODEL_PATH);
	}
	
	this->title = title;
	std::ifstream f(ANSWERS_JSON_PATH);
	this->answers = json::parse(f);
	f.close();
}
