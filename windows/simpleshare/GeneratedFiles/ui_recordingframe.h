/********************************************************************************
** Form generated from reading UI file 'recordingframe.ui'
**
** Created: Mon Jun 24 20:09:05 2013
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RECORDINGFRAME_H
#define UI_RECORDINGFRAME_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_RecordingFrame
{
public:
    QVBoxLayout *verticalLayout;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *mutePushButton;
    QPushButton *stopRecordingPushButton;

    void setupUi(QWidget *RecordingFrame)
    {
        if (RecordingFrame->objectName().isEmpty())
            RecordingFrame->setObjectName(QString::fromUtf8("RecordingFrame"));
        RecordingFrame->resize(400, 300);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(RecordingFrame->sizePolicy().hasHeightForWidth());
        RecordingFrame->setSizePolicy(sizePolicy);
        RecordingFrame->setContextMenuPolicy(Qt::NoContextMenu);
        RecordingFrame->setWindowOpacity(1);
        RecordingFrame->setAutoFillBackground(false);
        RecordingFrame->setStyleSheet(QString::fromUtf8("background-color: rgb(0, 0, 0);"));
        verticalLayout = new QVBoxLayout(RecordingFrame);
        verticalLayout->setSpacing(2);
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalSpacer = new QSpacerItem(20, 266, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        mutePushButton = new QPushButton(RecordingFrame);
        mutePushButton->setObjectName(QString::fromUtf8("mutePushButton"));
        mutePushButton->setMinimumSize(QSize(0, 20));
        mutePushButton->setMaximumSize(QSize(50, 20));
        QPalette palette;
        QBrush brush(QColor(0, 0, 0, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Button, brush);
        palette.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        palette.setBrush(QPalette::Active, QPalette::Base, brush);
        palette.setBrush(QPalette::Active, QPalette::Window, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Button, brush);
        palette.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Window, brush);
        palette.setBrush(QPalette::Disabled, QPalette::Button, brush);
        QBrush brush1(QColor(118, 116, 108, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Disabled, QPalette::ButtonText, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush);
        palette.setBrush(QPalette::Disabled, QPalette::Window, brush);
        mutePushButton->setPalette(palette);
        mutePushButton->setCheckable(true);
        mutePushButton->setChecked(false);
        mutePushButton->setFlat(false);

        horizontalLayout->addWidget(mutePushButton);

        stopRecordingPushButton = new QPushButton(RecordingFrame);
        stopRecordingPushButton->setObjectName(QString::fromUtf8("stopRecordingPushButton"));
        stopRecordingPushButton->setMinimumSize(QSize(0, 20));
        stopRecordingPushButton->setMaximumSize(QSize(16777215, 20));
        stopRecordingPushButton->setFocusPolicy(Qt::NoFocus);
        stopRecordingPushButton->setAutoFillBackground(false);
        stopRecordingPushButton->setStyleSheet(QString::fromUtf8("background-color: rgb(0, 0, 0);\n"
"color: rgb(255, 255, 255);"));
        stopRecordingPushButton->setAutoDefault(false);
        stopRecordingPushButton->setDefault(false);
        stopRecordingPushButton->setFlat(true);

        horizontalLayout->addWidget(stopRecordingPushButton);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(RecordingFrame);

        QMetaObject::connectSlotsByName(RecordingFrame);
    } // setupUi

    void retranslateUi(QWidget *RecordingFrame)
    {
        RecordingFrame->setWindowTitle(QApplication::translate("RecordingFrame", "Recording Frame", 0, QApplication::UnicodeUTF8));
        mutePushButton->setText(QApplication::translate("RecordingFrame", "Mute", 0, QApplication::UnicodeUTF8));
        stopRecordingPushButton->setText(QApplication::translate("RecordingFrame", "Stop Recording", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class RecordingFrame: public Ui_RecordingFrame {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RECORDINGFRAME_H
