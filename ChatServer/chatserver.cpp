#include "chatserver.h"
#include "serverworker.h"
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonArray>
#include <QThreadPool>
#include <QThread>

ChatServer::ChatServer(QObject *parent) : QTcpServer(parent)
{
}

void ChatServer::incomingConnection(qintptr socketDescriptor)
{
    ServerWorker *worker = new ServerWorker();
    if (!worker->setServerSocketDescriptor(socketDescriptor))
    {
        worker->deleteLater();
        return;
    }

    connect(worker, &ServerWorker::logMessage, this, &ChatServer::logMessage, Qt::QueuedConnection);
    connect(worker, &ServerWorker::jsonReceived, this, &ChatServer::jsonReceived, Qt::QueuedConnection);
    connect(worker, &ServerWorker::disconnectedFromClient, this,
            std::bind(&ChatServer::userDisconnected, this, worker), Qt::QueuedConnection);

    m_clients.append(worker);
    emit logMessage("新的用户加入聊天室");

    // 将工作任务提交到线程池执行
    QThreadPool::globalInstance()->start(worker);
}

void ChatServer::broadcast(const QJsonObject &message, ServerWorker *exclude)
{
    for (ServerWorker *worker : m_clients)
    {
        if (worker != exclude)
        {
            worker->sendJson(message);
        }
    }
}

void ChatServer::stopServer()
{
    close();
}

void ChatServer::jsonReceived(ServerWorker *sender, const QJsonObject &docObj)
{
    const QJsonValue typeVal = docObj.value("type");
    if (typeVal.isNull() || !typeVal.isString())
    {
        return;
    }
    if (typeVal.toString().compare("message", Qt::CaseInsensitive) == 0)
    {
        const QJsonValue textVal = docObj.value("text");
        if (textVal.isNull() || !textVal.isString())
        {
            return;
        }
        const QString text = textVal.toString().trimmed();
        if (text.isEmpty())
        {
            return;
        }
        QJsonObject message;
        message["type"] = "message";
        message["text"] = text;
        message["sender"] = sender->userName();

        const QJsonValue targetVal = docObj.value("target");
        if (!targetVal.isNull() && targetVal.isString())
        {
            QString targetUser = targetVal.toString();
            message["target"] = targetUser;
            // Find target worker
            for (ServerWorker *worker : m_clients)
            {
                if (worker->userName() == targetUser)
                {
                    worker->sendJson(message);
                    // Also send back to sender so they see their own private message
                    if (worker != sender)
                    {
                        sender->sendJson(message);
                    }
                    // 记录私聊消息
                    emit logMessage(QString("[私聊] %1 -> %2: %3").arg(sender->userName(), targetUser, text));
                    return;
                }
            }
            // If target not found, maybe send error back to sender?
            // For now, just ignore or maybe send back to sender only
        }
        else
        {
            broadcast(message, nullptr); // Broadcast to all clients including sender
            // 记录公聊消息
            emit logMessage(QString("[公聊] %1: %2").arg(sender->userName(), text));
        }
    }
    else if (typeVal.toString().compare("login", Qt::CaseInsensitive) == 0)
    {
        const QJsonValue usernameVal = docObj.value("user");
        if (usernameVal.isNull() || !usernameVal.isString())
        {
            return;
        }
        const QString username = usernameVal.toString().trimmed();
        if (username.isEmpty())
        {
            return;
        }
        sender->setUserName(usernameVal.toString());
        QJsonObject connectionMessage;
        connectionMessage["type"] = "newuser";
        connectionMessage["username"] = usernameVal.toString();

        broadcast(connectionMessage, sender);

        // send user list to new logined user
        QJsonObject userlistMessage;
        userlistMessage["type"] = "userlist";
        QJsonArray userlist;
        for (ServerWorker *worker : m_clients)
        {
            if (worker == sender)
                userlist.append(worker->userName() + "*");
            else
                userlist.append(worker->userName());
        }
        userlistMessage["userlist"] = userlist;
        sender->sendJson(userlistMessage);
    }
}

void ChatServer::userDisconnected(ServerWorker *sender)
{
    m_clients.removeAll(sender);
    const QString userName = sender->userName();
    if (!userName.isEmpty())
    {
        QJsonObject disconnectedMessage;
        disconnectedMessage["type"] = "userdisconnected";
        disconnectedMessage["username"] = userName;
        broadcast(disconnectedMessage, nullptr);
        emit logMessage(userName + " disconnected");
    }

    // 断开所有连接，然后删除对象
    sender->disconnectFromClient();
    sender->deleteLater();
}
