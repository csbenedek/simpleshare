/* recordingframe.cpp
 * Author: Evgeniy Sergeev, <evgeniy.sereev@gmail.com>
 */

#include "recordingframe.h"
#include "ui_recordingframe.h"

// RecordingFrame

RecordingFrame::RecordingFrame(int frameWidth, bool muted, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RecordingFrame)
    , m_frameWidth(frameWidth)
{
    ui->setupUi(this);

//#ifdef Q_OS_WIN
//    ui->mutePushButton->setChecked(muted);
//#else
    ui->mutePushButton->setChecked(false);
    ui->mutePushButton->setVisible(false);
//#endif
    //setAutoFillBackground(true);
    setWindowFlags(Qt::FramelessWindowHint
                   | Qt::WindowStaysOnTopHint
                   | Qt::Tool);
    setAttribute(Qt::WA_DeleteOnClose);
}

RecordingFrame::~RecordingFrame()
{
    delete ui;
}

int RecordingFrame::bottomMargin() const
{
    return ui->stopRecordingPushButton->geometry().height() + 1;
}

int RecordingFrame::frameWidth() const
{
    return m_frameWidth;
}

void RecordingFrame::updateRegion()
{
    QRect r = geometry();
    QRect buttonRect = ui->stopRecordingPushButton->geometry();
    //buttonRect.setSize(buttonRect.size() - QSize(2,2));
    //buttonRect.translate(QPoint(1,1));
    r.setHeight(r.height() - bottomMargin());
    const int size = m_frameWidth;

    QRegion maskRegion;

    maskRegion = maskRegion.unite(QRegion(0, 0, r.width(), size));//Top
    maskRegion = maskRegion.unite(QRegion(r.width()-size, 0, size, r.height()));//Right
    maskRegion = maskRegion.unite(QRegion(0, r.height()-size, r.width(), size));//Bottom
    maskRegion = maskRegion.unite(QRegion(0, 0, size, r.height()));//Left
    maskRegion = maskRegion.unite(QRegion(buttonRect));

    if (ui->mutePushButton->isVisible())
    {
        QRect muteRect = ui->mutePushButton->geometry();
        maskRegion = maskRegion.unite(QRegion(muteRect));
    }

    setMask(maskRegion);
}

void RecordingFrame::on_stopRecordingPushButton_clicked()
{
    close();
}

void RecordingFrame::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    updateRegion();
}

void RecordingFrame::setInnerRect(QRect rect)
{
    setFixedSize(QSize(
                     rect.width() + 2 * m_frameWidth,
                     rect.height() + 2 * m_frameWidth + bottomMargin()));
    move(rect.left() - m_frameWidth, rect.top() - m_frameWidth);
}

void RecordingFrame::on_mutePushButton_clicked()
{
    emit mute(ui->mutePushButton->isChecked());
}

void RecordingFrame::closeEvent(QCloseEvent* event)
{
    emit stopRecording();
}
