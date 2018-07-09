#ifndef SYARINGANWIDGET_H
#define SYARINGANWIDGET_H

#include <windows.h>
#include <QWidget>
#include <QDebug>
#include <QMessageBox>
#include <QTime>
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
    DWORD m_showItemMax;  //显示的搜索结果的上限
    bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;
    void QueryByEverything(const QString& text);

private slots:
    void textChangedSlot(const QString &text);
private:
    WorkerThread m_workerThread;
};

#endif // SYARINGANWIDGET_H
