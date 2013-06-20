#include <iostream>
#include <string>
#include <sstream>
#include <iomanip> 
#include <vector>

#include <QPrintDialog>
#include <QtWidgets>

#include <opencv2/opencv.hpp>

#include "qtapp_03.h"

using namespace std;
using namespace cv;

QtApp_03::QtApp_03(QWidget *parent) : QMainWindow(parent)
{
	setupUi(this);

	isBgEne = false;
	isJpeg = false;
	frameInt = 1;

	timer = new QTimer(this);
	timerInter = 100;
	timer->setInterval(timerInter);

	//connect(actionOpen, SIGNAL(triggered()), this, SLOT(open()));
	connect(timer, SIGNAL(timeout()), this, SLOT(processing()));
	
	// param
	connect(winInput_01,SIGNAL(textChanged(QString)),this,SLOT(txtChangeWinInput_01(QString)));
	connect(winInput_02,SIGNAL(textChanged(QString)),this,SLOT(txtChangeWinInput_02(QString)));
	connect(winInput_03,SIGNAL(textChanged(QString)),this,SLOT(txtChangeWinInput_03(QString)));
	connect(winInput_04,SIGNAL(textChanged(QString)),this,SLOT(txtChangeWinInput_04(QString)));
	connect(winInput_05,SIGNAL(textChanged(QString)),this,SLOT(txtChangeWinInput_05(QString)));
}

QtApp_03::~QtApp_03()
{

}

void QtApp_03::on_btnOpen_clicked() 
{
	fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::currentPath());

	if(fileName.isEmpty())
		return;

	
	Mat cv_frame = imread(fileName.toStdString(), CV_LOAD_IMAGE_COLOR);
	if(!cv_frame.data)
		return;
	frame = cv_frame.clone();
	sobs = SOBS_CF(cv_frame);
	cvtColor( cv_frame, cv_frame, CV_BGR2RGB);
	
	QImage qImgOrig((uchar*)cv_frame.data, cv_frame.cols, cv_frame.rows, cv_frame.step, QImage::Format_RGB888);
	frameLabelOrig->setPixmap(QPixmap::fromImage(qImgOrig));

	

	winFile->append(fileName);

	winOutput->append("start processing.. ");

	if(fileName.at(fileName.size()-3) == QString("b"))
	{
		isJpeg = false;
		fileName.resize(fileName.size()-9);
	}else
	{
		isJpeg = true;
		fileName.resize(fileName.size()-10);
	}


	timer->start();
	
}

void QtApp_03::processing()
{
	QString filePath = fileName;
	ostringstream oss;
	
	if(isJpeg)
	{
		oss << std::setfill ('0') << std::setw (6);
		oss << frameInt << ".jpg";
	}else{
		oss << std::setfill ('0') << std::setw (5);
		oss << frameInt << ".bmp";
	}

	filePath.append(oss.str().c_str());

	Mat cv_frame = imread(filePath.toStdString(), CV_LOAD_IMAGE_COLOR);
	frame = cv_frame.clone();
	Mat cv_fore;
	Mat cv_frame2;
	cvtColor( cv_frame, cv_frame2, CV_BGR2RGB);
	
	// convert from Mat to QImage and show on gui
	QImage qImgOrig((uchar*)cv_frame2.data, cv_frame2.cols, cv_frame2.rows, cv_frame2.step, QImage::Format_RGB888);
	frameLabelOrig->setPixmap(QPixmap::fromImage(qImgOrig));
	

	if(!cv_frame.data)
		return;

	sobs.update(cv_frame,cv_fore);

	
	// convert from Mat to QImage and show on gui
	
	QImage qImgProc((uchar*)cv_fore.data, cv_fore.cols, cv_fore.rows, cv_fore.step, QImage::Format_Indexed8);
	
	QVector<QRgb> colorTable;
    for (int i=0; i<256; i++)
           colorTable.push_back(qRgb(i,i,i));
	qImgProc.setColorTable(colorTable);
	

	frameLabelCV->setPixmap(QPixmap::fromImage(qImgProc));
	

	++frameInt;
	winFrameNumber->clear();
	winFrameNumber->append(QString::number(frameInt));

	// checkbox and backgroundimage
	if(chbBG->isChecked())
	{
		if(isBgEne == false)
			winOutput->append("Background is enabled");
		isBgEne = true;

		Mat back;
		sobs.getBackgroundImage(back);	
		cvtColor( back, back, CV_BGR2RGB);
		QImage qImgBack((uchar*)back.data, back.cols, back.rows, back.step, QImage::Format_RGB888);

		frameLabelBG->setPixmap(QPixmap::fromImage(qImgBack));
		
	}
	else
	{
		if(isBgEne == true)
			winOutput->append("Background is disabled");
		isBgEne = false;
	}
}

void QtApp_03::on_btnSlower_clicked()
{
	timerInter +=20;
	timer->setInterval(timerInter);
	winOutput->append("set timer interval to "+QString::number(timerInter));
}

void QtApp_03::on_btnFaster_clicked()
{
	timerInter -=20;
	timer->setInterval(timerInter);
	winOutput->append("set timer interval to "+QString::number(timerInter));
}

void QtApp_03::on_btnSetParam_clicked()
{
	winOutput->append("activate new parameter!");
	sobs = SOBS_CF(frame, epsilon1, epsilon2,alpha1, alpha2, duraLearn);
}

void QtApp_03::on_btnRestartLearning_clicked()
{
	winOutput->append("restart learning phase with duration "+QString::number(duraLearn)+"!");
	
}

void QtApp_03::on_btnClear_clicked() 
{
	winFile->clear();
	winOutput->clear();
	winFrameNumber->clear();
	frameInt = 1;

	timer->stop();
	timer->setInterval(100);

	frameLabelOrig->clear();
	frameLabelCV->clear();
	frameLabelBG->clear();

	winInput_01->clear();
	winInput_02->clear();
	winInput_03->clear();
	winInput_04->clear();
	winInput_05->clear();

	chbBG->setChecked(false);
}

void QtApp_03::on_btnPause_clicked() 
{
	timer->stop();
	winOutput->append("stopped processing.. ");
}

void QtApp_03::on_btnResume_clicked() 
{
	timer->start();
	winOutput->append("start processing.. ");
}
/*
void QtApp_03::open(void)
{
	fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::currentPath());

	if(fileName.isEmpty())
		return;

	
	Mat cv_frame = imread(fileName.toStdString(), CV_LOAD_IMAGE_COLOR);
	if(!cv_frame.data)
		return;

	QImage qImgOrig((uchar*)cv_frame.data, cv_frame.cols, cv_frame.rows, cv_frame.step, QImage::Format_RGB888);
	frameLabelOrig->setPixmap(QPixmap::fromImage(qImgOrig));

	sobs = SOBS_CF(cv_frame);

	winFile->append(fileName);

	winOutput->append("start processing.. ");

	if(fileName.at(fileName.size()-3) == QString("b"))
	{
		isJpeg = false;
		fileName.resize(fileName.size()-9);
	}else
	{
		isJpeg = true;
		fileName.resize(fileName.size()-10);
	}


	timer->start();
}

*/
QImage MatToQImage(const Mat& mat)
{
    // 8-bits unsigned, NO. OF CHANNELS=1
    if(mat.type()==CV_8UC1)
    {
        // Set the color table (used to translate colour indexes to qRgb values)
        QVector<QRgb> colorTable;
        for (int i=0; i<256; i++)
            colorTable.push_back(qRgb(i,i,i));
        // Copy input Mat
        const uchar *qImageBuffer = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_Indexed8);
        img.setColorTable(colorTable);
        return img;
    }
    // 8-bits unsigned, NO. OF CHANNELS=3
    if(mat.type()==CV_8UC3)
    {
        // Copy input Mat
        const uchar *qImageBuffer = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return img.rgbSwapped();
    }
    else
    {
        qDebug() << "ERROR: Mat could not be converted to QImage.";
        return QImage();
    }
}







