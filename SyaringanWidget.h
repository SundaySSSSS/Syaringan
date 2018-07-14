﻿#ifndef SYARINGANWIDGET_H
#define SYARINGANWIDGET_H

#include <windows.h>
#include <QWidget>
#include <QDebug>
#include <QMessageBox>
#include <QTime>
#include <QList>
#include <QString>
#include <QListWidget>
#include <QListWidgetItem>
#include "WorkerThread.h"

namespace Ui {
class SyaringanWidget;
}

class SyaringanWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SyaringanWidget(QWidget *parent = 0);
    ~SyaringanWidget();

private:
    Ui::SyaringanWidget *ui;
    ATOM m_HotKeyShow;
    QTime m_lastTime;   //上次点击Ctrl的时间
    bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;

private slots:
    void textChangedSlot(const QString &text);      //输入框文本变更
    void showQueryResult(QList<FileInfo> result);   //显示搜索结果
    void on_listWidgetResult_itemDoubleClicked(QListWidgetItem *item);  //双击某条目

private:
    WorkerThread m_workerThread;
};

#endif // SYARINGANWIDGET_H
