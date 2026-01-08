#ifndef CHATDBMANAGER_H
#define CHATDBMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMutex>
#include <QDateTime>

class ChatDbManager : public QObject
{
    Q_OBJECT
public:
    static ChatDbManager& getInstance();

    bool connectDb();
    void initTables();
    int insertUser(const QString &nickname, const QString &ip, int port);
    void updateUserLogout(int userId);
    void insertMessage(int senderId, int receiverId, const QString &type, const QString &content);
    bool exportRecords(const QString &filePath);
    bool clearRecords();
    
    // Helper to get userId by nickname if needed, or we can store userId in ServerWorker
    int getUserId(const QString &nickname);

private:
    ChatDbManager();
    ~ChatDbManager();
    ChatDbManager(const ChatDbManager&) = delete;
    ChatDbManager& operator=(const ChatDbManager&) = delete;

    QSqlDatabase m_db;
};

#endif // CHATDBMANAGER_H
