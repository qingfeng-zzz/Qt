#include "chatdbmanager.h"
#include <QDebug>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QFile>
#include <QTextStream>

ChatDbManager& ChatDbManager::getInstance()
{
    static ChatDbManager instance;
    return instance;
}

ChatDbManager::ChatDbManager()
{
}

ChatDbManager::~ChatDbManager()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
}

bool ChatDbManager::connectDb()
{
    if (QSqlDatabase::contains("qt_sql_default_connection")) {
        m_db = QSqlDatabase::database("qt_sql_default_connection");
    } else {
        m_db = QSqlDatabase::addDatabase("QSQLITE");
        m_db.setDatabaseName("./chat_room.db");
    }

    if (!m_db.open()) {
        qDebug() << "SQLite连接失败：" << m_db.lastError().text();
        return false;
    }

    // 开启外键支持
    QSqlQuery query;
    if (!query.exec("PRAGMA foreign_keys = ON;")) {
         qDebug() << "Failed to enable foreign keys:" << query.lastError().text();
    }
    
    initTables();
    return true;
}

void ChatDbManager::initTables()
{
    QSqlQuery query;
    
    // Create user_info table
    QString createUserInfo = R"(
        CREATE TABLE IF NOT EXISTS user_info ( 
            user_id INTEGER PRIMARY KEY AUTOINCREMENT, 
            nickname TEXT NOT NULL UNIQUE, 
            ip_address TEXT NOT NULL, 
            port INTEGER NOT NULL, 
            login_time DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP, 
            logout_time DATETIME NULL, 
            is_online INTEGER NOT NULL DEFAULT 1 
        );
    )";
    
    if (!query.exec(createUserInfo)) {
        qDebug() << "Create user_info table failed:" << query.lastError().text();
    }

    // Create chat_record table
    QString createChatRecord = R"(
        CREATE TABLE IF NOT EXISTS chat_record ( 
            record_id INTEGER PRIMARY KEY AUTOINCREMENT, 
            sender_id INTEGER NOT NULL, 
            receiver_id INTEGER NULL, 
            message_type TEXT NOT NULL, 
            message_content TEXT NOT NULL, 
            send_time DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP, 
            FOREIGN KEY (sender_id) REFERENCES user_info(user_id) ON DELETE CASCADE 
        );
    )";

    if (!query.exec(createChatRecord)) {
        qDebug() << "Create chat_record table failed:" << query.lastError().text();
    }
}

int ChatDbManager::insertUser(const QString &nickname, const QString &ip, int port)
{
    // First try to find existing user? 
    // The requirement says "insert record", but nickname is UNIQUE. 
    // If user exists, we might update login time and set is_online=1.
    // Or maybe the requirement implies a new record for every session? 
    // "nickname TEXT NOT NULL UNIQUE" implies one record per user.
    // Let's use UPSERT logic or check existence first.
    
    QSqlQuery query;
    query.prepare("SELECT user_id FROM user_info WHERE nickname = :nickname");
    query.bindValue(":nickname", nickname);
    if (query.exec() && query.next()) {
        int id = query.value(0).toInt();
        // Update status
        QSqlQuery updateQuery;
        updateQuery.prepare("UPDATE user_info SET is_online = 1, login_time = CURRENT_TIMESTAMP, logout_time = NULL, ip_address = :ip, port = :port WHERE user_id = :id");
        updateQuery.bindValue(":ip", ip);
        updateQuery.bindValue(":port", port);
        updateQuery.bindValue(":id", id);
        updateQuery.exec();
        return id;
    }

    query.prepare("INSERT INTO user_info (nickname, ip_address, port) VALUES (:nickname, :ip, :port)");
    query.bindValue(":nickname", nickname);
    query.bindValue(":ip", ip);
    query.bindValue(":port", port);
    
    if (query.exec()) {
        return query.lastInsertId().toInt();
    } else {
        qDebug() << "Insert user failed:" << query.lastError().text();
        return -1;
    }
}

void ChatDbManager::updateUserLogout(int userId)
{
    if (userId == -1) return;
    
    QSqlQuery query;
    query.prepare("UPDATE user_info SET is_online = 0, logout_time = CURRENT_TIMESTAMP WHERE user_id = :id");
    query.bindValue(":id", userId);
    
    if (!query.exec()) {
        qDebug() << "Update logout failed:" << query.lastError().text();
    }
}

void ChatDbManager::insertMessage(int senderId, int receiverId, const QString &type, const QString &content)
{
    if (senderId == -1) return;

    QSqlQuery query;
    query.prepare("INSERT INTO chat_record (sender_id, receiver_id, message_type, message_content) "
                  "VALUES (:senderId, :receiverId, :type, :content)");
    query.bindValue(":senderId", senderId);
    if (receiverId == -1) {
        query.bindValue(":receiverId", QVariant(QVariant::Int)); // NULL
    } else {
        query.bindValue(":receiverId", receiverId);
    }
    query.bindValue(":type", type);
    query.bindValue(":content", content);
    
    if (!query.exec()) {
        qDebug() << "Insert message failed:" << query.lastError().text();
    }
}

bool ChatDbManager::exportRecords(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream out(&file);
    // Write Header
    out << "Time,Type,Sender,Receiver,Content\n";

    QSqlQuery query("SELECT cr.send_time, cr.message_type, u1.nickname as sender, u2.nickname as receiver, cr.message_content "
                    "FROM chat_record cr "
                    "LEFT JOIN user_info u1 ON cr.sender_id = u1.user_id "
                    "LEFT JOIN user_info u2 ON cr.receiver_id = u2.user_id "
                    "ORDER BY cr.send_time ASC");
    
    while (query.next()) {
        QString time = query.value(0).toDateTime().toString("yyyy-MM-dd hh:mm:ss");
        QString type = query.value(1).toString();
        QString sender = query.value(2).toString();
        QString receiver = query.value(3).toString(); // might be empty/null
        QString content = query.value(4).toString();
        
        if (receiver.isEmpty()) receiver = "All";

        out << QString("%1,%2,%3,%4,%5\n").arg(time, type, sender, receiver, content);
    }

    file.close();
    return true;
}

bool ChatDbManager::clearRecords()
{
    QSqlQuery query;
    if (query.exec("DELETE FROM chat_record")) {
        // Optionally reset auto increment? Not strictly required but cleaner.
        query.exec("DELETE FROM sqlite_sequence WHERE name='chat_record'");
        return true;
    } else {
        qDebug() << "Clear records failed:" << query.lastError().text();
        return false;
    }
}

QSqlQuery ChatDbManager::getRecords(const QDateTime &startTime, const QDateTime &endTime, const QString &messageType)
{
    QSqlQuery query;
    QString sql = R"(
        SELECT cr.send_time, cr.message_type, u1.nickname as sender, u2.nickname as receiver, cr.message_content 
        FROM chat_record cr 
        LEFT JOIN user_info u1 ON cr.sender_id = u1.user_id 
        LEFT JOIN user_info u2 ON cr.receiver_id = u2.user_id 
        WHERE cr.send_time BETWEEN ? AND ?
    )";

    if (messageType != "全部消息") {
        sql += " AND cr.message_type = ?";
    }

    sql += " ORDER BY cr.send_time ASC";

    query.prepare(sql);
    query.addBindValue(startTime);
    query.addBindValue(endTime);
    
    if (messageType != "全部消息") {
        query.addBindValue(messageType);
    }

    if (!query.exec()) {
        qDebug() << "Get records failed:" << query.lastError().text();
    }

    return query;
}

int ChatDbManager::getUserId(const QString &nickname)
{
    QSqlQuery query;
    query.prepare("SELECT user_id FROM user_info WHERE nickname = :nickname");
    query.bindValue(":nickname", nickname);
    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    return -1;
}
