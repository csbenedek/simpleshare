/* fileutils.cpp
 * Author: Evgeniy Sergeev, <evgeniy.sereev@gmail.com>
 */


#include "fileutils.h"

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
