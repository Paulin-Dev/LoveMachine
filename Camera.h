#pragma once

#include <iostream>
#include <opencv2/opencv.hpp>
#include "cv_dnn_centerface.h"
#include <Windows.h>

class Camera {
private:
	cv::VideoCapture capture;
	Centerface centerface;
	std::vector<FaceInfo> face_info;
public:
	bool analysing = false;

	bool isOpened();
	int getWidth();
	int getHeight();

	void read(cv::OutputArray);
	void analyse(cv::Mat, std::vector <FaceInfo> &);
	void release();

	Camera();
	Camera(int, std::string);

	/*
	friend std::ostream& operator<<(std::ostream& os, Camera camera) {
		os << "Camera : " << camera.analysing << std::endl;
		return os;
	};
	*/
};
