/* applicationex.cpp
 * Author: Evgeniy Sergeev, <evgeniy.sereev@gmail.com>
 */

#include "applicationex.h"
#include <QMouseEvent>
#include <QDebug>
#include <QWidget>
#include <QLocale>
#include <QTextCodec>
#include <QLatin1Char>
#include <QDebug>
#ifdef Q_OS_LINUX
#include <X11/Xlib.h>
#include <langinfo.h>
#endif
#ifdef Q_OS_WIN
#include <windows.h>
#endif
#include <QFile>
#include <QTextStream>
#include <QDesktopServices>
#include <QDateTime>

static QtMsgHandler     s_msgHandler    = NULL;
static QFile*           s_logFile       = NULL;
static QTextStream*     s_log           = NULL;
static QString          s_logFileName;
static bool             s_loggingEnabled = false;

static void initLog()
{
    if (s_log != NULL)
    {
        return;
    }

    s_logFileName     = QDesktopServices::storageLocation(QDesktopServices::TempLocation)
                            + "/" + qApp->applicationName() + QDateTime::currentDateTime().toString("yyyy-MM-dd") +  ".log";
    s_logFile = new QFile(s_logFileName);
    s_logFile->open(QIODevice::WriteOnly | QIODevice::Append);
    s_log = new QTextStream(s_logFile);
}

static void doneLog()
{
    if (s_log)
    {
        s_log->flush();
        delete s_log;
    }
    if (s_logFile)
    {
        s_logFile->close();
        delete s_logFile;
    }
}

void appMessageOutput(QtMsgType type, const char *msg)
{
    if (s_log == NULL)
    {
        initLog();
    }

    switch (type)
    {
    case QtDebugMsg:
        *s_log << QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss Debug:") << QString(msg) << "\r\n";
        s_log->flush();
        break;
    case QtWarningMsg:
        *s_log << QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss Warning:") << QString(msg) << "\r\n";
        s_log->flush();
        break;
    case QtCriticalMsg:
        *s_log << QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss Critical:") << QString(msg) << "\r\n";
        s_log->flush();
        break;
    case QtFatalMsg:
        *s_log << QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss Fatal:") << QString(msg) << "\r\n";
        s_log->flush();
        abort();
    }

    if (s_msgHandler)
    {
        s_msgHandler(type, msg);
    }
}

ApplicationEx::ApplicationEx(int &argc, char **argv)
    : QtSingleApplication(argc, argv)
    , m_mouseListener(NULL)
    , m_transparentWId(NULL)
#ifdef Q_OS_WIN
    , m_isActive(false)
#endif
{
    initLocale();
}

ApplicationEx::~ApplicationEx()
{
    doneLog();
}

void ApplicationEx::enableLogging(bool enable)
{
    if (enable)
    {
        s_loggingEnabled = true;
        initLog();
        s_msgHandler = qInstallMsgHandler(appMessageOutput);
    }
    else
    {
        s_loggingEnabled = false;
        qInstallMsgHandler(0);
        s_msgHandler = NULL;
    }
}

bool ApplicationEx::isLoggingEnabled()
{
    return s_loggingEnabled;
}

QString ApplicationEx::logFileName()
{
    return s_logFileName;
}

void ApplicationEx::initLocale()
{
    ::setlocale(LC_ALL,"");
    ::setlocale(LC_NUMERIC,"POSIX");

    char *vpLang = ::getenv("LANG");
    if(vpLang != 0 && vpLang[0] != '\0')
    {
        QLocale vLocale(vpLang);
        QLocale::setDefault(vLocale);
    }

    QTextCodec *vpC = 0;

// Win32 doesn't have nl_langinfo. but
// QTextCodec::codecForLocale() based on native WinAPI working correctly

#ifdef Q_OS_LINUX
    char *vpNlCodeset = ::nl_langinfo(CODESET);
    if(vpNlCodeset!=0 && vpNlCodeset[0] != '\0')
    {
        vpC = QTextCodec::codecForName(vpNlCodeset);
        if(vpC) QTextCodec::setCodecForLocale(vpC);
    }
#endif

#ifdef Q_OS_WIN32
    vpC = QTextCodec::codecForName("UTF-8");
    if(vpC)
    {
        QTextCodec::setCodecForLocale(vpC);
    }
#endif

    if(!vpC)
    {
        vpC = QTextCodec::codecForLocale();
    }

    if(vpC)
    {
        QTextCodec::setCodecForCStrings(vpC);
        QTextCodec::setCodecForTr(vpC);
    }
}

#ifdef Q_OS_WIN

void ApplicationEx::setSysTrayWindowTransparent(WId id)
{
    m_transparentWId = id;
    qDebug() << "transparent window set to " << m_transparentWId;
}

bool ApplicationEx::winEventFilter(MSG *msg, long *result)        // Windows event filter
{
    switch (msg->message)
    {
    case WM_ACTIVATE:
        if (msg->wParam == 0)
        {
            m_isActive = false;
        }
        else
        {
            m_isActive = true;
        }
    case WM_ERASEBKGND:
        if (msg->hwnd && (m_transparentWId == msg->hwnd))
        {
            //return true;
        }
        break;
    case WM_PAINT:
        if (msg->hwnd && (m_transparentWId == msg->hwnd))
        {
            //qDebug() << QTime::currentTime().toString() << "WM_PAINT skipped for" << msg->hwnd;
            //return true;
        }
        break;
    case WM_TIMECHANGE:
        break;
    case WM_POWERBROADCAST:
        switch (msg->wParam)
        {
        case PBT_APMSUSPEND:
            emit powerSleep();
            processEvents();
            break;
        case PBT_APMRESUMEAUTOMATIC:
            emit powerResume();
            break;
        }
        break;
    }
    return false;
}

#endif


#ifdef Q_OS_LINUX

bool ApplicationEx::x11EventFilter(XEvent* event)
{
    switch (event->type)
    {
    case MotionNotify:
    {
        m_currentPos = QPoint(event->xmotion.x_root, event->xmotion.y_root);
        if (m_mouseListener)
        {
            QPoint pos = m_mouseListener->mapFromGlobal(m_currentPos);
            QRect globalRect(m_mouseListener->mapToGlobal(m_mouseListener->rect().topLeft()), m_mouseListener->mapToGlobal(m_mouseListener->rect().bottomRight()));
            //if ( !globalRect.contains(m_currentPos) || m_currentPos.isNull())
            {
                QMouseEvent mouseEvent(QEvent::MouseMove, pos, m_currentPos, Qt::NoButton, Qt::NoButton, 0);
                sendEvent(m_mouseListener, &mouseEvent);
            }

            return true;
        }
        break;
    }
    case ButtonRelease:
    {
        if (m_mouseListener)
        {
            QRect globalRect(m_mouseListener->mapToGlobal(m_mouseListener->rect().topLeft()), m_mouseListener->mapToGlobal(m_mouseListener->rect().bottomRight()));
            //if ( !globalRect.contains(m_currentPos) || m_currentPos.isNull())
            {
                QPoint pos = m_mouseListener->mapFromGlobal(m_currentPos);
                Qt::MouseButton button;
                switch (event->xbutton.button)
                {
                case 1: button = Qt::LeftButton; break;
                case 2: button = Qt::MidButton; break;
                case 3: button = Qt::RightButton; break;
                default:button = Qt::NoButton; break;
                }
                QMouseEvent mouseEvent(QEvent::MouseButtonRelease, pos, m_currentPos, button, button, 0);
                sendEvent(m_mouseListener, &mouseEvent);

                return true;
            }
        }
        break;
    }
    case ButtonPress:
    {
        if (m_mouseListener)
        {
            QRect globalRect(m_mouseListener->mapToGlobal(m_mouseListener->rect().topLeft()), m_mouseListener->mapToGlobal(m_mouseListener->rect().bottomRight()));
            //if ( !globalRect.contains(m_currentPos) || m_currentPos.isNull())
            {
                QPoint pos = m_mouseListener->mapFromGlobal(m_currentPos);
                Qt::MouseButton button;
                switch (event->xbutton.button)
                {
                case 1: button = Qt::LeftButton; break;
                case 2: button = Qt::MidButton; break;
                case 3: button = Qt::RightButton; break;
                default:button = Qt::NoButton; break;
                }
                QMouseEvent mouseEvent(QEvent::MouseButtonPress, pos, m_currentPos, button, button, 0);
                sendEvent(m_mouseListener, &mouseEvent);

                return true;
            }
        }
    }
    break;
    }

    return QtSingleApplication::x11EventFilter(event);
}

QWidget* ApplicationEx::mouseListener() const
{
    return m_mouseListener;
}

void ApplicationEx::setMouseListener(QWidget* widget)
{
    m_mouseListener = widget;
}

void ApplicationEx::resetMouseListener()
{
    m_mouseListener = NULL;
}

#endif
