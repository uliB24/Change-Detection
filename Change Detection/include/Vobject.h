#pragma once

#include "opencv2\opencv.hpp"

struct onVObject
{
	cv::Rect rec;	// aprox. size of the object
	cv::Point mob; // middle of the object
	onVObject() : mob(0,0), rec(0,0,0,0) {}
};

class vobject
{
public:
	vobject();
	~vobject();

	void refresh(cv::Mat&); // execute every frame, refresh the obj in the vec and push new obj
	void convertContours(cv::Mat&); // convert contours to vecOBjects
	void printVObjects(); // debug
	void drawVObjects(cv::Mat&);
	void deleteShadows(cv::Mat&, cv::Mat&);

	bool isInVec(cv::Point); // look if the given points is in a already existing object

private:
	std::vector<onVObject> vecVObject;
};