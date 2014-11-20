#include "applicationex.h"
#include "preferenceswidget.h"
#include <QSettings>
#include <QString>
#include <QDebug>


PreferencesWidget* g_preferencesWidget = NULL;

int main(int argc, char *argv[])
{
    try
    {
        ApplicationEx app(argc, argv);

        app.setApplicationName("SimpleShare");
        app.setOrganizationName("BoxNet");
        //app.setOrganizationDomain("box.net");
        app.setApplicationVersion(APP_VERSION);

        app.enableLogging();

        qDebug() << qApp->applicationName() << " " << qApp->applicationVersion();

        QStringList arguments = app.arguments();
        arguments.removeAt(0); // remove application name from arguments list;
        QString message = arguments.join("<");

        bool commandQuit    = arguments.indexOf("--quit") != -1;
        bool commandLogout  = arguments.indexOf("--logout") != -1;

        if(app.isRunning())
        {
            if (app.sendMessage(message))
            {
                qDebug() << "can't sendMessage to running copy of application";
            }
            return 0;
        }
        else
        {
            if (commandQuit)
            {
                qDebug() << "want to quit, but no running instances found";
                return 0;
            }
        }

        // migrate settings: /////////////////////////////////////////
        // in previous version.
        // ApplicationName was "simplescreenshot"
        // OrganizationName was "BoxNet"
        // OrganizationDomain "box.net"
        // try to migrate settings to new registry key:
        // ApplicationName "SimpleShare"

        QString defaultuser;
        bool needMigrate = false;
        {
            app.setApplicationName("simplescreenshot");
            QSettings registry;
            needMigrate = registry.contains("defaltuser")
                    || registry.contains("data/recentUpload0");
            defaultuser = registry.value("defaltuser", "").toString();
        }
        if (needMigrate)
        {
            Settings settings; // read
            app.setApplicationName("SimpleShare");
            settings.writeSettings(); // write
            {
                QSettings registry;
                registry.setValue("defaltuser", defaultuser);
            }

            app.setApplicationName("simplescreenshot");
            {
                QSettings registry;
                registry.remove("defaltuser");
                registry.remove("data");
                registry.remove("options");
            }
            app.setApplicationName("SimpleShare");
        }
        //////////////////////////////////////////////////////////////

        app.setApplicationName("SimpleShare");

        bool firstRun = false;
        {
            QSettings registry;
            firstRun = !registry.contains("options/automaticUpload");
        }

        if (commandLogout)
        {
            BxNet* bxnet  = new BxNet();
            bxnet->logout();
            delete bxnet;

            return 0;
        }

        //create the window
        g_preferencesWidget = new PreferencesWidget();
        if (g_preferencesWidget)
        {

#ifndef Q_OS_WIN
            if (firstRun)
#endif
            {
                //preferencesWidget.showLoginDialog();
                //preferencesWidget.show(); // need to show window for correct menu handling!
                //preferencesWidget.raise();
                //preferencesWidget.activateWindow();
                /*
                if (!firstRun && PreferencesWidget.boxnet()->authentificated())
                {
                    preferencesWidget.close(); // but don't need that window
                }
                */
            }

            if (!message.isEmpty())
            {
                g_preferencesWidget->handleMessage(message);
            }

            app.setActiveWindow(g_preferencesWidget);

            app.setQuitOnLastWindowClosed(false);
            int result = app.exec();

            delete g_preferencesWidget;

            return result;
        }
        else
        {
            return -1;
        }

    }
    catch(...)
    {
        qDebug() << "Absolutely unknow error";
    }

    return -1;
}

