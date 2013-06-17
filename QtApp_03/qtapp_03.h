#ifndef QTAPP_03_H
#define QTAPP_03_H

#include <QtWidgets>
#include <opencv2/opencv.hpp>
#include "ui_qtapp_03.h"

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
	int frameInt;
	QString param1,param2,param3,param4;

	//SOBS_CF bg;
public slots:
	

private slots:
	void open(void);
	void processing(void);
	void on_btnOpen_clicked();
	void on_btnClear_clicked();
	void on_btnPause_clicked();
	void on_btnResume_clicked();
	void on_btnSetParam_clicked();

	inline void txtChangeWinInput_01(QString input){param1=input;winOutput->append("set param1 to "+param1);};
	inline void txtChangeWinInput_02(QString input){param2=input;winOutput->append("set param2 to "+param2);};
	inline void txtChangeWinInput_03(QString input){param3=input;winOutput->append("set param3 to "+param3);};
	inline void txtChangeWinInput_04(QString input){param4=input;winOutput->append("set param4 to "+param4);};
};



#endif // QTAPP_03_H
