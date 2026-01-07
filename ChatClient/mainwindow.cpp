#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHostAddress>
#include <QJsonObject>
#include <QJsonValue>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentWidget(ui->loginpage);
    m_chatClient = new ChatClient(this);

    connect(m_chatClient, &ChatClient::connected, this, &MainWindow::connectedToServer);
    connect(m_chatClient, &ChatClient::jsonReceived, this, &MainWindow::jsonReceived);

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_btJoin_clicked()
{
    m_chatClient->connectToServer(QHostAddress(ui->ipEdit->text()), 1967);
}

void MainWindow::on_btSend_clicked()
{
    if (!ui->messageEdit->text().isEmpty()) {
        m_chatClient->sendMessage(ui->messageEdit->text(), "message");
        ui->messageEdit->clear();
    }
}

void MainWindow::connectedToServer()
{
    ui->stackedWidget->setCurrentWidget(ui->chatpage);
    m_chatClient->sendMessage(ui->nameEdit->text(), "login");
}

void MainWindow::messageReceived(const QString &sender, const QString &text)
{
    ui->textEdit->append(sender + ": " + text);
}

void MainWindow::jsonReceived(const QJsonObject &jsonObj)
{
    const QJsonValue typeVal = jsonObj.value("type");
    if (typeVal.isNull() || !typeVal.isString()) {
        return;
    }
    if (typeVal.toString().compare("message",Qt::CaseInsensitive) == 0) {
        const QJsonValue textVal = jsonObj.value("text");
        const QJsonValue senderVal = jsonObj.value("sender");
        if (textVal.isNull() || !textVal.isString()) {
            return;
        }
        if (senderVal.isNull() || !senderVal.isString()) {
            return;
        }

        messageReceived(senderVal.toString(), textVal.toString().trimmed());


    } else if (typeVal.toString().compare("newuser",Qt::CaseInsensitive) == 0) {
        const QJsonValue usernameVal = jsonObj.value("username");
        if (usernameVal.isNull() || !usernameVal.isString()) {
            return;
        }
        userJoined(usernameVal.toString());
    }else if (typeVal.toString().compare("userdisconnected",Qt::CaseInsensitive) == 0) {
        const QJsonValue usernameVal = jsonObj.value("username");
        if (usernameVal.isNull() || !usernameVal.isString()) {
            return;
        }
        userLeft(usernameVal.toString());
    }else if (typeVal.toString().compare("userlist", Qt::CaseInsensitive) == 0) { // user list
        const QJsonValue userlistVal = jsonObj.value("userlist");
        if (userlistVal.isNull() || !userlistVal.isArray())
            return;

        qDebug() << userlistVal.toVariant().toStringList();
        userListReceived(userlistVal.toVariant().toStringList());
    }
}

void MainWindow::userJoined(const QString &username)
{
    ui->userList->addItem(username);
}

void MainWindow::userLeft(const QString &username)
{
    for(auto aItem : ui->userList->findItems(username,Qt::MatchExactly)){
        qDebug("remove");
        ui->userList->removeItemWidget(aItem);
        delete aItem;
    }
}

void MainWindow::userListReceived(const QStringList &list)
{
    ui->userList->clear();
    ui->userList->addItems(list);
}

void MainWindow::on_btLeave_clicked()
{
    m_chatClient->disconnectFromHost();
    ui->stackedWidget->setCurrentWidget(ui->loginpage);
    m_chatClient->sendMessage(ui->nameEdit->text(), "logout");
    for(auto aItem : ui->userList->findItems(ui->nameEdit->text(),Qt::MatchExactly)){
        qDebug("remove");
        ui->userList->removeItemWidget(aItem);
        delete aItem;
    }
}
