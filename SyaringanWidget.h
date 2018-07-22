#ifndef SYARINGANWIDGET_H
#define SYARINGANWIDGET_H

#include <windows.h>
#include <QWidget>
#include <QDebug>
#include <QMessageBox>
#include <QTime>
#include <QList>
#include <QString>
#include <QListWidget>
#include <QSystemTrayIcon>
#include <QListWidgetItem>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QProcess>
#include <QVBoxLayout>
#include "WorkerThread.h"

namespace Ui {
class SyaringanWidget;
}

//程序界面上端距离屏幕顶端 / 屏幕总高度
#define HEIGHT_RATE (1.0 / 5)
//程序允许的最大高度
#define HEIGHT_MAX (400)

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

    void createMenu();
    void moveToDesignatedPoint();   //将屏幕移动到指定位置

private slots:
    void textChangedSlot(const QString &text);      //输入框文本变更
    void showQueryResult(QList<FileInfo> result);   //显示搜索结果
    void doubleClickedResultItem(QListWidgetItem *item);  //双击某条目
    void on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason);
    void showAtTop();   //让窗口显示到最前端

    void exitSlot();    //退出程序
private:
    WorkerThread m_workerThread;
    QListWidget* m_pResultList; //结果列表
    QVBoxLayout* m_pLayout;     //布局
    QSystemTrayIcon* m_pTrayIcon; //系统托盘
    QAction* m_pShowMainAction;
    QAction* m_pExitAppAction;
    QMenu* m_pMenu;

    int m_winHeight;    //窗口原始宽度
    int m_winWidth;     //窗口原始宽度
    QRect rect;
};

#endif // SYARINGANWIDGET_H
