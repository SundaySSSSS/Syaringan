﻿#include "SyaringanWidget.h"
#include "ui_SyaringanWidget.h"
#include "include/Everything.h"

#pragma execution_character_set("utf-8")

void SyaringanWidget::textChangedSlot(const QString &text)
{
    Command cmd(QUERY_LOCAL, text);
    m_workerThread.pushCommand(cmd);
}

void SyaringanWidget::showQueryResult(QList<QString> result)
{
    qDebug() << "show Query Result";
    ui->listWidgetResult->clear();
    for (int i = 0; i < result.size(); ++i)
    {
        ui->listWidgetResult->addItem(result.at(i));
    }
}

SyaringanWidget::SyaringanWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SyaringanWidget)
{
    ui->setupUi(this);

    connect(ui->lineEditInput, SIGNAL(textChanged(QString)), this, SLOT(textChangedSlot(QString)));
    connect(&m_workerThread, SIGNAL(LocalFileSearchResult(QList<QString>)), this, SLOT(showQueryResult(QList<QString>)));
    //背景透明
    this->setAttribute(Qt::WA_TranslucentBackground);
    //设置无边框
    this->setWindowFlags(Qt::FramelessWindowHint);

    //注册热键必须在setAttribute(Qt::WA_TranslucentBackground)之后, 否则会导致透明色变成黑色
    m_HotKeyShow = GlobalAddAtom(TEXT("showSyaringan")) - 0xC00;	//获得唯一ID()
    if (!RegisterHotKey((HWND)this->winId(), m_HotKeyShow, MOD_CONTROL, 0))
    {
        QMessageBox message(QMessageBox::Information, "警告", "注册全局热键失败");
        message.exec();
    }

    m_workerThread.start();
}

SyaringanWidget::~SyaringanWidget()
{
    delete ui;
    UnregisterHotKey((HWND)this->winId(), m_HotKeyShow);
    GlobalDeleteAtom(m_HotKeyShow);
}

bool SyaringanWidget::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    if (eventType == "windows_generic_MSG")
    {
        MSG *msg = (MSG *)message;
        if (msg->message == WM_HOTKEY)
        {   //通过双击两次Ctrl来启动或隐藏窗体
            if (msg->wParam == m_HotKeyShow)
            {
                //qDebug() << "hot key pressed";
                QTime time_now = QTime::currentTime();
                //两次点击Ctrl在500毫秒内认为是双击
                if (time_now < m_lastTime.addMSecs(500))
                {
                    if (this->isVisible())
                    {
                        //qDebug() << "hide...";
                        this->setWindowFlags(NULL);
                        this->hide();
                        ui->listWidgetResult->clear();
                    }
                    else
                    {
                        //设置无边框
                        this->setWindowFlags(Qt::FramelessWindowHint);
                        //设置窗口置顶
                        ::SetWindowPos(HWND(this->winId()), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
                        ::SetWindowPos(HWND(this->winId()), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
                        this->show();
                        this->activateWindow();
                    }
                    m_lastTime = QTime();    //让时间初始化, 使下一次Ctrl的点击不会认为双击
                }
                else
                    m_lastTime = time_now;
            }
            return true;
        }
        else
            return QWidget::nativeEvent(eventType, message, result);
    }
    else
        return false;
}
