#include "serverworker.h"
#include <QDataStream>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDateTime>
#include <QHostAddress>
#include <QThread>

ServerWorker::ServerWorker(QObject *parent) : QObject(parent)
{
    m_serverSocket = nullptr;
    m_socketDescriptor = 0;
    m_connectTime = QDateTime::currentSecsSinceEpoch();
    m_isRunning = false;
    m_thread = new QThread(this);
    this->moveToThread(m_thread);

    connect(m_thread, &QThread::started, this, &ServerWorker::startWork);
    connect(m_thread, &QThread::finished, m_thread, &QThread::deleteLater);
    connect(this, &ServerWorker::sendJsonRequested, this, &ServerWorker::sendJson, Qt::QueuedConnection);
    connect(this, &ServerWorker::disconnectRequested, this, &ServerWorker::disconnectFromClient, Qt::QueuedConnection);
}

bool ServerWorker::setServerSocketDescriptor(qintptr socketDescriptor)
{
    m_socketDescriptor = socketDescriptor;
    return true;
}

void ServerWorker::start()
{
    m_thread->start();
}

void ServerWorker::startWork()
{
    if (m_serverSocket)
    {
        delete m_serverSocket;
    }

    // 在当前线程中创建socket
    m_serverSocket = new QTcpSocket(this);
    if (!m_serverSocket->setSocketDescriptor(m_socketDescriptor))
    {
        emit logMessage("Failed to set socket descriptor");
        emit disconnectedFromClient();
        return;
    }

    m_connectTime = QDateTime::currentSecsSinceEpoch();
    m_isRunning = true;

    // 使用QueuedConnection确保信号在同一线程中处理
    connect(m_serverSocket, &QTcpSocket::readyRead, this, &ServerWorker::onReadyRead, Qt::QueuedConnection);
    connect(m_serverSocket, &QTcpSocket::disconnected, this, &ServerWorker::disconnectedFromClient, Qt::QueuedConnection);

    emit logMessage("Client connected - Thread: " + QString::number((quintptr)QThread::currentThreadId()));
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
        emit logMessage(QString("Failed to send json: socket not connected or null"));
        return;
    }
    const QByteArray jsonData = QJsonDocument(json).toJson(QJsonDocument::Compact);
    emit logMessage(QLatin1String("Sending to ") + userName() + QLatin1String(" - ") + QString::fromUtf8(jsonData));
    QDataStream socketStream(m_serverSocket);
    socketStream.setVersion(QDataStream::Qt_5_12);
    socketStream << jsonData;
}
