#include "SyaringanWidget.h"
#include "ui_SyaringanWidget.h"
#include "include/Everything.h"

#pragma execution_character_set("utf-8")

void SyaringanWidget::textChangedSlot(const QString &text)
{
    //检查前缀
    CommandType type = Config::parseCommandType(text);

    if (type == QUERY_LOCAL)
    {   //本地查找只要文本变化就进行查找
        Command cmd(type, text);
        m_workerThread.pushCommand(cmd);
    }
}

void SyaringanWidget::showQueryResult(QList<FileInfo> result)
{
    if (result.size() <= 0)
    {   //如果没有搜索结果, 则断开信号连接, 删除List控件, 恢复窗体大小
        if (m_pResultList != NULL)
        {
            disconnect(m_pResultList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(doubleClickedResultItem(QListWidgetItem*)));
            ui->verticalLayout->removeWidget(m_pResultList);
            delete m_pResultList;
            m_pResultList = NULL;

            this->setMinimumSize(m_winWidth, m_winHeight);
            this->setMaximumSize(m_winWidth, m_winHeight);
            return;
        }
    }
    //有搜索结果, 准备进行显示
    if (m_pResultList == NULL)
        m_pResultList = new QListWidget();
    if (m_pLayout == NULL)
        m_pLayout = new QVBoxLayout;

    m_pResultList->clear();
    for (int i = 0; i < result.size(); ++i)
    {
        QString path = result.at(i).path;
        QString filename = result.at(i).filename;
        QIcon icon;
        if (result.at(i).type == RESULT_FOLDER)
        {
            icon = QIcon(":/icon/res/folder.ico");
        }
        else if (result.at(i).type == RESULT_FILE)
        {
            icon = QIcon(":/icon/res/file.ico");
        }
        else
        {
            int temp = result.at(i).type;
            qDebug() << "result.at(i).type = " << temp;
            icon = QIcon(":/icon/res/unknown.ico");
        }
        QListWidgetItem* item = new QListWidgetItem(icon, path + QString("\\") + filename);
        m_pResultList->addItem(item);
    }
    this->setMinimumSize(m_winWidth, HEIGHT_MAX);
    this->setMaximumSize(m_winWidth, HEIGHT_MAX);
    showAtTop();
    ui->verticalLayout->addWidget(m_pResultList);
    //TODO 这里绑定的次数太多了, 导致一次双击触发多个信号
    connect(m_pResultList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(doubleClickedResultItem(QListWidgetItem*)));
    moveToDesignatedPoint();
}

SyaringanWidget::SyaringanWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SyaringanWidget)
{
    ui->setupUi(this);

    connect(ui->lineEditInput, SIGNAL(textChanged(QString)), this, SLOT(textChangedSlot(QString)));
    connect(&m_workerThread, SIGNAL(LocalFileSearchResult(QList<FileInfo>)), this, SLOT(showQueryResult(QList<FileInfo>)));

    showAtTop();
    moveToDesignatedPoint();

    //注册热键必须在setAttribute(Qt::WA_TranslucentBackground)之后, 否则会导致透明色变成黑色
    m_HotKeyShow = GlobalAddAtom(TEXT("showSyaringan")) - 0xC00;	//获得唯一ID()
    if (!RegisterHotKey((HWND)this->winId(), m_HotKeyShow, MOD_CONTROL, 0))
    {
        QMessageBox message(QMessageBox::Information, "警告", "注册全局热键失败");
        message.exec();
    }

    m_workerThread.start();

    //托盘相关
    m_pTrayIcon = new QSystemTrayIcon(this);
    //新建托盘要显示的icon
    QIcon icon = QIcon(":/icon/res/syaringan.ico");
    //将icon设到QSystemTrayIcon对象中
    m_pTrayIcon->setToolTip(QObject::trUtf8("Search Everything"));
    m_pTrayIcon->setIcon(icon);
    m_pTrayIcon->show();
    createMenu();
    //绑定信号
    connect(m_pTrayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason)));

    m_pResultList = NULL;
    m_winHeight = this->height();
    m_winWidth = this->width();
}

SyaringanWidget::~SyaringanWidget()
{
    delete ui;
    UnregisterHotKey((HWND)this->winId(), m_HotKeyShow);
    GlobalDeleteAtom(m_HotKeyShow);
    delete m_pShowMainAction;
    delete m_pExitAppAction;
    delete m_pMenu;
    delete m_pTrayIcon;
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
                        ui->lineEditInput->clear();
                    }
                    else
                    {
                        showAtTop();
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

void SyaringanWidget::showAtTop()
{
    //背景透明
    this->setAttribute(Qt::WA_TranslucentBackground);
    //设置无边框 和 不在任务栏显示icon
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    //设置窗口置顶
    ::SetWindowPos(HWND(this->winId()), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    ::SetWindowPos(HWND(this->winId()), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    this->show();
    this->activateWindow();
}

void SyaringanWidget::showConfigForm()
{
    m_configForm.show();
}

void SyaringanWidget::moveToDesignatedPoint()
{
    static int showX = -1;
    static int showY = -1;
    if (showX < 0 || showY < 0)
    {
        //将屏幕设置到指定位置
        int screenWidth = GetSystemMetrics(SM_CXSCREEN);
        int screenHeight = GetSystemMetrics(SM_CYSCREEN);
        showY = HEIGHT_RATE * screenHeight;
        showX = 0;
        if (screenWidth > this->width())
        {
            showX = (screenWidth - this->width()) / 2;
        }
    }
    //qDebug() << "showX = " << showX << " showY = " << showY;
    this->move(showX, showY);
}

void SyaringanWidget::exitSlot()
{
    if (m_pTrayIcon != NULL)
        delete m_pTrayIcon;
    exit(0);
}

void SyaringanWidget::createMenu()
{
    m_pShowMainAction = new QAction(QObject::trUtf8("显示主界面"), this);
    connect(m_pShowMainAction,SIGNAL(triggered()),this,SLOT(showAtTop()));

    m_pShowConfigForm = new QAction(QObject::trUtf8("设置"), this);
    connect(m_pShowConfigForm,SIGNAL(triggered()),this,SLOT(showConfigForm()));

    m_pExitAppAction = new QAction(QObject::trUtf8("退出"), this);
    connect(m_pExitAppAction,SIGNAL(triggered()),this,SLOT(exitSlot()));

    m_pMenu = new QMenu(this);
    //新增菜单项---显示主界面
    m_pMenu->addAction(m_pShowMainAction);
    //新增菜单项---显示设置界面
    m_pMenu->addAction(m_pShowConfigForm);
    //增加分隔符
    m_pMenu->addSeparator();
    //新增菜单项---退出程序
    m_pMenu->addAction(m_pExitAppAction);
    //把QMenu赋给QSystemTrayIcon对象
    m_pTrayIcon->setContextMenu(m_pMenu);
}

void SyaringanWidget::doubleClickedResultItem(QListWidgetItem *item)
{
    QString text = item->text();
    qDebug() << "double clicked " << text;
    if (text.endsWith("exe", Qt::CaseInsensitive))
    {   //可执行程序
        WinExec(LPCSTR(text.toLatin1()), SW_SHOW);
    }
    else
    {   //其他文件, 使用explorer可能导致报毒
        QString cmd = QString("explorer ") + text;
        qDebug() << cmd;
        QProcess::execute(cmd);
    }
}

void SyaringanWidget::on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
    case QSystemTrayIcon::Trigger:
        //单击托盘图标
        break;
    case QSystemTrayIcon::DoubleClick:
        //双击托盘图标后显示主程序窗口
        showAtTop();
        break;
    default:
        break;
    }
}

void SyaringanWidget::on_lineEditInput_returnPressed()
{
    QString text = ui->lineEditInput->text();

    //检查前缀
    CommandType type = Config::parseCommandType(text);

    if (type == QUERY_LOCAL)
    {   //本地查找只要文本变化就进行查找
        Command cmd(type, text);
        m_workerThread.pushCommand(cmd);
    }
    else if (type == QUERY_NET)
    {
        Command cmd(type, text);
        m_workerThread.pushCommand(cmd);
    }
}
