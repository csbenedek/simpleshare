#ifndef SYSTEMTRAYICON_H  	
#define SYSTEMTRAYICON_H

#include <QObject>

class QMenu;
class QEvent;
class QWheelEvent;
class QMouseEvent;
class QPoint;

class SystemTrayIcon : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString toolTip READ toolTip WRITE setToolTip)
    Q_PROPERTY(QIcon icon READ icon WRITE setIcon)
    Q_PROPERTY(bool visible READ isVisible WRITE setVisible DESIGNABLE false)

public:
    SystemTrayIcon(QObject *parent = 0);
    SystemTrayIcon(const QIcon &icon, QObject *parent = 0);
    ~SystemTrayIcon();

    QIcon icon() const;
    void setIcon(const QIcon &icon);

    QString toolTip() const;
    void setToolTip(const QString &tip);

    static bool isSystemTrayAvailable();
    static bool supportsMessages();

    void showMessage(const QString &title, const QString &msg, QPixmap pixmap, int msecs = 10000);

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

#endif // SYSTEMTRAYICON_H
