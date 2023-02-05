#include "Camera.h"

bool Camera::isOpened() {
	return this->capture.isOpened();
}

int Camera::getWidth() {
	return static_cast<int>(this->capture.get(cv::CAP_PROP_FRAME_WIDTH));
}

int Camera::getHeight() {
	return static_cast<int>(this->capture.get(cv::CAP_PROP_FRAME_HEIGHT));
}

void Camera::read(cv::OutputArray image) {
	if (this->capture.isOpened()) {
		this->capture.read(image);
		cv::flip(image, image, 1);
	}
}

void Camera::analyse(cv::Mat image, std::vector<FaceInfo> &backup) {
	if (!image.empty()) {
		this->face_info = std::vector<FaceInfo>();
		this->centerface.detect(image, this->face_info);
		backup = this->face_info;
		Sleep(25);			// /!\ reduce CPU utilization
	}
	this->analysing = false;
}

void Camera::release() {
	this->capture.release();
}

Camera::Camera() {}

Camera::Camera(int camera_id, std::string model_path) {
	this->capture = cv::VideoCapture(camera_id, cv::CAP_DSHOW);
	this->capture.set(cv::CAP_PROP_FRAME_WIDTH, 1920);
	this->capture.set(cv::CAP_PROP_FRAME_HEIGHT, 1080);
	this->capture.set(cv::CAP_PROP_FPS, 60);
	this->capture.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));

	std::cout << this->capture.get(cv::CAP_PROP_FRAME_WIDTH) << "x" << this->capture.get(cv::CAP_PROP_FRAME_HEIGHT) << " " << this->capture.get(cv::CAP_PROP_FPS) << " fps" << std::endl;

	this->centerface = Centerface(model_path, this->getWidth() / 4, this->getHeight() / 4);
}

Camera::Camera(Camera& camera) {
	this->capture = camera.capture;
	this->centerface = camera.centerface;
	this->face_info = camera.face_info;
}