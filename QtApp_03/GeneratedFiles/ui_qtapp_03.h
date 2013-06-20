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
    QLineEdit *winShow_01;
    QLineEdit *winShow_02;
    QLineEdit *winShow_03;
    QLineEdit *winShow_04;
    QLineEdit *winShow_05;
    QLineEdit *winInput_05;
    QPushButton *btnRestartLearning;
    QPushButton *btnFaster;
    QPushButton *btnSlower;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *QtApp_03Class)
    {
        if (QtApp_03Class->objectName().isEmpty())
            QtApp_03Class->setObjectName(QStringLiteral("QtApp_03Class"));
        QtApp_03Class->resize(933, 730);
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
        winInput_04->setGeometry(QRect(120, 610, 31, 31));
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
        winInput_01->setGeometry(QRect(120, 490, 31, 31));
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
        frameLabelBG->setGeometry(QRect(460, 400, 432, 288));
        frameLabelBG->setAutoFillBackground(true);
        btnOpen = new QPushButton(centralWidget);
        btnOpen->setObjectName(QStringLiteral("btnOpen"));
        btnOpen->setGeometry(QRect(10, 10, 91, 31));
        btnOpen->setFont(font);
        winInput_02 = new QLineEdit(centralWidget);
        winInput_02->setObjectName(QStringLiteral("winInput_02"));
        winInput_02->setGeometry(QRect(120, 530, 31, 31));
        winInput_03 = new QLineEdit(centralWidget);
        winInput_03->setObjectName(QStringLiteral("winInput_03"));
        winInput_03->setGeometry(QRect(120, 570, 31, 31));
        frameLabelCV = new QLabel(centralWidget);
        frameLabelCV->setObjectName(QStringLiteral("frameLabelCV"));
        frameLabelCV->setGeometry(QRect(460, 60, 432, 288));
        frameLabelCV->setAutoFillBackground(true);
        frameLabelOrig = new QLabel(centralWidget);
        frameLabelOrig->setObjectName(QStringLiteral("frameLabelOrig"));
        frameLabelOrig->setGeometry(QRect(10, 60, 432, 288));
        frameLabelOrig->setAutoFillBackground(true);
        winShow_01 = new QLineEdit(centralWidget);
        winShow_01->setObjectName(QStringLiteral("winShow_01"));
        winShow_01->setGeometry(QRect(10, 490, 101, 31));
        winShow_01->setFont(font);
        winShow_02 = new QLineEdit(centralWidget);
        winShow_02->setObjectName(QStringLiteral("winShow_02"));
        winShow_02->setGeometry(QRect(10, 530, 101, 31));
        winShow_02->setFont(font);
        winShow_03 = new QLineEdit(centralWidget);
        winShow_03->setObjectName(QStringLiteral("winShow_03"));
        winShow_03->setGeometry(QRect(10, 570, 101, 31));
        winShow_03->setFont(font);
        winShow_04 = new QLineEdit(centralWidget);
        winShow_04->setObjectName(QStringLiteral("winShow_04"));
        winShow_04->setGeometry(QRect(10, 610, 101, 31));
        winShow_04->setFont(font);
        winShow_05 = new QLineEdit(centralWidget);
        winShow_05->setObjectName(QStringLiteral("winShow_05"));
        winShow_05->setGeometry(QRect(240, 490, 91, 31));
        winShow_05->setFont(font);
        winInput_05 = new QLineEdit(centralWidget);
        winInput_05->setObjectName(QStringLiteral("winInput_05"));
        winInput_05->setGeometry(QRect(340, 490, 31, 31));
        btnRestartLearning = new QPushButton(centralWidget);
        btnRestartLearning->setObjectName(QStringLiteral("btnRestartLearning"));
        btnRestartLearning->setGeometry(QRect(240, 540, 181, 31));
        btnRestartLearning->setFont(font);
        btnFaster = new QPushButton(centralWidget);
        btnFaster->setObjectName(QStringLiteral("btnFaster"));
        btnFaster->setGeometry(QRect(210, 610, 111, 31));
        btnFaster->setFont(font);
        btnSlower = new QPushButton(centralWidget);
        btnSlower->setObjectName(QStringLiteral("btnSlower"));
        btnSlower->setGeometry(QRect(320, 610, 111, 31));
        btnSlower->setFont(font);
        QtApp_03Class->setCentralWidget(centralWidget);
        mainToolBar = new QToolBar(QtApp_03Class);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        QtApp_03Class->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(QtApp_03Class);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        QtApp_03Class->setStatusBar(statusBar);

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
        winShow_01->setText(QApplication::translate("QtApp_03Class", "epsilon1:", 0));
        winShow_02->setText(QApplication::translate("QtApp_03Class", "epsilon2:", 0));
        winShow_03->setText(QApplication::translate("QtApp_03Class", "alpha1:", 0));
        winShow_04->setText(QApplication::translate("QtApp_03Class", "alpha2:", 0));
        winShow_05->setText(QApplication::translate("QtApp_03Class", "Lerndauer:", 0));
        btnRestartLearning->setText(QApplication::translate("QtApp_03Class", "restart learning phase", 0));
        btnFaster->setText(QApplication::translate("QtApp_03Class", "faster", 0));
        btnSlower->setText(QApplication::translate("QtApp_03Class", "slower", 0));
    } // retranslateUi

};

namespace Ui {
    class QtApp_03Class: public Ui_QtApp_03Class {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTAPP_03_H
