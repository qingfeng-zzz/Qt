#ifndef SERVERWORKER_H
#define SERVERWORKER_H

#include <QObject>
#include <QTcpSocket>
#include <QRunnable>

class ServerWorker : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit ServerWorker(QObject *parent = nullptr);
    virtual bool setServerSocketDescriptor(qintptr socketDescriptor);
    void run() override;

    QString userName();
    void setUserName(QString user);

    QString userIp();
    quint16 userPort();
    qint64 connectionDuration() const;
    QTcpSocket *serverSocket() const;
    bool isRunning() const;
    void disconnectFromClient();

signals:
    void logMessage(const QString &msg);
    void jsonReceived(ServerWorker *sender, const QJsonObject &docObj);
    void disconnectedFromClient();

public slots:
    void onReadyRead();
    void sendMessage(const QString &text, const QString &type = "message");
    void sendJson(const QJsonObject &json);

private:
    QTcpSocket *m_serverSocket;
    QString m_userName;
    qint64 m_connectTime; // Timestamp in seconds
    qintptr m_socketDescriptor;
    bool m_isRunning;
};

#endif // SERVERWORKER_H
