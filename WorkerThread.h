#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H
#include <QThread>
#include <QDebug>

class WorkerThread : public QThread
{
    Q_OBJECT
public:
    WorkerThread();
private:
    void run();

signals:
    void LocalFileSearchResult(QString item);
};

#endif // WORKERTHREAD_H
