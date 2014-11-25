/* oleutils.cpp
 * Author: Evgeniy Sergeev, <evgeniy.sereev@gmail.com>
 */

#include "oleutils.h"

#include <windows.h>
/*
#include <QDesktopServices>
#include <QAxObject>
#include <QUuid>
#include <QSettings>
#include <QStringList>
#include <QDebug>

QString uploadFromOffice(const QString& appName)
{   
    QString result;

    QString search;

    CLSID clsid;
    HRESULT res = CLSIDFromProgID((wchar_t*)appName.utf16(), &clsid);
    if (res == S_OK)
    {
        search = QUuid(clsid).toString();
    }
    else
    {
        QSettings controls(QLatin1String("HKEY_LOCAL_MACHINE\\Software\\Classes\\"), QSettings::NativeFormat);
        search = controls.value(appName + QLatin1String("/CLSID/Default")).toString();
        if (search.isEmpty())
        {
            controls.beginGroup(QLatin1String("/CLSID"));
            QStringList clsids = controls.childGroups();
            for (QStringList::Iterator it = clsids.begin(); it != clsids.end(); ++it)
            {
                QString clsid = *it;
                QString name = controls.value(clsid + QLatin1String("/Default")).toString();
                if (name == appName)
                {
                    search = clsid;
                    break;
                }
            }
            controls.endGroup();
        }
    }

    if (search.isEmpty())
    {
        search = appName;
    }

    search.replace("{", "");
    search.replace("}", "");

    IUnknown* ptr;
    GetActiveObject(QUuid(search), 0, &ptr);

    QAxObject *word = new QAxObject(ptr);
    if (word)
    {
        //word->setProperty("Visible", true);
        //QAxObject * documents = word->querySubObject("Documents");
        //documents->dynamicCall("Add (void)");

        QString activeDocumentName = "ActiveDocument";
        if (!appName.contains("Word"))
        {
            activeDocumentName = "ActiveWorkbook";
        }

        QAxObject* document = word->querySubObject(activeDocumentName.toLatin1().data());

        if (document)
        {
            //foreach(QByteArray name, document->dynamicPropertyNames())
            //{
            //    qDebug() << "name = " << name;
            //}


            //result = document->property("Name").toString();
            //result = QDesktopServices::storageLocation(QDesktopServices::TempLocation) + "/" + result;
            ////WCHAR buff[MAX_PATH];
            ////result.toWCharArray(buff);
            ////DeleteFileW(buff);
            ////document->dynamicCall("SaveCopyAs (const QString&)", result);

            result = document->property("FullName").toString();
            if (!result.contains(":"))
            {
                result = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation) + "/" + result;
            }
            document->dynamicCall("SaveAs (const QString&)", result);
            result = document->property("FullName").toString();
            //document->dynamicCall("Close (boolean)", false);
            //word->dynamicCall("Quit (void)");
            delete document;
        }


        delete word;
    }

    return result;
}

            */
QString activeWindowTitle()
{
    HWND hwnd = GetForegroundWindow();
    WCHAR buff[256];
    buff[0] = 0;
    GetWindowTextW(hwnd, buff, 256);

    QString result = QString::fromWCharArray(buff);

    return result;
}

QString uploadFromOffice(const QString& appName)
{
    return QString();
}
