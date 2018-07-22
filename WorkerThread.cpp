#include <QMetaType>
#include "WorkerThread.h"
#include "include/Everything.h"

WorkerThread::WorkerThread()
{
    qRegisterMetaType<QList<QString> > ("QList<FileInfo>");
}

void WorkerThread::pushCommand(Command cmd)
{
    m_mutex.lock();
    m_commandQueue.enqueue(cmd);
    m_mutex.unlock();
}

Command WorkerThread::popCommand()
{
    Command cmd = Command::createInvalidCmd();
    m_mutex.lock();
    if (!m_commandQueue.isEmpty())
    {
        cmd = m_commandQueue.dequeue();
    }
    m_mutex.unlock();
    return cmd;
}

void WorkerThread::run()
{
    Command cmd;
    while (true)
    {
        cmd = popCommand();
        if (cmd.isValid())
        {
            //qDebug() << "valid search cmd";
            if (cmd.getType() == QUERY_LOCAL)
                queryLocalFile(cmd.getContent());
        }
        Sleep(10);
    }
}

void WorkerThread::queryLocalFile(QString search_content)
{
    QList<FileInfo> result;

    if (search_content.size() == 0)
    {
        emit LocalFileSearchResult(result);
        return;
    }

    Everything_SetSort(EVERYTHING_SORT_NAME_ASCENDING);
    Everything_SetSearchW(reinterpret_cast<const wchar_t *>(search_content.utf16()));
    Everything_QueryW(TRUE);

    for(DWORD i=0;i < Everything_GetNumResults();i++)
    {
        if (i >= 25)
            break;

        FileInfo item;
        if (Everything_IsFolderResult(i))
        {   //结构为文件夹
            item.type = RESULT_FOLDER;
            item.filename = QString("");
            //目录获取的path不全, 加上filename才是完整的目录路径
            item.path = QString::fromStdWString(Everything_GetResultPath(i)) + "\\" +
                    QString::fromStdWString(Everything_GetResultFileName(i));
        }
        else if (Everything_IsFileResult(i))
        {   //结果为文件
            item.type = RESULT_FILE;
            item.filename = QString::fromStdWString(Everything_GetResultFileName(i));
            item.path = QString::fromStdWString(Everything_GetResultPath(i));
        }
        else
        {
            item.type = RESULT_UNKNOWN;
            item.filename = QString("");
            item.path = QString("");
        }
        result.append(item);
    }

    emit LocalFileSearchResult(result);
}
