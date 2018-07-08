#include "SyaringanWidget.h"
#include "ui_SyaringanWidget.h"
#include "include/Everything.h"

#pragma execution_character_set("utf-8")

void SyaringanWidget::QueryByEverything(const QString& text)
{
    Everything_SetSort(EVERYTHING_SORT_NAME_ASCENDING);
    Everything_SetSearchW(reinterpret_cast<const wchar_t *>(text.utf16()));
    Everything_QueryW(TRUE);
    {
        ui->listWidgetResult->clear();
        for(DWORD i=0;i < Everything_GetNumResults();i++)
        {
            if (i >= m_showItemMax)
                break;
            QString item = QString::fromStdWString(Everything_GetResultFileName(i));
            //qDebug() << QString::fromStdWString(Everything_GetResultFileName(i));
            ui->listWidgetResult->addItem(item);
        }
    }
}

void SyaringanWidget::textChangedSlot(const QString &text)
{
    //qDebug() << "text changed";
    QueryByEverything(text);
}

SyaringanWidget::SyaringanWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SyaringanWidget)
{
    ui->setupUi(this);
    m_HotKeyShow = GlobalAddAtom(TEXT("showSyaringan")) - 0xC00;	//获得唯一ID()
    if (!RegisterHotKey((HWND)this->winId(), m_HotKeyShow, MOD_CONTROL, 0))
    {
        QMessageBox message(QMessageBox::Information, "警告", "注册全局热键失败");
        message.exec();
    }
    connect(ui->lineEditInput, SIGNAL(textChanged(QString)), this, SLOT(textChangedSlot(QString)));
    m_showItemMax = 10;
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
                    }
                    else
                    {
                        //qDebug() << "show...";
                        this->setWindowFlags(Qt::WindowStaysOnBottomHint);
                        this->show();
                    }
                }
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
