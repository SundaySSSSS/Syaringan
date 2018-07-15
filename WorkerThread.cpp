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
            qDebug() << "valid search cmd";
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
        item.filename = QString::fromStdWString(Everything_GetResultFileName(i));
        item.path = QString::fromStdWString(Everything_GetResultPath(i));
        result.append(item);
    }

    emit LocalFileSearchResult(result);
}
