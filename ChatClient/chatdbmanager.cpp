
bool ChatDbManager::exportRecords(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
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

    while (query.next())
    {
        QString time = query.value(0).toDateTime().toString("yyyy-MM-dd hh:mm:ss");
        QString type = query.value(1).toString();
        QString sender = query.value(2).toString();
        QString receiver = query.value(3).toString(); // might be empty/null
        QString content = query.value(4).toString();

        if (receiver.isEmpty())
            receiver = "All";

        out << QString("%1,%2,%3,%4,%5\n").arg(time, type, sender, receiver, content);
    }

    file.close();
    return true;
}

bool ChatDbManager::clearRecords()
{
    QSqlQuery query;
    if (query.exec("DELETE FROM chat_record"))
    {
        // Optionally reset auto increment? Not strictly required but cleaner.
        query.exec("DELETE FROM sqlite_sequence WHERE name='chat_record'");
        return true;
    }
    else
    {
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

    if (messageType != "全部消息")
    {
        sql += " AND cr.message_type = ?";
    }

    sql += " ORDER BY cr.send_time ASC";

    query.prepare(sql);
    query.addBindValue(startTime);
    query.addBindValue(endTime);

    if (messageType != "全部消息")
    {
        query.addBindValue(messageType);
    }

    if (!query.exec())
    {
        qDebug() << "Get records failed:" << query.lastError().text();
    }

    return query;
}

int ChatDbManager::getUserId(const QString &nickname)
{
    QSqlQuery query;
    query.prepare("SELECT user_id FROM user_info WHERE nickname = :nickname");
    query.bindValue(":nickname", nickname);
    if (query.exec() && query.next())
    {
        return query.value(0).toInt();
    }
    return -1;
}
