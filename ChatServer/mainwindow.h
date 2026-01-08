#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QLabel>
#include <QTimer>
#include "chatserver.h"

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
    void on_startStopButton_clicked();
    void updateServerStatus();
    void on_exportButton_clicked();
    void on_clearButton_clicked();

public slots:
    void logMessage(const QString &msg);

private:
    Ui::MainWindow *ui;
    ChatServer * m_chatServer;
    
    QListWidget *clientListWidget;
    QLabel *coreThreadLbl;
    QLabel *activeThreadLbl;
    QLabel *taskQueueLbl;
    QTimer *statusTimer;
};
#endif // MAINWINDOW_H
