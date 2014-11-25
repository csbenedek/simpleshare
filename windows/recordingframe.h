/* recordingframe.h
 * Author: Evgeniy Sergeev, <evgeniy.sereev@gmail.com>
 */

#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

namespace Ui {
    class RecordingFrame;
}

class RecordingFrame : public QWidget
{
    Q_OBJECT

public:
    explicit RecordingFrame(int frameWidth, bool muted, QWidget *parent = 0);
    ~RecordingFrame();

    void setInnerRect(QRect rect);
    void updateRegion();
    int  bottomMargin() const;
    int  frameWidth() const;

protected:
    virtual void resizeEvent(QResizeEvent* event);
    virtual void closeEvent(QCloseEvent* event);

private slots:
    void on_stopRecordingPushButton_clicked();

    void on_mutePushButton_clicked();

signals:
    void stopRecording();
    void mute(bool muted);

private:
    Ui::RecordingFrame *ui;

    int m_frameWidth;
};

#endif // WIDGET_H
