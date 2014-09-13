#include "synchttp.h"
#include <QBuffer>

SyncHTTP::SyncHTTP(QObject *parent)
    : QHttp(parent)
    , requestID(-1)
    , status(false)
{
}

SyncHTTP::SyncHTTP(const QString& hostName, quint16 port, QObject* parent)
    : QHttp(hostName, port, parent)
    ,requestID(-1)
    ,status(false)
{
}

SyncHTTP::~SyncHTTP()
{
}

bool SyncHTTP::syncGet(const QString& path, QIODevice* to)
{
    connect(this,SIGNAL(requestFinished(int,bool)), SLOT(finished(int,bool)));
    requestID = get(path, to );
    loop.exec();
    return status;
}

bool SyncHTTP::syncPost(const QString& path, QIODevice* data, QIODevice* to)
{
    connect(this,SIGNAL(requestFinished(int,bool)), SLOT(finished(int,bool)));
    requestID = post(path, data , to );
    loop.exec();
    return status;
}

bool SyncHTTP::syncPost(const QString& path, const QByteArray& data, QIODevice* to)
{
    QBuffer buffer;
    buffer.setData(data);
    return syncPost(path, &buffer, to);
}

void SyncHTTP::finished(int idx, bool err)
{
    if(idx!=requestID) return;
    status = !err;
    loop.exit();
}
