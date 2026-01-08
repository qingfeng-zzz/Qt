#include "messagerecorddialog.h"
#include "chatdbmanager.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

MessageRecordDialog::MessageRecordDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("聊天记录查看");
    setFixedSize(800, 600);
    setStyleSheet("background-color: #F5F5F5;");

    // 设置标题栏字体
    QFont titleFont;
    titleFont.setBold(true);
    titleFont.setPointSize(14);
    setFont(titleFont);

    initUI();
    loadRecords();
}

MessageRecordDialog::~MessageRecordDialog()
{
}

void MessageRecordDialog::initUI()
{
    // Main Layout
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(8);
    m_mainLayout->setContentsMargins(10, 10, 10, 10);

    // Button Style (matching existing client)
    QString btnStyle = "QPushButton { min-width: 80px; min-height: 30px; background-color: #E0E0E0; border: 1px solid #BDBDBD; } "
                       "QPushButton:hover { background-color: #BDBDBD; } "
                       "QPushButton:pressed { background-color: #9E9E9E; }";

    QString dangerBtnStyle = btnStyle + "QPushButton { background-color: #F56C6C; color: white; border: 1px solid #F56C6C; } "
                                        "QPushButton:hover { background-color: #E64A19; } "
                                        "QPushButton:pressed { background-color: #D84315; }";

    // Top Filter Section
    m_filterLayout = new QHBoxLayout();
    m_filterLayout->setSpacing(8);

    m_typeComboBox = new QComboBox();
    m_typeComboBox->addItems({"全部消息", "public", "private"});
    m_typeComboBox->setStyleSheet("QComboBox { background-color: white; border: 1px solid #BDBDBD; padding: 5px; }");

    m_startDateEdit = new QDateEdit(QDate::currentDate().addDays(-7));
    m_startDateEdit->setCalendarPopup(true);
    m_startDateEdit->setStyleSheet("QDateEdit { background-color: white; border: 1px solid #BDBDBD; padding: 5px; }");

    m_endDateEdit = new QDateEdit(QDate::currentDate());
    m_endDateEdit->setCalendarPopup(true);
    m_endDateEdit->setStyleSheet("QDateEdit { background-color: white; border: 1px solid #BDBDBD; padding: 5px; }");

    m_filterButton = new QPushButton("筛选");
    m_filterButton->setStyleSheet(btnStyle);

    m_filterLayout->addWidget(m_typeComboBox);
    m_filterLayout->addWidget(m_startDateEdit);
    m_filterLayout->addWidget(m_endDateEdit);
    m_filterLayout->addWidget(m_filterButton);
    m_filterLayout->addStretch();

    // Middle Display Section
    m_recordTextEdit = new QTextEdit();
    m_recordTextEdit->setObjectName("recordTextEdit");
    m_recordTextEdit->setReadOnly(true);
    m_recordTextEdit->setStyleSheet("QTextEdit { background-color: white; border: 1px solid #BDBDBD; padding: 8px; font-family: 'Microsoft YaHei'; font-size: 12px; }");

    // Bottom Action Section
    m_actionLayout = new QHBoxLayout();
    m_actionLayout->setSpacing(8);

    m_exportButton = new QPushButton("导出记录");
    m_exportButton->setStyleSheet(btnStyle);

    m_clearButton = new QPushButton("清空记录");
    m_clearButton->setStyleSheet(dangerBtnStyle);

    m_closeButton = new QPushButton("关闭");
    m_closeButton->setStyleSheet(btnStyle);

    m_actionLayout->addWidget(m_exportButton);
    m_actionLayout->addWidget(m_clearButton);
    m_actionLayout->addStretch();
    m_actionLayout->addWidget(m_closeButton);

    // Add sections to main layout
    m_mainLayout->addLayout(m_filterLayout);
    m_mainLayout->addWidget(m_recordTextEdit, 1); // Takes 80% height
    m_mainLayout->addLayout(m_actionLayout);

    // Connect signals
    connect(m_filterButton, &QPushButton::clicked, this, &MessageRecordDialog::onFilterButtonClicked);
    connect(m_exportButton, &QPushButton::clicked, this, &MessageRecordDialog::onExportButtonClicked);
    connect(m_clearButton, &QPushButton::clicked, this, &MessageRecordDialog::onClearButtonClicked);
    connect(m_closeButton, &QPushButton::clicked, this, &MessageRecordDialog::onCloseButtonClicked);
}

void MessageRecordDialog::loadRecords()
{
    ChatDbManager &dbManager = ChatDbManager::getInstance();
    if (!dbManager.connectDb())
    {
        m_recordTextEdit->setText("数据库连接失败，请检查数据库文件");
        return;
    }

    // 默认加载所有记录
    QDateTime startTime = m_startDateEdit->date().startOfDay();
    QDateTime endTime = m_endDateEdit->date().endOfDay();
    QString messageType = m_typeComboBox->currentText();

    QSqlQuery query = dbManager.getRecords(startTime, endTime, messageType);

    m_recordTextEdit->clear();

    while (query.next())
    {
        QString time = query.value(0).toDateTime().toString("yyyy-MM-dd hh:mm:ss");
        QString type = query.value(1).toString();
        QString sender = query.value(2).toString();
        QString receiver = query.value(3).toString();
        QString content = query.value(4).toString();

        QString formattedMessage = formatMessage(time, type, sender, receiver, content);
        m_recordTextEdit->append(formattedMessage);
    }

    if (m_recordTextEdit->toPlainText().isEmpty())
    {
        m_recordTextEdit->setText("没有找到符合条件的聊天记录");
    }
}

void MessageRecordDialog::onFilterButtonClicked()
{
    loadRecords();
}

void MessageRecordDialog::onExportButtonClicked()
{
    QString filePath = QFileDialog::getSaveFileName(this, "导出聊天记录", "chat_records.txt", "Text Files (*.txt);;All Files (*)");
    if (filePath.isEmpty())
    {
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, "导出失败", "无法打开文件进行写入");
        return;
    }

    QTextStream out(&file);
    out << "聊天记录导出\n";
    out << "====================================\n";
    out << m_recordTextEdit->toPlainText();
    out << "\n====================================\n";
    out << "导出时间：" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << "\n";

    file.close();

    QMessageBox::information(this, "导出成功", "聊天记录已成功导出到：\n" + filePath);
}

void MessageRecordDialog::onClearButtonClicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::warning(this, "确认清空", "确认清空数据库中所有聊天记录？此操作不可恢复。",
                                 QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes)
    {
        ChatDbManager &dbManager = ChatDbManager::getInstance();
        if (dbManager.connectDb() && dbManager.clearRecords())
        {
            m_recordTextEdit->clear();
            QMessageBox::information(this, "操作成功", "所有聊天记录已清空");
        }
        else
        {
            QMessageBox::warning(this, "操作失败", "清空聊天记录失败，请重试");
        }
    }
}

void MessageRecordDialog::onCloseButtonClicked()
{
    close();
}

QString MessageRecordDialog::formatMessage(const QString &time, const QString &type, const QString &sender, const QString &receiver, const QString &content)
{
    if (type == "public")
    {
        return QString("[%1] 【公共】 %2：%3").arg(time, sender, content);
    }
    else if (type == "private")
    {
        return QString("[%1] 【私聊】 %2 → %3：%4").arg(time, sender, receiver.isEmpty() ? "未知" : receiver, content);
    }
    else
    {
        return QString("[%1] 【%2】 %3：%4").arg(time, type, sender, content);
    }
}
