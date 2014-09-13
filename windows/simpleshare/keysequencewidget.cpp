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

#include "keysequencewidget.h"
#include <QPainter>
#include <QDebug>

KeySequenceWidget::KeySequenceWidget(QWidget *parent)
    : QLineEdit(parent)
{
    init(QKeySequence(), QString());
}

KeySequenceWidget::KeySequenceWidget(QKeySequence seq, QWidget *parent)
    : QLineEdit(parent)
{
    init(seq, QString());
}

KeySequenceWidget::~KeySequenceWidget()
{

}

void KeySequenceWidget::setKeySequence(const QKeySequence& key)
{
    if (!key.isEmpty() && m_defaultKeySequence.isEmpty())
    {
        m_defaultKeySequence = key;
    }

    if (m_isRecording)
    {
        m_oldSequence = m_currentSequence;
        doneRecording();
    }
    m_currentSequence = key;
    updateDisplayShortcut();
}

QKeySequence KeySequenceWidget::keySequence() const
{
    return m_currentSequence;
}

void KeySequenceWidget::setNoneText(const QString text)
{
    m_noneSequenceText = text;
    updateDisplayShortcut();
}

QString KeySequenceWidget::noneText() const
{
    return m_noneSequenceText;
}

void KeySequenceWidget::init(const QKeySequence keySeq, const QString noneStr)
{   
#ifdef UI_CLEAR_BUTTON
    m_reloadIcon.load(":/res/clear.png");
#endif

    if (noneStr.isNull())
    {
        m_noneSequenceText = "Enter shortcut ...";
    }
    else
    {
        m_noneSequenceText = noneStr;
    }

    clearKeySequence();
    m_currentSequence = keySeq;
    m_oldSequence = m_currentSequence;

    setFocusPolicy(Qt::StrongFocus);
    setContextMenuPolicy(Qt::NoContextMenu);

    // connect signals to slots
    connect(&m_modifierlessTimeout, SIGNAL(timeout()), this, SLOT(doneRecording()));

    // update ui
    updateDisplayShortcut();
}

void KeySequenceWidget::startRecording()
{
    setCursor(Qt::IBeamCursor);

    if (m_isRecording)
    {
        return;
    }

    emit keySequenceStart();

    m_numKey = 0;
    m_modifierKeys = 0;
    m_oldSequence = m_currentSequence;
    m_currentSequence = QKeySequence();
    m_isRecording = true;
    grabKeyboard();

    if (!QWidget::keyboardGrabber())
    {
        qWarning() << "Failed to grab the keyboard! Most likely qt's nograb option is active";
    }

    // update Shortcut display
    updateDisplayShortcut();
}

void KeySequenceWidget::doneRecording()
{
    setCursor(Qt::ArrowCursor);

    releaseKeyboard();
    m_modifierlessTimeout.stop();

    if (m_isRecording)
    {
        m_isRecording = false;
        emit keySequenceAccepted(m_currentSequence);
    }
    else
    {
        if (!m_oldSequence.isEmpty())
        {
            emit keySequenceChanged(m_currentSequence);
        }
    }


    // if sequence is not changed
    if (m_currentSequence == m_oldSequence)
    {
        // update Shortcut display
        updateDisplayShortcut();

        return;
    }
    // update Shortcut display
    updateDisplayShortcut();

    emit keySequenceEnd();
}

void KeySequenceWidget::cancelRecording()
{
    if (!m_isRecording)
    {
        return;
    }

    m_currentSequence = m_oldSequence;
    doneRecording();
}

void KeySequenceWidget::clearKeySequence()
{
    //setKeySequence(QKeySequence());
    setKeySequence(m_defaultKeySequence);
    updateDisplayShortcut();
    emit keySequenceCleared();
}

void KeySequenceWidget::controlModifierlessTimeout()
{
    if (m_numKey != 0 && !m_modifierKeys)
    {
        // No modifier key pressed currently. Start the timeout
        m_modifierlessTimeout.start(600);
    }
    else
    {
        // A modifier is pressed. Stop the timeout
        m_modifierlessTimeout.stop();
    }
}

void KeySequenceWidget::updateDisplayShortcut()
{
    QString str;
    if (m_isRecording)
    {
        if (m_modifierKeys)
        {
            if ((m_modifierKeys & Qt::META) )
                str += "Meta+";

            if ((m_modifierKeys & Qt::CTRL) )
                str += "Ctrl+";

            if ((m_modifierKeys & Qt::ALT) )
                str += "Alt+";

            if ((m_modifierKeys & Qt::SHIFT) )
                str += "Shift+";
        }

        // make it clear that input is still going on
        if (str.isEmpty())
        {
            //str = m_noneSequenceText;
        }
        else
        {
            str.append("...");
        }
    }
    else
    {
        str = m_currentSequence.toString(QKeySequence::NativeText);
        //str += " " + QString::number(currentNativeKey) + " " + QString::number(currentNativeScanCode) + " " + QChar(currentNativeKey) + " " + QChar(currentNativeScanCode);
    }

    if (str.isEmpty())
    {
        //str = m_noneSequenceText;
    }

    setText(str);

    if (m_isRecording)
    {
        setReadOnly(false);
        setCursorPosition(str.length());
    }
    else
    {
        setReadOnly(true);
    }
}

bool KeySequenceWidget::event(QEvent *e)
{
    if (m_isRecording && e->type() == QEvent::KeyPress)
    {
        keyPressEvent(static_cast<QKeyEvent*>(e));
        return true;
    }

    if (m_isRecording && e->type() == QEvent::ShortcutOverride)
    {
        e->accept();
        return true;
    }

    if (m_isRecording && e->type() == QEvent::FocusOut)
    {
        cancelRecording();
        return true;
    }

    return QLineEdit::event(e);
}

void KeySequenceWidget::keyPressEvent(QKeyEvent *keyEvent)
{
    int keyQt =  keyEvent->key();

// Qt sometimes returns garbage keycodes, I observed -1,
// if it doesn't know a key.
// We cannot do anything useful with those (several keys have -1,
// indistinguishable)
// and QKeySequence.toString() will also yield a garbage string.
    if (keyQt == -1)
    {
        // key not supported in Qt
        cancelRecording();
        emit keyNotSupported();

    }

    if (m_isRecording && (keyQt == Qt::Key_Escape || keyQt == Qt::Key_Tab))
    {
        cancelRecording();
        if (keyQt == Qt::Key_Tab)
        {
            QLineEdit::keyPressEvent(keyEvent);
        }
        return;
    }

    //get modifiers key
    uint newModifiers = keyEvent->modifiers() & (Qt::SHIFT | Qt::CTRL | Qt::ALT | Qt::META);

    // block autostart capturing on key_return or key space press
    if (!m_isRecording && (keyQt == Qt::Key_Return || keyQt == Qt::Key_Space))
    {
        return;
    }

    // We get events even if recording isn't active.
    if (!m_isRecording)
    {
        return; //QLineEdit::keyPressEvent(keyEvent);
    }

    keyEvent->accept();
    m_modifierKeys = newModifiers;

    // switching key type
    switch(keyQt)
    {
        case Qt::Key_AltGr: //or else we get unicode salad
            return;
        case Qt::Key_Shift:
        case Qt::Key_Control:
        case Qt::Key_Alt:
        case Qt::Key_Meta:
        case Qt::Key_Menu: //unused (yes, but why?)
        // TODO - check it key
            controlModifierlessTimeout();
            updateDisplayShortcut();
            break;
        default:
        {

        }

        // We now have a valid key press.
        if (keyQt)
        {
            if (keyEvent->nativeVirtualKey() >= Qt::Key_0 && keyEvent->nativeVirtualKey() <= Qt::Key_9)
            {
                keyQt = '0' + keyEvent->nativeVirtualKey() - Qt::Key_0;
            }

            if ((keyQt == Qt::Key_Backtab) && (m_modifierKeys & Qt::SHIFT))
            {
                keyQt = Qt::Key_Tab | m_modifierKeys;
            }
            else //if (d->isShiftAsModifierAllowed(keyQt))
            {
                keyQt |= m_modifierKeys;
            }


            if (m_numKey == 0)
            {
                m_currentSequence = QKeySequence(keyQt);
            }

            m_numKey++; // increment nuber of pressed keys

            if (m_numKey >= 1) // only one key in sequence allowed
            {
                doneRecording();
                return;
            }

            controlModifierlessTimeout();
            updateDisplayShortcut();
        }
    }
}

void KeySequenceWidget::keyReleaseEvent(QKeyEvent *keyEvent)
{
    if (keyEvent->key() == -1)
    {
        // ignore garbage, see keyPressEvent()
        return;
    }

    // if not recording mode
    if (m_isRecording == false)
    {
        return QLineEdit::keyReleaseEvent(keyEvent);
    }

    keyEvent->accept();

    uint newModifiers = keyEvent->modifiers() & (Qt::SHIFT | Qt::CTRL | Qt::ALT | Qt::META);

    // if a modifier that belongs to the shortcut was released...
    if ((newModifiers & m_modifierKeys) < m_modifierKeys)
    {
        m_modifierKeys = newModifiers;
        controlModifierlessTimeout();
        updateDisplayShortcut();
    }
}

void KeySequenceWidget::focusInEvent(QFocusEvent* event)
{
    QLineEdit::focusInEvent(event);
    startRecording();
}

void KeySequenceWidget::focusOutEvent(QFocusEvent* event)
{
    QLineEdit::focusOutEvent(event);
    if (m_isRecording)
    {
        cancelRecording();
    }
}

void KeySequenceWidget::mouseMoveEvent(QMouseEvent* mouseEvent)
{
    QLineEdit::mouseMoveEvent(mouseEvent);

#ifdef UI_CLEAR_BUTTON
    if (m_isRecording)
    {
        QRect clearRect = QRect(rect().topRight() - QPoint(m_reloadIcon.width() + 2, -2),
                                QPoint(rect().right() - 2, rect().top() + m_reloadIcon.height() + 2));
        if (clearRect.contains(mouseEvent->pos()))
        {
            if (cursor().shape() != Qt::ArrowCursor)
            {
                setCursor(Qt::ArrowCursor);
            }
        }
        else
        {
            if (cursor().shape() != Qt::IBeamCursor)
            {
                setCursor(Qt::IBeamCursor);
            }
        }
    }
#endif
}


void KeySequenceWidget::mousePressEvent(QMouseEvent* mouseEvent)
{
    QLineEdit::mousePressEvent(mouseEvent);

#ifdef UI_CLEAR_BUTTON
    if (m_isRecording)
    {
        QRect clearRect = QRect(rect().topRight() - QPoint(m_reloadIcon.width() + 2, -2),
                                QPoint(rect().right() - 2, rect().top() + m_reloadIcon.height() + 2));
        if (clearRect.contains(mouseEvent->pos()))
        {
            clearKeySequence();
            return;
        }
    }
#endif

    if (mouseEvent->button() == Qt::LeftButton)
    {
        startRecording();
    }
}

void KeySequenceWidget::paintEvent(QPaintEvent* event)
{
    QLineEdit::paintEvent(event);

    if (m_isRecording)
    {
        QPainter painter(this);

        if (text().isEmpty())
        {
            painter.setPen(Qt::darkGray);
            QRect r = rect();
            r.translate(5,3);
            painter.drawText(r, m_noneSequenceText);
        }

#ifdef UI_CLEAR_BUTTON
        if (!m_reloadIcon.isNull())
        {
            QRect clearRect = QRect(rect().topRight() - QPoint(m_reloadIcon.width() + 2, -2),
                                    QPoint(rect().right() - 2, rect().top() + m_reloadIcon.height() + 2));
            painter.drawPixmap(clearRect.topLeft(), m_reloadIcon);
        }
#endif
    }
}
