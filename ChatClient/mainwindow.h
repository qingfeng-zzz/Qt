#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "chatclient.h"
#include "chatdbmanager.h"

class QRadioButton;
class QButtonGroup;

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
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
    void messageReceived(const QString &sender, const QString &text, const QString &target = "");
    void jsonReceived(const QJsonObject &jsonObj);
    void userJoined(const QString &username);
    void userLeft(const QString &username);
    void userListReceived(const QStringList &list);

private:
    Ui::MainWindow *ui;
    ChatClient *m_chatClient;
    QRadioButton *radioPublic;
    QRadioButton *radioPrivate;
    QButtonGroup *radioGroup;
    int m_userId; // 用户ID，用于数据库操作
};
#endif // MAINWINDOW_H
