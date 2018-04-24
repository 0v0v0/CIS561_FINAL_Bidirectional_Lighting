/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>
#include "mygl.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionQuit;
    QAction *actionLoad_Scene;
    QAction *actionRender;
    QAction *actionCamera_Controls;
    QWidget *centralWidget;
    MyGL *mygl;
    QSpinBox *samplesSpinBox;
    QLabel *label;
    QSpinBox *recursionSpinBox;
    QLabel *label_2;
    QCheckBox *checkBox_Progressive;
    QComboBox *integratorSpinBox;
    QLabel *label_3;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuHelp;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(640, 607);
        actionQuit = new QAction(MainWindow);
        actionQuit->setObjectName(QStringLiteral("actionQuit"));
        actionLoad_Scene = new QAction(MainWindow);
        actionLoad_Scene->setObjectName(QStringLiteral("actionLoad_Scene"));
        actionRender = new QAction(MainWindow);
        actionRender->setObjectName(QStringLiteral("actionRender"));
        actionCamera_Controls = new QAction(MainWindow);
        actionCamera_Controls->setObjectName(QStringLiteral("actionCamera_Controls"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        mygl = new MyGL(centralWidget);
        mygl->setObjectName(QStringLiteral("mygl"));
        mygl->setGeometry(QRect(11, 11, 618, 432));
        QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(1);
        sizePolicy.setHeightForWidth(mygl->sizePolicy().hasHeightForWidth());
        mygl->setSizePolicy(sizePolicy);
        mygl->setMinimumSize(QSize(618, 432));
        samplesSpinBox = new QSpinBox(centralWidget);
        samplesSpinBox->setObjectName(QStringLiteral("samplesSpinBox"));
        samplesSpinBox->setGeometry(QRect(150, 470, 81, 22));
        samplesSpinBox->setMinimum(1);
        samplesSpinBox->setMaximum(100);
        samplesSpinBox->setValue(10);
        label = new QLabel(centralWidget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(20, 460, 121, 41));
        label->setWordWrap(true);
        recursionSpinBox = new QSpinBox(centralWidget);
        recursionSpinBox->setObjectName(QStringLiteral("recursionSpinBox"));
        recursionSpinBox->setGeometry(QRect(380, 470, 81, 22));
        recursionSpinBox->setMinimum(1);
        recursionSpinBox->setMaximum(10);
        recursionSpinBox->setValue(5);
        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(280, 460, 91, 41));
        label_2->setWordWrap(true);
        checkBox_Progressive = new QCheckBox(centralWidget);
        checkBox_Progressive->setObjectName(QStringLiteral("checkBox_Progressive"));
        checkBox_Progressive->setGeometry(QRect(490, 470, 131, 20));
        checkBox_Progressive->setChecked(true);
        integratorSpinBox = new QComboBox(centralWidget);
        integratorSpinBox->setObjectName(QStringLiteral("integratorSpinBox"));
        integratorSpinBox->setGeometry(QRect(130, 530, 161, 22));
        label_3 = new QLabel(centralWidget);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(20, 520, 91, 41));
        label_3->setWordWrap(true);
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 640, 26));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QStringLiteral("menuHelp"));
        MainWindow->setMenuBar(menuBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuHelp->menuAction());
        menuFile->addAction(actionRender);
        menuFile->addAction(actionLoad_Scene);
        menuFile->addAction(actionQuit);
        menuHelp->addAction(actionCamera_Controls);

        retranslateUi(MainWindow);
        QObject::connect(mygl, SIGNAL(sig_ResizeToCamera(int,int)), MainWindow, SLOT(slot_ResizeToCamera(int,int)));
        QObject::connect(samplesSpinBox, SIGNAL(valueChanged(int)), mygl, SLOT(slot_SetNumSamplesSqrt(int)));
        QObject::connect(recursionSpinBox, SIGNAL(valueChanged(int)), mygl, SLOT(slot_SetRecursionLimit(int)));
        QObject::connect(checkBox_Progressive, SIGNAL(toggled(bool)), mygl, SLOT(slot_SetProgressiveRender(bool)));
        QObject::connect(integratorSpinBox, SIGNAL(currentIndexChanged(int)), mygl, SLOT(slot_SetIntegratorType(int)));
        QObject::connect(mygl, SIGNAL(sig_DisableGUI(bool)), MainWindow, SLOT(slot_DisableGUI(bool)));

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Path Tracer", 0));
        actionQuit->setText(QApplication::translate("MainWindow", "Quit", 0));
        actionQuit->setShortcut(QApplication::translate("MainWindow", "Ctrl+Q", 0));
        actionLoad_Scene->setText(QApplication::translate("MainWindow", "Load Scene", 0));
        actionLoad_Scene->setShortcut(QApplication::translate("MainWindow", "Ctrl+L", 0));
        actionRender->setText(QApplication::translate("MainWindow", "Render", 0));
        actionRender->setShortcut(QApplication::translate("MainWindow", "Ctrl+R", 0));
        actionCamera_Controls->setText(QApplication::translate("MainWindow", "Camera Controls", 0));
        label->setText(QApplication::translate("MainWindow", "Square root of pixel samples count:", 0));
        label_2->setText(QApplication::translate("MainWindow", "Recursion limit:", 0));
        checkBox_Progressive->setText(QApplication::translate("MainWindow", "Progressive Preview", 0));
        integratorSpinBox->clear();
        integratorSpinBox->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "Naive", 0)
         << QApplication::translate("MainWindow", "BiDirectional Lighting", 0)
         << QApplication::translate("MainWindow", "Indirect Lighting", 0)
         << QApplication::translate("MainWindow", "Full Lighting", 0)
        );
        label_3->setText(QApplication::translate("MainWindow", "Integrator type:", 0));
        menuFile->setTitle(QApplication::translate("MainWindow", "File", 0));
        menuHelp->setTitle(QApplication::translate("MainWindow", "Help", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
