/******************************************************************************
Copyright (c) 2011, Evgeniy Sergeev <evgeniy.sergeev@gmail.com>
All rights reserved.
******************************************************************************/
/******************************************************************************
Copyright (c) 2010, Artem Galichkin <doomer3d@gmail.com>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#ifndef KEYSEQUENCEWIDGET_H
#define KEYSEQUENCEWIDGET_H

#include <QLineEdit>
#include <QKeySequence>
#include <QString>
#include <QEvent>
#include <QKeyEvent>
#include <QTimer>

//#define UI_CLEAR_BUTTON

class KeySequenceWidget : public QLineEdit
{
    Q_OBJECT

    Q_PROPERTY(QKeySequence keySequence READ keySequence WRITE setKeySequence)
    Q_PROPERTY(QString noneText READ noneText WRITE setNoneText)

public:
    explicit KeySequenceWidget(QWidget *parent = 0);
    explicit KeySequenceWidget(QKeySequence seq, QWidget *parent = 0);
    virtual ~KeySequenceWidget();

    QKeySequence keySequence() const;
    QString noneText() const;

protected:
    // Reimplemented for internal reasons.
    virtual bool event(QEvent *e);
    virtual void keyPressEvent(QKeyEvent *keyEvent);
    virtual void keyReleaseEvent(QKeyEvent *keyEvent);
    virtual void focusInEvent(QFocusEvent* event);
    virtual void focusOutEvent(QFocusEvent* event);
    virtual void mousePressEvent(QMouseEvent* mouseEvent);
    virtual void mouseMoveEvent(QMouseEvent* mouseEvent);
    virtual void paintEvent(QPaintEvent* event);

private:
    void init(const QKeySequence keySeq, const QString noneStr);

    void updateDisplayShortcut();

signals:
    void keySequenceStart();
    void keySequenceEnd();
    void keySequenceChanged(const QKeySequence &seq);
    void keySequenceAccepted(const QKeySequence &seq);
    void keySequenceCleared();
    void keyNotSupported();

public slots:
    void startRecording();
    void doneRecording();
    void cancelRecording();
    void setKeySequence(const QKeySequence &key);
    void clearKeySequence();
    void setNoneText(const QString text);
    void controlModifierlessTimeout();

    // members
private:
    QKeySequence    m_currentSequence;
    QKeySequence    m_oldSequence;
    QKeySequence    m_defaultKeySequence;
    QString         m_noneSequenceText;

    QTimer          m_modifierlessTimeout;

    quint32         m_numKey;
    quint32         m_modifierKeys;

    bool            m_isRecording;

#ifdef UI_CLEAR_BUTTON
    QPixmap         m_reloadIcon;
#endif

};

#endif // KEYSEQUENCEWIDGET_H
