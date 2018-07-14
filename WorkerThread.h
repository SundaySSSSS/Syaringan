#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H
#include <QThread>
#include <QDebug>
#include <QQueue>
#include <QMutex>
#include <QList>
#include "command.h"

typedef struct _FileInfo
{
    QString path;
    QString filename;
}
FileInfo;

class WorkerThread : public QThread
{
    Q_OBJECT
public:
    WorkerThread();
    void pushCommand(Command cmd);

private:
    Command popCommand();

    void run();
    //在本地文件中搜索
    void queryLocalFile(QString search_content);

    QQueue<Command> m_commandQueue;
    QMutex m_mutex;

signals:
    void LocalFileSearchResult(QList<FileInfo> result);
};

#endif // WORKERTHREAD_H
