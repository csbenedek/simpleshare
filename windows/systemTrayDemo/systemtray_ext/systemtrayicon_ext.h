#ifndef SYSTEMTRAYICON_EXT_H
#define SYSTEMTRAYICON_EXT_H

#include <QObject>
#include <QSystemTrayIcon>  // for QSystemTrayIcon::MessageIcon type
#include <inttypes.h>       // for uint16_t
#include <windows.h>

class QMenu;
class QEvent;
class QWheelEvent;
class QMouseEvent;
class QPoint;
class QIcon;

class SystemTrayIconExt : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString toolTip READ toolTip WRITE setToolTip)
    Q_PROPERTY(QIcon icon READ icon WRITE setIcon)
    Q_PROPERTY(bool visible READ isVisible WRITE setVisible DESIGNABLE false)

public:
    SystemTrayIconExt(QObject *parent = 0);
    SystemTrayIconExt(const QIcon &icon, QObject *parent = 0);
    ~SystemTrayIconExt();

    QIcon icon() const;
    void setIcon(const QIcon &icon);

    QString toolTip() const;
    void setToolTip(const QString &tip);

    static bool isSystemTrayAvailable();
    static bool supportsMessages();

    bool trayMessage(uint16_t msg);
    void showMessage(const QString &title, const QString &msg, QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::Information,
                     int msecs = 5000, QPixmap pixmap = QPixmap());

    QRect geometry() const;
    bool isVisible() const;

public Q_SLOTS:
    void setVisible(bool visible);
    inline void show() { setVisible(true); }
    inline void hide() { setVisible(false); }

Q_SIGNALS:
    void activated(QSystemTrayIcon::ActivationReason reason);
    void messageClicked();

protected:
    bool event(QEvent *event);

private:
    friend void qtsystray_sendActivated(QSystemTrayIcon *, int);

private:
	HWND	m_hWnd;
	QMenu*	m_menu;
	QIcon*	m_icon;
	HICON	m_hIcon;
	HICON	m_hBalloonIcon;
};

#endif // SYSTEMTRAYICON_EXT_H
