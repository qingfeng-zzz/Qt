#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHostAddress>
#include <QJsonObject>
#include <QJsonValue>
#include <QMessageBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QDateTime>
#include <QScrollBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->label->setText("匿名多人聊天室"); // Update Title
    ui->stackedWidget->setCurrentWidget(ui->loginpage);
    m_chatClient = new ChatClient(this);

    connect(m_chatClient, &ChatClient::connected, this, &MainWindow::connectedToServer);
    connect(m_chatClient, &ChatClient::jsonReceived, this, &MainWindow::jsonReceived);

    // --- UI Modifications ---

    // 1. User List Widget
    // Rename/Setup existing userList
    ui->userList->setFixedWidth(200);
    ui->userList->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->userList->setStyleSheet("QListWidget::item:selected { background-color: #E6F7FF; border: 1px solid #409EFF; color: black; }");

    // 2. Chat Text Edit
    ui->textEdit->setReadOnly(true);
    QFont font("Microsoft YaHei", 12);
    ui->textEdit->setFont(font);
    // Auto word wrap is default for QTextEdit
    // Scroll bar is default

    // 3. Radio Buttons
    radioPublic = new QRadioButton("公共", this);
    radioPrivate = new QRadioButton("私聊", this);
    radioPublic->setChecked(true); // Default Public

    radioGroup = new QButtonGroup(this);
    radioGroup->addButton(radioPublic);
    radioGroup->addButton(radioPrivate);

    // Insert into layout (horizontalLayout_2)
    // Layout items: messageEdit (0), btSend (1), btLeave (2) -> after insertion indices shift
    ui->horizontalLayout_2->insertWidget(1, radioPublic);
    ui->horizontalLayout_2->insertWidget(2, radioPrivate);

    // 4. Button Styles
    QString btnStyle = "QPushButton { min-width: 80px; min-height: 30px; }";
    ui->btJoin->setStyleSheet(btnStyle + "background-color: #67C23A; color: white;");
    ui->btSend->setStyleSheet(btnStyle);
    ui->btLeave->setStyleSheet(btnStyle + "background-color: #F56C6C; color: white;");
    ui->horizontalLayout_2->setSpacing(8);

    // Disable Send initially
    ui->btSend->setEnabled(false);

    // Connections
    connect(ui->userList, &QListWidget::itemClicked, this, [this](QListWidgetItem *item){
        radioPrivate->setChecked(true);
        ui->messageEdit->setPlaceholderText(QString("私聊@%1：").arg(item->text()));
    });

    connect(radioPublic, &QRadioButton::toggled, this, [this](bool checked){
        if(checked) {
            ui->messageEdit->setPlaceholderText("");
            ui->userList->clearSelection();
        }
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_btJoin_clicked()
{
    QString nickname = ui->nameEdit->text().trimmed();
    if(nickname.isEmpty()) {
        QMessageBox::warning(this, "警告", "昵称不能为空！");
        return;
    }
    m_chatClient->connectToServer(QHostAddress(ui->ipEdit->text()), 1967);
}

void MainWindow::on_btSend_clicked()
{
    QString text = ui->messageEdit->text();
    if (text.isEmpty()) return;

    if (radioPrivate->isChecked()) {
        QList<QListWidgetItem*> selected = ui->userList->selectedItems();
        if (selected.isEmpty()) {
             QMessageBox::warning(this, "提示", "请选择私聊对象！");
             return;
        }
        QString target = selected.first()->text();
        m_chatClient->sendMessage(text, "message", target);
    } else {
        m_chatClient->sendMessage(text, "message");
    }
    ui->messageEdit->clear()；
}

void MainWindow::connectedToServer()
{
    ui->stackedWidget->setCurrentWidget(ui->chatpage);
    m_chatClient->sendMessage(ui->nameEdit->text(), "login");
    ui->btSend->setEnabled(true);
}

void MainWindow::messageReceived(const QString &sender, const QString &text, const QString &target)
{
    QString timeStr = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString displayHtml;

    if (!target.isEmpty()) {
        // Private Message
        QString relation;
        QString myName = ui->nameEdit->text();
        if (sender == myName) {
            relation = QString("我→%1").arg(target);
        } else {
            relation = QString("%1→我").arg(sender);
        }

        displayHtml = QString("<font color='#409EFF'>[私聊][%1] %2：%3</font>")
                      .arg(timeStr, relation, text);
    } else {
        // Public Message
        displayHtml = QString("<font color='black'>[公共][%1] %2：%3</font>")
                      .arg(timeStr, sender, text);
    }

    ui->textEdit->append(displayHtml);
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

        const QJsonValue targetVal = jsonObj.value("target");
        QString target = "";
        if (!targetVal.isNull() && targetVal.isString()) {
            target = targetVal.toString();
        }

        messageReceived(senderVal.toString(), textVal.toString().trimmed(), target);


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
    if (m_chatClient) {
        m_chatClient->sendMessage(ui->nameEdit->text(), "logout");
        m_chatClient->disconnectFromHost();
    }

    ui->stackedWidget->setCurrentWidget(ui->loginpage);

    ui->userList->clear();
    ui->textEdit->clear();
    ui->btSend->setEnabled(false);
    radioPublic->setChecked(true);
}
