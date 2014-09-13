#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QRect>
#include <QWidget>
#include <QDateTime>

// File helpers

enum FileType
{
    FILE_UNDEFIED = 0,
    FILE_IMAGE,
    FILE_VIDEO,
    FILE_AI,
    FILE_PSD,
    FILE_BOOKMARK,
    FILE_DOC,
    FILE_XLS,
    FILE_PPT,
    FILE_FLA,
    FILE_MUSIC,
    FILE_HTML,
    FILE_ZIP,

    FILE_MAX
};

// Try to guess file type base on it's name

FileType getFileType(const QString& fileName);
bool renameFile(const QString& source, const QString& destination);
void deletePath(const QString& path);
void clearArchiveBits(const QString& path);
void removeUntouched(const QString& path);
void removeUntouched(const QString path, QDateTime border);
                             
// Randomizer

unsigned int setRandSeed();
float rand(float Begin, float End);
int rand(int Begin, int End);

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


QString sizeToHumanReadableString(double size);

// Rect helpers

QRect inflateRect(const QRect rect, int dx, int dy);
QRect deflateRect(const QRect rect, int dx, int dy);
QRectF inflateRect(const QRectF& rect, qreal dx, qreal dy);
QRectF deflateRect(const QRectF& rect, qreal dx, qreal dy);

int alignSize(int size, int delta, int min);

// Widget helpers

void centrateWidget(QWidget* widget);

#endif // UTILS_H
