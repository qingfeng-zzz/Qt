/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QLabel *label;
    QStackedWidget *stackedWidget;
    QWidget *loginpage;
    QWidget *layoutWidget;
    QGridLayout *gridLayout_2;
    QSpacerItem *verticalSpacer;
    QSpacerItem *horizontalSpacer_2;
    QGridLayout *gridLayout;
    QLineEdit *ipEdit;
    QLabel *label_2;
    QLabel *label_3;
    QLineEdit *nameEdit;
    QSpacerItem *horizontalSpacer;
    QPushButton *btJoin;
    QSpacerItem *verticalSpacer_2;
    QWidget *chatpage;
    QWidget *layoutWidget1;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QTextEdit *textEdit;
    QListWidget *userList;
    QHBoxLayout *horizontalLayout_2;
    QLineEdit *messageEdit;
    QPushButton *btSend;
    QPushButton *btLeave;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(728, 540);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        label = new QLabel(centralwidget);
        label->setObjectName("label");
        label->setGeometry(QRect(20, -1, 651, 41));
        QFont font;
        font.setFamilies({QString::fromUtf8("Franklin Gothic")});
        font.setPointSize(16);
        label->setFont(font);
        label->setFrameShape(QFrame::Shape::Box);
        label->setFrameShadow(QFrame::Shadow::Plain);
        label->setScaledContents(false);
        label->setAlignment(Qt::AlignmentFlag::AlignCenter);
        stackedWidget = new QStackedWidget(centralwidget);
        stackedWidget->setObjectName("stackedWidget");
        stackedWidget->setGeometry(QRect(10, 30, 711, 491));
        loginpage = new QWidget();
        loginpage->setObjectName("loginpage");
        layoutWidget = new QWidget(loginpage);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(0, 0, 711, 491));
        gridLayout_2 = new QGridLayout(layoutWidget);
        gridLayout_2->setObjectName("gridLayout_2");
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        gridLayout_2->addItem(verticalSpacer, 0, 1, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_2, 1, 0, 1, 1);

        gridLayout = new QGridLayout();
        gridLayout->setObjectName("gridLayout");
        ipEdit = new QLineEdit(layoutWidget);
        ipEdit->setObjectName("ipEdit");

        gridLayout->addWidget(ipEdit, 0, 1, 1, 1);

        label_2 = new QLabel(layoutWidget);
        label_2->setObjectName("label_2");

        gridLayout->addWidget(label_2, 0, 0, 1, 1);

        label_3 = new QLabel(layoutWidget);
        label_3->setObjectName("label_3");

        gridLayout->addWidget(label_3, 1, 0, 1, 1);

        nameEdit = new QLineEdit(layoutWidget);
        nameEdit->setObjectName("nameEdit");

        gridLayout->addWidget(nameEdit, 1, 1, 1, 1);


        gridLayout_2->addLayout(gridLayout, 1, 1, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout_2->addItem(horizontalSpacer, 1, 2, 1, 1);

        btJoin = new QPushButton(layoutWidget);
        btJoin->setObjectName("btJoin");

        gridLayout_2->addWidget(btJoin, 2, 1, 1, 1);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        gridLayout_2->addItem(verticalSpacer_2, 3, 1, 1, 1);

        stackedWidget->addWidget(loginpage);
        chatpage = new QWidget();
        chatpage->setObjectName("chatpage");
        layoutWidget1 = new QWidget(chatpage);
        layoutWidget1->setObjectName("layoutWidget1");
        layoutWidget1->setGeometry(QRect(10, 10, 681, 481));
        verticalLayout = new QVBoxLayout(layoutWidget1);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        textEdit = new QTextEdit(layoutWidget1);
        textEdit->setObjectName("textEdit");

        horizontalLayout->addWidget(textEdit);

        userList = new QListWidget(layoutWidget1);
        userList->setObjectName("userList");
        userList->setMaximumSize(QSize(120, 16777215));

        horizontalLayout->addWidget(userList);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        messageEdit = new QLineEdit(layoutWidget1);
        messageEdit->setObjectName("messageEdit");

        horizontalLayout_2->addWidget(messageEdit);

        btSend = new QPushButton(layoutWidget1);
        btSend->setObjectName("btSend");

        horizontalLayout_2->addWidget(btSend);

        btLeave = new QPushButton(layoutWidget1);
        btLeave->setObjectName("btLeave");

        horizontalLayout_2->addWidget(btLeave);


        verticalLayout->addLayout(horizontalLayout_2);

        stackedWidget->addWidget(chatpage);
        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        stackedWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "\350\201\212\345\244\251\345\256\244\345\256\242\346\210\267\347\253\257", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "\345\210\230\350\202\262\346\243\256\347\232\204\350\201\212\345\244\251\345\256\244", nullptr));
        ipEdit->setText(QCoreApplication::translate("MainWindow", "127.0.0.1", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "\346\234\215\345\212\241\345\231\250\345\234\260\345\235\200", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "\346\230\265\347\247\260", nullptr));
        nameEdit->setText(QCoreApplication::translate("MainWindow", "\345\210\230\350\202\262\346\243\256", nullptr));
        btJoin->setText(QCoreApplication::translate("MainWindow", "\345\212\240\345\205\245", nullptr));
        btSend->setText(QCoreApplication::translate("MainWindow", "\345\217\221\351\200\201", nullptr));
        btLeave->setText(QCoreApplication::translate("MainWindow", "\351\200\200\345\207\272", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
