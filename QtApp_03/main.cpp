#include "qtapp_03.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QtApp_03 w;
	w.show();
	return a.exec();
}

