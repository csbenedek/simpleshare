/* cryptutils.cpp
 * Author: Evgeniy Sergeev, <evgeniy.sereev@gmail.com>
 */

#include "cryptutils.h"


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
