#ifndef ZIPHELPER_H
#define ZIPHELPER_H

#include <QObject>
#include <QString>
#include <QStringList>

class ZipHelper: public QObject
{
    Q_OBJECT
public:
    explicit ZipHelper(const QString& zipName, QObject* parent = NULL);
    ~ZipHelper();

    void compressPaths(const QStringList& paths);
    void compressPath(const QString& path);

    const QStringList paths() const;

signals:
    void compressed(const QString& zipName);

private slots:
    void onThreadFinished();

private:
     QString        m_zipName;
     QStringList    m_paths;
};

#endif // ZIPHELPER_H
