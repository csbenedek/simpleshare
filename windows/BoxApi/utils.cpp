#include "utils.h"
#include <QtCore/QCoreApplication>
#include <QtNetwork/QNetworkInterface>
#include <QFileInfo>
#include <QVariant>
#include <QStringList>
#include <QDir>
#include <QDesktopWidget>
#include <QApplication>
#include <QHostInfo>
#include <QCryptographicHash>
#include <windows.h>
#include "crypto/blowfish.h"
#include <algorithm>
#include <lmcons.h>

void deletePath(const QString& path)
{    
    QFileInfo fileInfo(path);

    if (path.isEmpty())
    {
        return;
    }

    if (fileInfo.exists())
    {
        if(fileInfo.isFile())
        {
            fileInfo.dir().remove(fileInfo.fileName());
        }
        else
        {
            QDir dir2(path);
            dir2.setFilter(QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs);
            foreach(const QFileInfo& fileInfo, dir2.entryInfoList())
            {
                deletePath(fileInfo.absoluteFilePath());
            }
            fileInfo.dir().rmdir(fileInfo.fileName());
        }
    }
}

void removeUntouched(QString path, QDateTime border)
{
    //qDebug() << "border " << border << "path" << path;

    if (path.isEmpty())
    {
        return;
    }

    {
        WIN32_FIND_DATAW ffd;

        //LARGE_INTEGER filesize;
        //size_t length_of_arg;
        HANDLE hFind = INVALID_HANDLE_VALUE;
        //DWORD dwError=0;

        path = path + "\\*";


        WCHAR buf[MAX_PATH];
        buf[path.toWCharArray((wchar_t*)&buf)]=0;
        hFind = FindFirstFile(buf, &ffd);

        if (hFind != INVALID_HANDLE_VALUE)
        {
            do
            {
                if (ffd.cFileName && ffd.cFileName[0] != '.')
                {
                    QString p = path;

                    p.replace("*", QString::fromWCharArray(ffd.cFileName));

                    QFileInfo fileInfo(p);
                    //qDebug() << "check " << fileInfo.filePath() << "modified" << fileInfo.lastModified() << "created" << fileInfo.created();

                    if (fileInfo.lastModified() < border)
                    {
                        p.replace("/", "\\");
                        qDebug() << "deleting " << p;

                        WCHAR buf2[MAX_PATH];
                        buf2[p.toWCharArray((wchar_t*)&buf2)]=0;
                        DeleteFile(buf2);
                    }
                }
           }
           while (FindNextFileW(hFind, &ffd) != 0);
        }

        FindClose(hFind);
    }
}


bool renameFile(const QString& source, const QString& destination)
{
    QFileInfo sourceInfo(source), destinationInfo(destination);

    Q_ASSERT(sourceInfo.exists());
    Q_ASSERT(sourceInfo.isFile());

    if (!sourceInfo.exists() || !sourceInfo.isFile())
    {
        return false;
    }

    QFile sourceFile(source);
    QFile destinationFile(destination);

    destinationFile.remove(); // overwrite by default

    if (sourceFile.rename(destination))
    {
        return true;
    }

    bool result = true;
    if (sourceFile.open(QIODevice::ReadOnly)
        && destinationFile.open(QIODevice::WriteOnly))
    {
        uint dataLength = 4096;
        char *data = new char[dataLength];
        while(!sourceFile.atEnd())
        {
            int size = sourceFile.read(data, dataLength);
            if (destinationFile.write(data, size) != size)
            {
                result =false;
                break;
            }
        }
        delete[] data;
    }

    if (result == false)
    {
        return false;
    }

    sourceFile.remove();
    sourceFile.close();
    destinationFile.close();
    return true;
}

FileType getFileType(const QString& fileName)
{
    if (fileName.endsWith(".ai", Qt::CaseInsensitive))
    {
        return FILE_AI;
    }
    if (fileName.endsWith(".psd", Qt::CaseInsensitive))
    {
        return FILE_PSD;
    }
    if (fileName.endsWith(".bookmark", Qt::CaseInsensitive))
    {
        return FILE_BOOKMARK;
    }
    if (fileName.endsWith(".doc", Qt::CaseInsensitive)
        || fileName.endsWith(".docx", Qt::CaseInsensitive))
    {
        return FILE_DOC;
    }
    if (fileName.endsWith(".xls", Qt::CaseInsensitive)
        || fileName.endsWith(".csv", Qt::CaseInsensitive)
        || fileName.endsWith(".xlsx", Qt::CaseInsensitive))
    {
        return FILE_XLS;
    }
    if (fileName.endsWith(".ppt", Qt::CaseInsensitive)
        || fileName.endsWith(".pptx", Qt::CaseInsensitive)
        || fileName.endsWith(".pps", Qt::CaseInsensitive))
    {
        return FILE_PPT;
    }
    if (fileName.endsWith(".fla", Qt::CaseInsensitive)
        || fileName.endsWith(".flash", Qt::CaseInsensitive))
    {
        return FILE_FLA;
    }
    if (fileName.endsWith(".png", Qt::CaseInsensitive)
        || fileName.endsWith(".bmp", Qt::CaseInsensitive)
        || fileName.endsWith(".jpg", Qt::CaseInsensitive)
        || fileName.endsWith(".jpeg", Qt::CaseInsensitive))
    {
        return FILE_IMAGE;
    }
    if (fileName.endsWith(".mov", Qt::CaseInsensitive)
        || fileName.endsWith(".m4v", Qt::CaseInsensitive)
        || fileName.endsWith(".mp4", Qt::CaseInsensitive)
        || fileName.endsWith(".flv", Qt::CaseInsensitive)
        || fileName.endsWith(".divx", Qt::CaseInsensitive)
        || fileName.endsWith(".avi", Qt::CaseInsensitive)
        || fileName.endsWith(".mpg", Qt::CaseInsensitive)
        || fileName.endsWith(".rm", Qt::CaseInsensitive)
        || fileName.endsWith(".swf", Qt::CaseInsensitive)
        || fileName.endsWith(".vob", Qt::CaseInsensitive)
        || fileName.endsWith(".wmv", Qt::CaseInsensitive))
    {
        return FILE_VIDEO;
    }
    if (fileName.endsWith(".mp3", Qt::CaseInsensitive)
        || fileName.endsWith(".m4a", Qt::CaseInsensitive)
        || fileName.endsWith(".ape", Qt::CaseInsensitive)
        || fileName.endsWith(".flac", Qt::CaseInsensitive)
        || fileName.endsWith(".wav", Qt::CaseInsensitive)
        || fileName.endsWith(".aac", Qt::CaseInsensitive)
        || fileName.endsWith(".aif", Qt::CaseInsensitive)
        || fileName.endsWith(".m3u", Qt::CaseInsensitive)
        || fileName.endsWith(".mid", Qt::CaseInsensitive)
        || fileName.endsWith(".mpa", Qt::CaseInsensitive)
        || fileName.endsWith(".wma", Qt::CaseInsensitive)
        || fileName.endsWith(".ra", Qt::CaseInsensitive))
    {
        return FILE_MUSIC;
    }
    if (fileName.endsWith(".html", Qt::CaseInsensitive)
        || fileName.endsWith(".htm", Qt::CaseInsensitive)
        || fileName.endsWith(".php", Qt::CaseInsensitive))
    {
        return FILE_HTML;
    }
    if (fileName.endsWith(".zip", Qt::CaseInsensitive)
        || fileName.endsWith(".tar", Qt::CaseInsensitive)
        || fileName.endsWith(".rar", Qt::CaseInsensitive)
        || fileName.endsWith(".dmg", Qt::CaseInsensitive)
        || fileName.endsWith(".iso", Qt::CaseInsensitive)
        || fileName.endsWith(".msi", Qt::CaseInsensitive))
    {
        return FILE_ZIP;
    }

    return FILE_UNDEFIED;
}

QRect inflateRect(const QRect rect, int dx, int dy)
{
    return QRect(rect.x() - dx,
                 rect.y() - dx,
                 rect.width() + 2*dx,
                 rect.height() + 2*dy);
}

QRect deflateRect(const QRect rect, int dx, int dy)
{
    return QRect(rect.x() + dx,
                 rect.y() + dx,
                 rect.width() - 2*dx,
                 rect.height() - 2*dy);
}

QRectF inflateRect(const QRectF& rect, qreal dx, qreal dy)
{
    return QRectF(rect.x() - dx,
                  rect.y() - dx,
                  rect.width() + 2*dx,
                  rect.height() + 2*dy);
}

QRectF deflateRect(const QRectF& rect, qreal dx, qreal dy)
{
    return QRectF(rect.x() + dx,
                  rect.y() + dx,
                  rect.width() - 2*dx,
                  rect.height() - 2*dy);
}

int alignSize(int size, int delta, int min)
{
    // dimesions should be aligned
    int div = size % delta;
    size -= div;
    size += 8;
    if (size < min)
    {
        size = min;
    }
    return size;
}

QString sizeToHumanReadableString(double size)
{
    static QStringList s_suffix(QStringList() << "" << " Kb" << " Mb" << " Gb");

    foreach(const QString& suffix, s_suffix)
    {
        if (size < 1024)
        {
            return QString::number(size, 'f', 1) + suffix;
        }
        size /= 1024;
    }

    return QString::number(size, 'f', 1) + " Tb";
}

void centrateWidget(QWidget *widget)
{
    QDesktopWidget* desktop = QApplication::desktop();
    Q_ASSERT(desktop);
    if (desktop == NULL)
    {
        return;
    }
    QRect rect = desktop->screenGeometry(desktop->primaryScreen());

    int screenWidth = rect.width(); // get width of screen
    int screenHeight = rect.height(); // get height of screen

    QSize windowSize = widget->size(); // size of our application window
    int width = windowSize.width();
    int height = windowSize.height();

    // little computations
    int x = (screenWidth - width) / 2;
    int y = (screenHeight - height) / 2;

    // move window to desired coordinates
    widget->move(x, y);
}


// Randomizer

unsigned int setRandSeed()
{
    unsigned int seed = QDateTime::currentDateTime().toTime_t();
    srand(seed);
    return seed;
}


float rand(float Begin, float End)
{
    return static_cast<float>(rand()) / RAND_MAX * (End - Begin) + Begin;
}

int rand(int Begin, int End)
{
    return rand() % (End - Begin + 1) + Begin;
}

QString randStr(int maxSize)
{
    QString result;
    int size = maxSize;
    if (size <= 0)
    {
        size = rand(16,128);
    }

    result.resize(size);

    for(int i = 0; i < result.size(); ++i)
    {
        switch(rand(0, 2))
        {
        case 0: result[i] = rand('A','Z'); break;
        case 1: result[i] = rand('a','z'); break;
        case 2: result[i] = rand('0','9'); break;
        }
    }

    return result;
}

void securelyErase(QString& str)
{
    // don't store password more than it need
    // securely clear pass
    setRandSeed();
    for(int i = 0; i < str.length(); ++i)
    {
        str[i] = qrand();
    }
    str.clear();
}

void securelyErase(char* ptr, int size)
{
    setRandSeed();
    for(int i = 0; i < size; ++i)
    {
        ptr[i] = rand(0, 255);
    }
}

void securelyErase(QChar* ptr, int size)
{
    setRandSeed();
    for(int i = 0; i < size; ++i)
    {
        ptr[i] = rand(0, 255);
    }
}

QByteArray encrypt(const QByteArray& value, QString& password)
{
    Blowfish blowfish;
    blowfish.Set_Passwd(password.toStdString().c_str());
    securelyErase(password);
    const char* data = value.data();
    int size = value.size();

    blowfish.Encrypt((unsigned char*)data, size);
    return value.toBase64();
}

QByteArray decrypt(const QByteArray& value, QString& password)
{
    Blowfish blowfish;
    blowfish.Set_Passwd(password.toStdString().c_str());
    securelyErase(password);
    QByteArray encoded = QByteArray::fromBase64(value);
    const char* data = encoded.data();
    int size = encoded.size();

    blowfish.Decrypt((unsigned char*)data, size);
    return encoded;
}

QString encryptXor(const QString& pass)
{
    QString res;

    size_t len = pass.length();
    wchar_t* p = new wchar_t[len];
    if (len == 0)
    {
        return res;
    }
    if (p)
    {
        pass.toWCharArray(p);
        int key=129;
        for(size_t i = 0; i < len; i++)
        {
            wchar_t c = p[i] ^ key;
            res.append(c);
        }

        delete p;
    }

    return res;
}

QString decryptXor(const QString& pass)
{
    QString res;

    size_t len = pass.length();
    wchar_t* p = new wchar_t[len];
    if (len == 0)
    {
        return res;
    }
    if (p)
    {
        pass.toWCharArray(p);
        int key=129;
        for(size_t i = 0; i < len; i++)
        {
            wchar_t c = p[i] ^ key;
            res.append(c);
        }

        delete p;
    }

    return res;
}


QString generatePasswordForComputer()
{
    wchar_t winUserName[UNLEN + 1]; // UNLEN is defined in LMCONS.H
    DWORD winUserNameSize = sizeof(winUserName);
    GetUserName( winUserName, &winUserNameSize );
    QString userName = QString::fromWCharArray(winUserName, winUserNameSize);

    QString uniqueString = //QHostInfo::localDomainName() + QDir::separator() +
            QHostInfo::localHostName()
            + QDir::separator() + QString::number(QSysInfo::windowsVersion())
            + QDir::separator() + userName;
/*
    QList<QNetworkInterface> ifaces = QNetworkInterface::allInterfaces();

    for (int i = 0; i < ifaces.count(); i++)
    {
        QNetworkInterface iface = ifaces.at(i);
        if (//iface.flags().testFlag(QNetworkInterface::IsUp) &&
                !iface.flags().testFlag(QNetworkInterface::IsLoopBack) )
        {
            if (iface.addressEntries().count() > 0 && !iface.hardwareAddress().isEmpty())
            {
                uniqueString += QDir::separator();
                uniqueString += iface.hardwareAddress();
            }
        }
    }
*/
    return QString::fromLatin1(QCryptographicHash::hash(uniqueString.toLatin1(), QCryptographicHash::Md5).toHex().constData());
}
