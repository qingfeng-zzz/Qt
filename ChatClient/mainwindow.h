#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "chatclient.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btJoin_clicked();

    void on_btSend_clicked();

    void on_btLeave_clicked();

    void connectedToServer();
    void messageReceived(const QString &sender, const QString &text);
    void jsonReceived(const QJsonObject &jsonObj);
    void userJoined(const QString &username);
    void userLeft(const QString &username);
    void userListReceived(const QStringList &list);

private:
    Ui::MainWindow *ui;

    ChatClient *m_chatClient;
};
#endif // MAINWINDOW_H
