#include <vector>
#include <string>

#include "vobject.h"
#include <opencv2\opencv.hpp>

using namespace std;
using namespace cv;

vobject::vobject()
{
	onVObject a;
	vecVObject.push_back(a);
}

vobject::~vobject(void)
{
}

void vobject::refresh(cv::Mat& fore)
{
	convertContours(fore);

	//printVObjects();

	Mat vobFrame = Mat(fore.size(), CV_8UC3,Scalar(0));
	drawVObjects(vobFrame);
	namedWindow("vobFrame", CV_WINDOW_AUTOSIZE);
	imshow("vobFrame",vobFrame);

	Mat foreShadow = Mat(fore.size(), CV_8U,Scalar(0));
	deleteShadows(fore, foreShadow);
	namedWindow("fore without shadows", CV_WINDOW_AUTOSIZE);
	imshow("fore without shadows", foreShadow);

}

void vobject::convertContours(cv::Mat& foreOrig)
{
	std::vector<std::vector<Point> > contours;
	Mat fore = Mat(foreOrig);
	findContours(fore,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);

	if(contours.size()>0)
	{
		for(unsigned int i=0; i<contours.size(); ++i)
		{
			if(contours.at(i).size()>0)
			{
				for(unsigned int j=0; j<contours.at(i).size(); ++j)
				{					
					onVObject Vob;
					Vob.mob = contours.at(i).at(j);
					Vob.rec.width = 10;
					Vob.rec.height = 10;
					Vob.rec.x = contours.at(i).at(j).x-(Vob.rec.width/2); 
					Vob.rec.y = contours.at(i).at(j).y-(Vob.rec.height/2);
					
					
					vecVObject.push_back(Vob);
				}
			}
		}
	}
}

void vobject::drawVObjects(cv::Mat& vobFrame)
{
	if(vecVObject.size()>0)
	{
		for(unsigned int i=0; i<vecVObject.size(); ++i)
		{
			rectangle(vobFrame, vecVObject.at(i).rec.tl(), vecVObject.at(i).rec.br(), Scalar(255,0,0));
		}
	}
}

void vobject::deleteShadows(cv::Mat& fore, cv::Mat& foreShadow)
{
	Mat grey = Mat(fore.size(), CV_8U,Scalar(125));

	Mat vobFrame = Mat(fore.size(), CV_8UC3,Scalar(0));

	

	if(vecVObject.size()>0)
	{
		for(unsigned int i=0; i<vecVObject.size(); ++i)
		{
			cout << "fore: " << fore.at<unsigned int>(vecVObject.at(i).mob.x,vecVObject.at(i).mob.y) << endl;
			if(fore.at<unsigned int>(vecVObject.at(i).mob.x,vecVObject.at(i).mob.y) > 200)
			{
				cout << "DRINNNNNNNNNNNNN" << fore.at<unsigned int>(vecVObject.at(i).mob.x,vecVObject.at(i).mob.y) << endl;
				//onVObject Vob = vecVObject.at(i);
				//cout << Vob.mob << " " <<  Vob.rec.x+(Vob.rec.width/2) << ":" << Vob.rec.y+(Vob.rec.height/2) << endl;

				rectangle(vobFrame, vecVObject.at(i).rec.tl(), vecVObject.at(i).rec.br(), Scalar(255,0,0));
			}
		}
	}

	namedWindow("tt", CV_WINDOW_AUTOSIZE);
	imshow("tt",vobFrame);


	//foreShadow = grey - fore;
}

void vobject::printVObjects()
{
	cout << "output vecVObject: " << vecVObject.size() << endl;
	if(vecVObject.size()>0)
	{
		for(unsigned int i=0; i<vecVObject.size(); ++i)
		{
			cout << "mob: " << vecVObject.at(i).mob << " ";
		}
	}	
}


