#ifndef MESSAGERECORDDIALOG_H
#define MESSAGERECORDDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QDateEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QDateTime>

class MessageRecordDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MessageRecordDialog(QWidget *parent = nullptr);
    ~MessageRecordDialog();

private slots:
    void onFilterButtonClicked();
    void onExportButtonClicked();
    void onClearButtonClicked();
    void onCloseButtonClicked();

private:
    void initUI();
    void loadRecords();
    QString formatMessage(const QString &time, const QString &type, const QString &sender, const QString &receiver, const QString &content);

    // UI Components
    QVBoxLayout *m_mainLayout;
    
    // Top Filter Section
    QHBoxLayout *m_filterLayout;
    QComboBox *m_typeComboBox;
    QDateEdit *m_startDateEdit;
    QDateEdit *m_endDateEdit;
    QPushButton *m_filterButton;
    
    // Middle Display Section
    QTextEdit *m_recordTextEdit;
    
    // Bottom Action Section
    QHBoxLayout *m_actionLayout;
    QPushButton *m_exportButton;
    QPushButton *m_clearButton;
    QPushButton *m_closeButton;
};

#endif // MESSAGERECORDDIALOG_H
