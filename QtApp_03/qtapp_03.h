#ifndef QTAPP_03_H
#define QTAPP_03_H

#include <QtWidgets>
#include <opencv2/opencv.hpp>
#include "ui_qtapp_03.h"
#include "SOBS_CF.h"

class QtApp_03 : public QMainWindow, private Ui::QtApp_03Class
{
	Q_OBJECT

public:
	QtApp_03(QWidget *parent = 0);
	~QtApp_03();

private:
	Ui::QtApp_03Class ui;
	QString fileName;
	QTimer *timer;

	bool isJpeg;
	bool isBgEne;
	int frameInt;
	int timerInter;
	float epsilon1;
	float epsilon2;
	float alpha1;
	float alpha2;
	float duraLearn;
	QString param1,param2,param3,param4;

	SOBS_CF sobs;
	cv::Mat frame;
public slots:
	

private slots:
	void processing(void);
	void on_btnOpen_clicked();
	void on_btnClear_clicked();
	void on_btnPause_clicked();
	void on_btnResume_clicked();
	void on_btnSetParam_clicked();
	void on_btnRestartLearning_clicked();
	void on_btnSlower_clicked();
	void on_btnFaster_clicked();

	inline void txtChangeWinInput_01(QString input){epsilon1=input.toFloat();winOutput->append("set variable epsilon1 to "+QString::number(epsilon1));};
	inline void txtChangeWinInput_02(QString input){epsilon2=input.toFloat();winOutput->append("set variable epsilon2 to "+QString::number(epsilon2));};
	inline void txtChangeWinInput_03(QString input){alpha1=input.toFloat();winOutput->append("set variable alpha1 to "+QString::number(alpha1));};
	inline void txtChangeWinInput_04(QString input){alpha2=input.toFloat();winOutput->append("set variable alpha2 to "+QString::number(alpha2));};
	inline void txtChangeWinInput_05(QString input){duraLearn=input.toFloat();winOutput->append("set variable duraLearn to "+QString::number(duraLearn));};
};



#endif // QTAPP_03_H
