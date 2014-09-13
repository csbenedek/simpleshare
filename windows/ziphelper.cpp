#include <QtNetwork>
#include <QtCore>
#include <QtXml>
#include <QDebug>
#include "ziphelper.h"
#include <QFileInfo>
#include <QDirIterator>
#include <QThread>

#include "qzipwriter_p.h"

//ZipThread

//TODO: in future realize way to stop thread on a long operations

class ZipThread: public QThread
{
public:
    explicit ZipThread(const QString& zipName, const QStringList& paths, QObject* parent = NULL);
    virtual ~ZipThread();

private:
    bool compressFile(const QString& fileName, QZip::QZipWriter& zip);
    bool compressDir(QString dirName, QZip::QZipWriter& zip);

protected:
    virtual void run();

private:
    QString     m_zipName;
    QStringList m_paths;
};

ZipThread::ZipThread(const QString& zipName, const QStringList& paths, QObject* parent)
    : QThread(parent)
    , m_zipName(zipName)
    , m_paths(paths)
{

}

ZipThread::~ZipThread()
{

}

void ZipThread::run()
{
    QZip::QZipWriter zip(m_zipName);
    zip.setCompressionPolicy(QZip::QZipWriter::AutoCompress);

    foreach(const QString& path, m_paths)
    {
        QFileInfo fileInfo(path);

        if (fileInfo.isFile())
        {
            compressFile(path, zip);
        }
        else if (fileInfo.isDir())
        {
            compressDir(path, zip);
        }
    }

    zip.close();
}

bool ZipThread::compressFile(const QString& fileName, QZip::QZipWriter& zip)
{
    if (zip.status() != QZip::QZipWriter::NoError)
    {
        return false;
    }

    QFileInfo fileInfo(fileName);
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly))
    {
        return false;
    }

    zip.setCreationPermissions(fileInfo.permissions());
    QByteArray data = file.readAll();
    zip.addFile(fileInfo.fileName(), data);

    file.close();

    return true;
}

bool ZipThread::compressDir(QString dirName, QZip::QZipWriter& zip)
{
    dirName = dirName.replace("\\", "/");   

    QString unneededPath = QFileInfo(dirName).dir().absolutePath();
    unneededPath += "/";

    /*
    zip.setCreationPermissions(QFile::permissions(dirName));
    QString shortName = dirName;
    shortName = shortName.remove(unneededPath);
    zip.addDirectory(shortName);
    */

    dirName += "/"; // leading '/' is very important

    QDirIterator it(dirName, QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        QString filePath = it.next();

        if (it.fileInfo().isDir())
        {
            zip.setCreationPermissions(QFile::permissions(filePath));
            QString shortName = filePath.remove(unneededPath);
            zip.addDirectory(shortName);
        }
        else if (it.fileInfo().isFile())
        {
            QFile file(filePath);
            if (!file.open(QIODevice::ReadOnly))
            {
                continue;
            }

            zip.setCreationPermissions(QFile::permissions(filePath));
            QByteArray data = file.readAll();
            QString shortName = filePath.remove(unneededPath);
            zip.addFile(shortName, data);
        }
    }

    return true;
}

// ZipHelper

ZipHelper::ZipHelper(const QString& zipName, QObject* parent)
    : QObject(parent)
    , m_zipName(zipName)
{

}

ZipHelper::~ZipHelper()
{

}

const QStringList ZipHelper::paths() const
{
    return m_paths;
}

void ZipHelper::compressPath(const QString& path)
{
    compressPaths(QStringList() << path);
}

void ZipHelper::compressPaths(const QStringList& paths)
{
    m_paths = paths;
    ZipThread* thread = new ZipThread(m_zipName, m_paths, this);
    connect(thread, SIGNAL(finished()), this, SLOT(onThreadFinished()));
    thread->start();
}

void ZipHelper::onThreadFinished()
{
    emit compressed(m_zipName);
}
