/********************************************************************************
** Form generated from reading UI file 'qtapp_03.ui'
**
** Created by: Qt User Interface Compiler version 5.0.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QTAPP_03_H
#define UI_QTAPP_03_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QtApp_03Class
{
public:
    QAction *actionOpen;
    QWidget *centralWidget;
    QPushButton *btnResume;
    QPushButton *btnSetParam;
    QLineEdit *winInput_04;
    QTextEdit *winFile;
    QFrame *line_3;
    QFrame *line;
    QLineEdit *winInput_01;
    QCheckBox *chbBG;
    QFrame *line_2;
    QTextEdit *winOutput;
    QPushButton *btnClear;
    QTextEdit *winFrameNumber;
    QPushButton *btnPause;
    QLabel *frameLabelBG;
    QPushButton *btnOpen;
    QLineEdit *winInput_02;
    QLineEdit *winInput_03;
    QLabel *frameLabelCV;
    QLabel *frameLabelOrig;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *QtApp_03Class)
    {
        if (QtApp_03Class->objectName().isEmpty())
            QtApp_03Class->setObjectName(QStringLiteral("QtApp_03Class"));
        QtApp_03Class->resize(946, 763);
        actionOpen = new QAction(QtApp_03Class);
        actionOpen->setObjectName(QStringLiteral("actionOpen"));
        centralWidget = new QWidget(QtApp_03Class);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        btnResume = new QPushButton(centralWidget);
        btnResume->setObjectName(QStringLiteral("btnResume"));
        btnResume->setGeometry(QRect(320, 650, 111, 31));
        QFont font;
        font.setPointSize(10);
        font.setBold(true);
        font.setWeight(75);
        btnResume->setFont(font);
        btnSetParam = new QPushButton(centralWidget);
        btnSetParam->setObjectName(QStringLiteral("btnSetParam"));
        btnSetParam->setGeometry(QRect(10, 650, 111, 31));
        btnSetParam->setFont(font);
        winInput_04 = new QLineEdit(centralWidget);
        winInput_04->setObjectName(QStringLiteral("winInput_04"));
        winInput_04->setGeometry(QRect(10, 610, 31, 31));
        winFile = new QTextEdit(centralWidget);
        winFile->setObjectName(QStringLiteral("winFile"));
        winFile->setGeometry(QRect(280, 10, 631, 31));
        line_3 = new QFrame(centralWidget);
        line_3->setObjectName(QStringLiteral("line_3"));
        line_3->setGeometry(QRect(10, 40, 881, 20));
        line_3->setFrameShape(QFrame::HLine);
        line_3->setFrameShadow(QFrame::Sunken);
        line = new QFrame(centralWidget);
        line->setObjectName(QStringLiteral("line"));
        line->setGeometry(QRect(10, 350, 891, 20));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        winInput_01 = new QLineEdit(centralWidget);
        winInput_01->setObjectName(QStringLiteral("winInput_01"));
        winInput_01->setGeometry(QRect(10, 490, 31, 31));
        chbBG = new QCheckBox(centralWidget);
        chbBG->setObjectName(QStringLiteral("chbBG"));
        chbBG->setGeometry(QRect(470, 370, 211, 21));
        QFont font1;
        font1.setPointSize(12);
        font1.setBold(false);
        font1.setItalic(false);
        font1.setWeight(50);
        chbBG->setFont(font1);
        line_2 = new QFrame(centralWidget);
        line_2->setObjectName(QStringLiteral("line_2"));
        line_2->setGeometry(QRect(440, 50, 20, 641));
        line_2->setFrameShape(QFrame::VLine);
        line_2->setFrameShadow(QFrame::Sunken);
        winOutput = new QTextEdit(centralWidget);
        winOutput->setObjectName(QStringLiteral("winOutput"));
        winOutput->setGeometry(QRect(10, 370, 431, 111));
        winOutput->setAutoFillBackground(false);
        winOutput->setReadOnly(true);
        btnClear = new QPushButton(centralWidget);
        btnClear->setObjectName(QStringLiteral("btnClear"));
        btnClear->setGeometry(QRect(110, 10, 91, 31));
        btnClear->setFont(font);
        winFrameNumber = new QTextEdit(centralWidget);
        winFrameNumber->setObjectName(QStringLiteral("winFrameNumber"));
        winFrameNumber->setGeometry(QRect(210, 10, 51, 31));
        QFont font2;
        font2.setPointSize(12);
        winFrameNumber->setFont(font2);
        winFrameNumber->setReadOnly(true);
        winFrameNumber->setOverwriteMode(true);
        btnPause = new QPushButton(centralWidget);
        btnPause->setObjectName(QStringLiteral("btnPause"));
        btnPause->setGeometry(QRect(210, 650, 111, 31));
        btnPause->setFont(font);
        frameLabelBG = new QLabel(centralWidget);
        frameLabelBG->setObjectName(QStringLiteral("frameLabelBG"));
        frameLabelBG->setGeometry(QRect(470, 400, 432, 288));
        frameLabelBG->setAutoFillBackground(true);
        btnOpen = new QPushButton(centralWidget);
        btnOpen->setObjectName(QStringLiteral("btnOpen"));
        btnOpen->setGeometry(QRect(10, 10, 91, 31));
        btnOpen->setFont(font);
        winInput_02 = new QLineEdit(centralWidget);
        winInput_02->setObjectName(QStringLiteral("winInput_02"));
        winInput_02->setGeometry(QRect(10, 530, 31, 31));
        winInput_03 = new QLineEdit(centralWidget);
        winInput_03->setObjectName(QStringLiteral("winInput_03"));
        winInput_03->setGeometry(QRect(10, 570, 31, 31));
        frameLabelCV = new QLabel(centralWidget);
        frameLabelCV->setObjectName(QStringLiteral("frameLabelCV"));
        frameLabelCV->setGeometry(QRect(460, 60, 432, 288));
        frameLabelCV->setAutoFillBackground(true);
        frameLabelOrig = new QLabel(centralWidget);
        frameLabelOrig->setObjectName(QStringLiteral("frameLabelOrig"));
        frameLabelOrig->setGeometry(QRect(10, 60, 432, 288));
        frameLabelOrig->setAutoFillBackground(true);
        QtApp_03Class->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(QtApp_03Class);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 946, 21));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        QtApp_03Class->setMenuBar(menuBar);
        mainToolBar = new QToolBar(QtApp_03Class);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        QtApp_03Class->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(QtApp_03Class);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        QtApp_03Class->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuFile->addAction(actionOpen);

        retranslateUi(QtApp_03Class);

        QMetaObject::connectSlotsByName(QtApp_03Class);
    } // setupUi

    void retranslateUi(QMainWindow *QtApp_03Class)
    {
        QtApp_03Class->setWindowTitle(QApplication::translate("QtApp_03Class", "QtApp_03", 0));
        actionOpen->setText(QApplication::translate("QtApp_03Class", "open", 0));
        btnResume->setText(QApplication::translate("QtApp_03Class", "resume", 0));
        btnSetParam->setText(QApplication::translate("QtApp_03Class", "set", 0));
        chbBG->setText(QApplication::translate("QtApp_03Class", "enable background image", 0));
        btnClear->setText(QApplication::translate("QtApp_03Class", "Clear", 0));
        btnPause->setText(QApplication::translate("QtApp_03Class", "pause", 0));
        frameLabelBG->setText(QString());
        btnOpen->setText(QApplication::translate("QtApp_03Class", "Open", 0));
        frameLabelCV->setText(QString());
        frameLabelOrig->setText(QString());
        menuFile->setTitle(QApplication::translate("QtApp_03Class", "File", 0));
    } // retranslateUi

};

namespace Ui {
    class QtApp_03Class: public Ui_QtApp_03Class {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTAPP_03_H
