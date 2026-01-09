#include "serverworker.h"
#include <QDataStream>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDateTime>
#include <QHostAddress>
#include <QEventLoop>
#include <QThread>
ServerWorker::ServerWorker(QObject *parent) : QObject(parent), QRunnable()
{
    m_serverSocket = nullptr;
    m_socketDescriptor = 0;
    m_connectTime = QDateTime::currentSecsSinceEpoch();
    m_isRunning = false;
    setAutoDelete(false); // 不要自动删除，因为我们需要管理对象的生命周期
}

bool ServerWorker::setServerSocketDescriptor(qintptr socketDescriptor)
{
    m_socketDescriptor = socketDescriptor;
    return true;
}

void ServerWorker::run()
{
    if (m_serverSocket)
    {
        delete m_serverSocket;
    }

    m_serverSocket = new QTcpSocket();
    if (!m_serverSocket->setSocketDescriptor(m_socketDescriptor))
    {
        emit logMessage("Failed to set socket descriptor");
        emit disconnectedFromClient();
        return;
    }

    m_connectTime = QDateTime::currentSecsSinceEpoch();
    m_isRunning = true;

    connect(m_serverSocket, &QTcpSocket::readyRead, this, &ServerWorker::onReadyRead, Qt::DirectConnection);
    connect(m_serverSocket, &QTcpSocket::disconnected, this, &ServerWorker::disconnectedFromClient, Qt::DirectConnection);

    emit logMessage("Client connected - Thread: " + QString::number((quintptr)QThread::currentThreadId()));

    // 进入事件循环，处理网络事件
    QEventLoop eventLoop;
    connect(m_serverSocket, &QTcpSocket::disconnected, &eventLoop, &QEventLoop::quit);
    eventLoop.exec();

    m_isRunning = false;
    m_serverSocket->deleteLater();
    m_serverSocket = nullptr;
    emit disconnectedFromClient();
    emit logMessage("Client disconnected");
}

QString ServerWorker::userName()
{
    return m_userName;
}

void ServerWorker::setUserName(QString user)
{
    m_userName = user;
}

QString ServerWorker::userIp()
{
    return m_serverSocket->peerAddress().toString();
}

quint16 ServerWorker::userPort()
{
    return m_serverSocket->peerPort();
}

qint64 ServerWorker::connectionDuration() const
{
    return QDateTime::currentSecsSinceEpoch() - m_connectTime;
}

QTcpSocket *ServerWorker::serverSocket() const
{
    return m_serverSocket;
}

bool ServerWorker::isRunning() const
{
    return m_isRunning;
}

void ServerWorker::disconnectFromClient()
{
    if (m_serverSocket)
    {
        m_serverSocket->disconnectFromHost();
        m_serverSocket->waitForDisconnected(1000);
    }
}

void ServerWorker::onReadyRead()
{
    QByteArray jsonData;
    QDataStream socketStream(m_serverSocket);
    socketStream.setVersion(QDataStream::Qt_5_12);
    for (;;)
    {
        socketStream.startTransaction();
        socketStream >> jsonData;
        if (socketStream.commitTransaction())
        {
            emit logMessage(QString::fromUtf8(jsonData));
            // sendMessage("服务器已收到消息");

            QJsonParseError parseError;
            const QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);
            if (parseError.error == QJsonParseError::NoError)
            {
                if (jsonDoc.isObject())
                {
                    emit logMessage(QJsonDocument(jsonDoc).toJson(QJsonDocument::Compact));
                    emit jsonReceived(this, jsonDoc.object());
                }
            }
        }
        else
        {
            break;
        }
    }
}

void ServerWorker::sendMessage(const QString &text, const QString &type)
{
    if (m_serverSocket->state() != QAbstractSocket::ConnectedState)
    {
        return;
    }

    if (!text.isEmpty())
    {
        QDataStream serverStream(m_serverSocket);
        serverStream.setVersion(QDataStream::Qt_5_12);

        QJsonObject message;
        message["type"] = type;
        message["text"] = text;

        serverStream << QJsonDocument(message).toJson();
    }
}

void ServerWorker::sendJson(const QJsonObject &json)
{
    if (!m_serverSocket || m_serverSocket->state() != QAbstractSocket::ConnectedState)
    {
        emit logMessage("Failed to send json: socket not connected");
        return;
    }
    const QByteArray jsonData = QJsonDocument(json).toJson(QJsonDocument::Compact);
    emit logMessage(QLatin1String("Sending to ") + userName() + QLatin1String(" - ") + QString::fromUtf8(jsonData));
    QDataStream socketStream(m_serverSocket);
    socketStream.setVersion(QDataStream::Qt_5_12); // 使用与接收端相同的版本
    socketStream << jsonData;
    m_serverSocket->flush(); // 确保数据立即发送
}
