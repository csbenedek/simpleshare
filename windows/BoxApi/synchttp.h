#ifndef SYNCHTTP_H
#define SYNCHTTP_H

#include <QHttp>
#include <QEventLoop>

class SyncHTTP
        : public QHttp
{
    Q_OBJECT

public:
    /// structors
    SyncHTTP(QObject* parent = NULL);
    SyncHTTP(const QString &hostName, quint16 port = 80, QObject* parent = 0);
    virtual ~SyncHTTP();

    bool syncGet(const QString& path, QIODevice* to = 0);
    bool syncPost(const QString& path, QIODevice* data, QIODevice* to = 0);
    bool syncPost(const QString& path, const QByteArray& data, QIODevice* to = 0);

protected slots:
    virtual void finished(int idx, bool err);

private:
    int requestID;
    bool status;
    QEventLoop loop;
};

#endif // SYNCHTTP_H
