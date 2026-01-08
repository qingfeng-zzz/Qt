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
