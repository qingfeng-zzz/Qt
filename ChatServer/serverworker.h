#ifndef SERVERWORKER_H
#define SERVERWORKER_H

#include <QObject>
#include <QTcpSocket>
#include <QThread>

class ServerWorker : public QObject
{
    Q_OBJECT
public:
    explicit ServerWorker(QObject *parent = nullptr);
    virtual bool setServerSocketDescriptor(qintptr socketDescriptor);

    void start();

    QString userName();
    void setUserName(QString user);

    QString userIp();
    quint16 userPort();
    qint64 connectionDuration() const;
    QTcpSocket *serverSocket() const;
    bool isRunning() const;

signals:
    void logMessage(const QString &msg);
    void jsonReceived(ServerWorker *sender, const QJsonObject &docObj);
    void disconnectedFromClient();
    void sendJsonRequested(const QJsonObject &json);
    void disconnectRequested();

public slots:
    void sendMessage(const QString &text, const QString &type = "message");

private slots:
    void onReadyRead();
    void startWork();
    void sendJson(const QJsonObject &json);
    void disconnectFromClient();

private:
    QTcpSocket *m_serverSocket;
    QString m_userName;
    qint64 m_connectTime; // Timestamp in seconds
    qintptr m_socketDescriptor;
    bool m_isRunning;
    QThread *m_thread;
};

#endif // SERVERWORKER_H
