#include <iostream>
#include <string>
#include <sstream>
#include <iomanip> 

#include <QPrintDialog>
#include <QtWidgets>

#include "qtapp_03.h"

using namespace std;

QtApp_03::QtApp_03(QWidget *parent) : QMainWindow(parent)
{
	setupUi(this);

	
	setupUi(this);

	isJpeg = false;
	frameInt = 0;


	timer = new QTimer(this);
	timer->setInterval(100);

	connect(actionOpen, SIGNAL(triggered()), this, SLOT(open()));
	connect(timer, SIGNAL(timeout()), this, SLOT(processing()));

	// param
	connect(winInput_01,SIGNAL(textChanged(QString)),this,SLOT(txtChangeWinInput_01(QString)));
	connect(winInput_02,SIGNAL(textChanged(QString)),this,SLOT(txtChangeWinInput_02(QString)));
	connect(winInput_03,SIGNAL(textChanged(QString)),this,SLOT(txtChangeWinInput_03(QString)));
	connect(winInput_04,SIGNAL(textChanged(QString)),this,SLOT(txtChangeWinInput_04(QString)));

}

QtApp_03::~QtApp_03()
{

}

void QtApp_03::on_btnOpen_clicked() 
{
	frameInt = 0;
	fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::currentPath());
	

	QImage frame(fileName);
	frameLabelOrig->setPixmap(QPixmap::fromImage(frame));
	frameLabelOrig->adjustSize();
	
	frameLabelCV->setPixmap(QPixmap::fromImage(frame));
	frameLabelCV->adjustSize();

	winOutput->append("open file: ");
	winOutput->append(fileName);
	winOutput->append("\n");

	winFile->append(fileName);

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

	QImage frame(filePath);
	frameLabelOrig->setPixmap(QPixmap::fromImage(frame));
	frameLabelOrig->adjustSize();



	++frameInt;
	winFrameNumber->clear();
	winFrameNumber->append(QString::number(frameInt));

	// checkbox and backgroundimage
	if(chbBG->isChecked())
	{
		//winOutput->append("is checked");



	}
	else
	{
		//winOutput->append("is not checked");



	}
}

void QtApp_03::on_btnSetParam_clicked()
{
	

}

void QtApp_03::on_btnClear_clicked() 
{
	winFile->clear();
	winOutput->clear();
	winFrameNumber->clear();

	timer->stop();
	timer = new QTimer(this);
	timer->setInterval(100);

	frameLabelOrig->setPixmap(QPixmap());
	frameLabelCV->setPixmap(QPixmap());
}

void QtApp_03::on_btnPause_clicked() 
{
	timer->stop();
}

void QtApp_03::on_btnResume_clicked() 
{
	timer->start();
}


void QtApp_03::open(void)
{
	frameInt = 0;
	fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::currentPath());

	if(fileName.isEmpty())
		return;

	QImage frame(fileName);
	// opencv_mat(fileName)
	// bg = SOB_CF(opencv_mat)

	frameLabelOrig->setPixmap(QPixmap::fromImage(frame));
	frameLabelOrig->adjustSize();

	frameLabelCV->setPixmap(QPixmap::fromImage(frame));
	frameLabelCV->adjustSize();

	winFile->append(fileName);

	winOutput->append("open file: ");
	winOutput->append(fileName);
	winOutput->append("\n");

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







