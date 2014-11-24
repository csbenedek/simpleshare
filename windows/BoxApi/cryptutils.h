/* cryptutils.h
 * Author: Evgeniy Sergeev, <evgeniy.sereev@gmail.com>
 */


#ifndef CRYPTUTILS_H
#define CRYPTUTILS_H

#include <QString>
#include <QRect>
#include <QWidget>
#include <QDateTime>


// crypt helpers
QString generatePasswordForComputer();
QByteArray encrypt(const QByteArray& value, QString& password);
QByteArray decrypt(const QByteArray& pass, QString& password);
QString randStr(int maxSize = 0);
void securelyErase(QString& str);
void securelyErase(char* ptr, int size);
void securelyErase(QChar* ptr, int size);

QString encryptXor(const QString& pass);
QString decryptXor(const QString& pass);

#endif // CRYPTUTILS_H
