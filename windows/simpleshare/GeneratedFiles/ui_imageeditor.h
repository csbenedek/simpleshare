/********************************************************************************
** Form generated from reading UI file 'imageeditor.ui'
**
** Created: Mon Jun 24 20:09:05 2013
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_IMAGEEDITOR_H
#define UI_IMAGEEDITOR_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QFrame>
#include <QtGui/QGraphicsView>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "editablelabel.h"
#include "simplebutton.h"

QT_BEGIN_NAMESPACE

class Ui_ImageEditor
{
public:
    QAction *colorAction;
    QAction *actionShapeButton;
    QAction *actionSave;
    QVBoxLayout *verticalLayout;
    QWidget *toolbarWidget;
    QHBoxLayout *horizontalLayout;
    EditableLabel *imageNameLineEdit;
    QFrame *line_3;
    SimpleButton *colorToolButton;
    SimpleButton *highlightToolButton;
    SimpleButton *textToolButton;
    SimpleButton *arrowToolButton;
    SimpleButton *shapeToolButton;
    QFrame *line_2;
    SimpleButton *blurToolButton;
    SimpleButton *cropToolButton;
    QFrame *line;
    SimpleButton *saveToolButton;
    QGraphicsView *graphicsView;

    void setupUi(QWidget *ImageEditor)
    {
        if (ImageEditor->objectName().isEmpty())
            ImageEditor->setObjectName(QString::fromUtf8("ImageEditor"));
        ImageEditor->resize(350, 200);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(ImageEditor->sizePolicy().hasHeightForWidth());
        ImageEditor->setSizePolicy(sizePolicy);
        ImageEditor->setMinimumSize(QSize(350, 200));
        ImageEditor->setStyleSheet(QString::fromUtf8("QScrollBar:vertical { \n"
"color: white;\n"
"background-color: #333333;\n"
"}\n"
"\n"
"QScrollBar:horizontal { \n"
"color: white;\n"
"background-color: #333333;\n"
"}\n"
""));
        colorAction = new QAction(ImageEditor);
        colorAction->setObjectName(QString::fromUtf8("colorAction"));
        actionShapeButton = new QAction(ImageEditor);
        actionShapeButton->setObjectName(QString::fromUtf8("actionShapeButton"));
        actionSave = new QAction(ImageEditor);
        actionSave->setObjectName(QString::fromUtf8("actionSave"));
        verticalLayout = new QVBoxLayout(ImageEditor);
        verticalLayout->setSpacing(0);
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        toolbarWidget = new QWidget(ImageEditor);
        toolbarWidget->setObjectName(QString::fromUtf8("toolbarWidget"));
        toolbarWidget->setMinimumSize(QSize(0, 32));
        toolbarWidget->setMaximumSize(QSize(16777215, 32));
        QPalette palette;
        QBrush brush(QColor(0, 0, 0, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Light, brush);
        QBrush brush1(QColor(76, 74, 73, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Midlight, brush1);
        palette.setBrush(QPalette::Active, QPalette::Shadow, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Light, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Midlight, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Shadow, brush);
        palette.setBrush(QPalette::Disabled, QPalette::Light, brush);
        palette.setBrush(QPalette::Disabled, QPalette::Midlight, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Shadow, brush);
        toolbarWidget->setPalette(palette);
        toolbarWidget->setStyleSheet(QString::fromUtf8("background-image: url(:/res/bg_slice_gradient.png);"));
        horizontalLayout = new QHBoxLayout(toolbarWidget);
        horizontalLayout->setSpacing(2);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(8, 0, 8, 0);
        imageNameLineEdit = new EditableLabel(toolbarWidget);
        imageNameLineEdit->setObjectName(QString::fromUtf8("imageNameLineEdit"));
        imageNameLineEdit->setEnabled(true);
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(1);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(imageNameLineEdit->sizePolicy().hasHeightForWidth());
        imageNameLineEdit->setSizePolicy(sizePolicy1);
        QPalette palette1;
        QBrush brush2(QColor(255, 255, 255, 255));
        brush2.setStyle(Qt::SolidPattern);
        palette1.setBrush(QPalette::Active, QPalette::WindowText, brush2);
        palette1.setBrush(QPalette::Active, QPalette::Light, brush);
        palette1.setBrush(QPalette::Active, QPalette::Text, brush2);
        QBrush brush3(QColor(255, 0, 0, 255));
        brush3.setStyle(Qt::SolidPattern);
        palette1.setBrush(QPalette::Active, QPalette::Shadow, brush3);
        palette1.setBrush(QPalette::Inactive, QPalette::WindowText, brush2);
        palette1.setBrush(QPalette::Inactive, QPalette::Light, brush);
        palette1.setBrush(QPalette::Inactive, QPalette::Text, brush2);
        palette1.setBrush(QPalette::Inactive, QPalette::Shadow, brush3);
        QBrush brush4(QColor(117, 116, 113, 255));
        brush4.setStyle(Qt::SolidPattern);
        palette1.setBrush(QPalette::Disabled, QPalette::WindowText, brush4);
        palette1.setBrush(QPalette::Disabled, QPalette::Light, brush);
        palette1.setBrush(QPalette::Disabled, QPalette::Text, brush4);
        palette1.setBrush(QPalette::Disabled, QPalette::Shadow, brush3);
        imageNameLineEdit->setPalette(palette1);
        QFont font;
        font.setFamily(QString::fromUtf8("Arial"));
        font.setPointSize(12);
        imageNameLineEdit->setFont(font);
        imageNameLineEdit->setFrame(false);
        imageNameLineEdit->setReadOnly(true);

        horizontalLayout->addWidget(imageNameLineEdit);

        line_3 = new QFrame(toolbarWidget);
        line_3->setObjectName(QString::fromUtf8("line_3"));
        line_3->setMinimumSize(QSize(1, 16));
        line_3->setMaximumSize(QSize(1, 16));
        line_3->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 0);"));
        line_3->setFrameShadow(QFrame::Plain);
        line_3->setFrameShape(QFrame::VLine);

        horizontalLayout->addWidget(line_3);

        colorToolButton = new SimpleButton(toolbarWidget);
        colorToolButton->setObjectName(QString::fromUtf8("colorToolButton"));
        colorToolButton->setMinimumSize(QSize(22, 22));
        colorToolButton->setMaximumSize(QSize(22, 22));
        QPalette palette2;
        palette2.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette2.setBrush(QPalette::Active, QPalette::Button, brush);
        QBrush brush5(QColor(84, 84, 84, 255));
        brush5.setStyle(Qt::SolidPattern);
        palette2.setBrush(QPalette::Active, QPalette::Light, brush5);
        QBrush brush6(QColor(63, 63, 63, 255));
        brush6.setStyle(Qt::SolidPattern);
        palette2.setBrush(QPalette::Active, QPalette::Midlight, brush6);
        QBrush brush7(QColor(38, 38, 38, 255));
        brush7.setStyle(Qt::SolidPattern);
        palette2.setBrush(QPalette::Active, QPalette::Dark, brush7);
        QBrush brush8(QColor(43, 42, 41, 255));
        brush8.setStyle(Qt::SolidPattern);
        palette2.setBrush(QPalette::Active, QPalette::Mid, brush8);
        palette2.setBrush(QPalette::Active, QPalette::Text, brush);
        palette2.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        palette2.setBrush(QPalette::Active, QPalette::Shadow, brush);
        palette2.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette2.setBrush(QPalette::Inactive, QPalette::Button, brush);
        palette2.setBrush(QPalette::Inactive, QPalette::Light, brush5);
        palette2.setBrush(QPalette::Inactive, QPalette::Midlight, brush6);
        palette2.setBrush(QPalette::Inactive, QPalette::Dark, brush7);
        palette2.setBrush(QPalette::Inactive, QPalette::Mid, brush8);
        palette2.setBrush(QPalette::Inactive, QPalette::Text, brush);
        palette2.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        palette2.setBrush(QPalette::Inactive, QPalette::Shadow, brush);
        palette2.setBrush(QPalette::Disabled, QPalette::WindowText, brush7);
        palette2.setBrush(QPalette::Disabled, QPalette::Button, brush);
        palette2.setBrush(QPalette::Disabled, QPalette::Light, brush5);
        palette2.setBrush(QPalette::Disabled, QPalette::Midlight, brush6);
        palette2.setBrush(QPalette::Disabled, QPalette::Dark, brush7);
        palette2.setBrush(QPalette::Disabled, QPalette::Mid, brush8);
        palette2.setBrush(QPalette::Disabled, QPalette::Text, brush7);
        palette2.setBrush(QPalette::Disabled, QPalette::ButtonText, brush7);
        palette2.setBrush(QPalette::Disabled, QPalette::Shadow, brush);
        colorToolButton->setPalette(palette2);
        colorToolButton->setStyleSheet(QString::fromUtf8(""));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/res/16x16_square_outline.png"), QSize(), QIcon::Normal, QIcon::Off);
        colorToolButton->setIcon(icon);
        colorToolButton->setCheckable(false);
        colorToolButton->setChecked(false);
        colorToolButton->setPopupMode(QToolButton::InstantPopup);
        colorToolButton->setAutoRaise(true);

        horizontalLayout->addWidget(colorToolButton);

        highlightToolButton = new SimpleButton(toolbarWidget);
        highlightToolButton->setObjectName(QString::fromUtf8("highlightToolButton"));
        highlightToolButton->setMinimumSize(QSize(22, 22));
        highlightToolButton->setMaximumSize(QSize(22, 22));
        QPalette palette3;
        palette3.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette3.setBrush(QPalette::Active, QPalette::Button, brush);
        palette3.setBrush(QPalette::Active, QPalette::Light, brush5);
        palette3.setBrush(QPalette::Active, QPalette::Midlight, brush6);
        palette3.setBrush(QPalette::Active, QPalette::Dark, brush7);
        palette3.setBrush(QPalette::Active, QPalette::Mid, brush8);
        palette3.setBrush(QPalette::Active, QPalette::Text, brush);
        palette3.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        palette3.setBrush(QPalette::Active, QPalette::Shadow, brush);
        palette3.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette3.setBrush(QPalette::Inactive, QPalette::Button, brush);
        palette3.setBrush(QPalette::Inactive, QPalette::Light, brush5);
        palette3.setBrush(QPalette::Inactive, QPalette::Midlight, brush6);
        palette3.setBrush(QPalette::Inactive, QPalette::Dark, brush7);
        palette3.setBrush(QPalette::Inactive, QPalette::Mid, brush8);
        palette3.setBrush(QPalette::Inactive, QPalette::Text, brush);
        palette3.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        palette3.setBrush(QPalette::Inactive, QPalette::Shadow, brush);
        palette3.setBrush(QPalette::Disabled, QPalette::WindowText, brush7);
        palette3.setBrush(QPalette::Disabled, QPalette::Button, brush);
        palette3.setBrush(QPalette::Disabled, QPalette::Light, brush5);
        palette3.setBrush(QPalette::Disabled, QPalette::Midlight, brush6);
        palette3.setBrush(QPalette::Disabled, QPalette::Dark, brush7);
        palette3.setBrush(QPalette::Disabled, QPalette::Mid, brush8);
        palette3.setBrush(QPalette::Disabled, QPalette::Text, brush7);
        palette3.setBrush(QPalette::Disabled, QPalette::ButtonText, brush7);
        palette3.setBrush(QPalette::Disabled, QPalette::Shadow, brush);
        highlightToolButton->setPalette(palette3);
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/res/16x16_highlight.png"), QSize(), QIcon::Normal, QIcon::Off);
        highlightToolButton->setIcon(icon1);
        highlightToolButton->setCheckable(false);
        highlightToolButton->setPopupMode(QToolButton::InstantPopup);
        highlightToolButton->setAutoRaise(true);

        horizontalLayout->addWidget(highlightToolButton);

        textToolButton = new SimpleButton(toolbarWidget);
        textToolButton->setObjectName(QString::fromUtf8("textToolButton"));
        textToolButton->setMinimumSize(QSize(22, 22));
        textToolButton->setMaximumSize(QSize(22, 22));
        QPalette palette4;
        palette4.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette4.setBrush(QPalette::Active, QPalette::Button, brush);
        palette4.setBrush(QPalette::Active, QPalette::Light, brush5);
        palette4.setBrush(QPalette::Active, QPalette::Midlight, brush6);
        palette4.setBrush(QPalette::Active, QPalette::Dark, brush7);
        palette4.setBrush(QPalette::Active, QPalette::Mid, brush8);
        palette4.setBrush(QPalette::Active, QPalette::Text, brush);
        palette4.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        palette4.setBrush(QPalette::Active, QPalette::Shadow, brush);
        palette4.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette4.setBrush(QPalette::Inactive, QPalette::Button, brush);
        palette4.setBrush(QPalette::Inactive, QPalette::Light, brush5);
        palette4.setBrush(QPalette::Inactive, QPalette::Midlight, brush6);
        palette4.setBrush(QPalette::Inactive, QPalette::Dark, brush7);
        palette4.setBrush(QPalette::Inactive, QPalette::Mid, brush8);
        palette4.setBrush(QPalette::Inactive, QPalette::Text, brush);
        palette4.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        palette4.setBrush(QPalette::Inactive, QPalette::Shadow, brush);
        palette4.setBrush(QPalette::Disabled, QPalette::WindowText, brush7);
        palette4.setBrush(QPalette::Disabled, QPalette::Button, brush);
        palette4.setBrush(QPalette::Disabled, QPalette::Light, brush5);
        palette4.setBrush(QPalette::Disabled, QPalette::Midlight, brush6);
        palette4.setBrush(QPalette::Disabled, QPalette::Dark, brush7);
        palette4.setBrush(QPalette::Disabled, QPalette::Mid, brush8);
        palette4.setBrush(QPalette::Disabled, QPalette::Text, brush7);
        palette4.setBrush(QPalette::Disabled, QPalette::ButtonText, brush7);
        palette4.setBrush(QPalette::Disabled, QPalette::Shadow, brush);
        textToolButton->setPalette(palette4);
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/res/16x16_text.png"), QSize(), QIcon::Normal, QIcon::Off);
        textToolButton->setIcon(icon2);
        textToolButton->setCheckable(false);
        textToolButton->setAutoRaise(true);

        horizontalLayout->addWidget(textToolButton);

        arrowToolButton = new SimpleButton(toolbarWidget);
        arrowToolButton->setObjectName(QString::fromUtf8("arrowToolButton"));
        arrowToolButton->setMinimumSize(QSize(22, 22));
        arrowToolButton->setMaximumSize(QSize(22, 22));
        QPalette palette5;
        palette5.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette5.setBrush(QPalette::Active, QPalette::Button, brush);
        palette5.setBrush(QPalette::Active, QPalette::Light, brush5);
        palette5.setBrush(QPalette::Active, QPalette::Midlight, brush6);
        palette5.setBrush(QPalette::Active, QPalette::Dark, brush7);
        palette5.setBrush(QPalette::Active, QPalette::Mid, brush8);
        palette5.setBrush(QPalette::Active, QPalette::Text, brush);
        palette5.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        palette5.setBrush(QPalette::Active, QPalette::Shadow, brush);
        palette5.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette5.setBrush(QPalette::Inactive, QPalette::Button, brush);
        palette5.setBrush(QPalette::Inactive, QPalette::Light, brush5);
        palette5.setBrush(QPalette::Inactive, QPalette::Midlight, brush6);
        palette5.setBrush(QPalette::Inactive, QPalette::Dark, brush7);
        palette5.setBrush(QPalette::Inactive, QPalette::Mid, brush8);
        palette5.setBrush(QPalette::Inactive, QPalette::Text, brush);
        palette5.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        palette5.setBrush(QPalette::Inactive, QPalette::Shadow, brush);
        palette5.setBrush(QPalette::Disabled, QPalette::WindowText, brush7);
        palette5.setBrush(QPalette::Disabled, QPalette::Button, brush);
        palette5.setBrush(QPalette::Disabled, QPalette::Light, brush5);
        palette5.setBrush(QPalette::Disabled, QPalette::Midlight, brush6);
        palette5.setBrush(QPalette::Disabled, QPalette::Dark, brush7);
        palette5.setBrush(QPalette::Disabled, QPalette::Mid, brush8);
        palette5.setBrush(QPalette::Disabled, QPalette::Text, brush7);
        palette5.setBrush(QPalette::Disabled, QPalette::ButtonText, brush7);
        palette5.setBrush(QPalette::Disabled, QPalette::Shadow, brush);
        arrowToolButton->setPalette(palette5);
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/res/16x16_arrow.png"), QSize(), QIcon::Normal, QIcon::Off);
        arrowToolButton->setIcon(icon3);
        arrowToolButton->setCheckable(false);
        arrowToolButton->setAutoRaise(true);

        horizontalLayout->addWidget(arrowToolButton);

        shapeToolButton = new SimpleButton(toolbarWidget);
        shapeToolButton->setObjectName(QString::fromUtf8("shapeToolButton"));
        shapeToolButton->setMinimumSize(QSize(22, 22));
        shapeToolButton->setMaximumSize(QSize(22, 22));
        QPalette palette6;
        palette6.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette6.setBrush(QPalette::Active, QPalette::Button, brush);
        palette6.setBrush(QPalette::Active, QPalette::Light, brush5);
        palette6.setBrush(QPalette::Active, QPalette::Midlight, brush6);
        palette6.setBrush(QPalette::Active, QPalette::Dark, brush7);
        palette6.setBrush(QPalette::Active, QPalette::Mid, brush8);
        palette6.setBrush(QPalette::Active, QPalette::Text, brush);
        palette6.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        palette6.setBrush(QPalette::Active, QPalette::Shadow, brush);
        palette6.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette6.setBrush(QPalette::Inactive, QPalette::Button, brush);
        palette6.setBrush(QPalette::Inactive, QPalette::Light, brush5);
        palette6.setBrush(QPalette::Inactive, QPalette::Midlight, brush6);
        palette6.setBrush(QPalette::Inactive, QPalette::Dark, brush7);
        palette6.setBrush(QPalette::Inactive, QPalette::Mid, brush8);
        palette6.setBrush(QPalette::Inactive, QPalette::Text, brush);
        palette6.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        palette6.setBrush(QPalette::Inactive, QPalette::Shadow, brush);
        palette6.setBrush(QPalette::Disabled, QPalette::WindowText, brush7);
        palette6.setBrush(QPalette::Disabled, QPalette::Button, brush);
        palette6.setBrush(QPalette::Disabled, QPalette::Light, brush5);
        palette6.setBrush(QPalette::Disabled, QPalette::Midlight, brush6);
        palette6.setBrush(QPalette::Disabled, QPalette::Dark, brush7);
        palette6.setBrush(QPalette::Disabled, QPalette::Mid, brush8);
        palette6.setBrush(QPalette::Disabled, QPalette::Text, brush7);
        palette6.setBrush(QPalette::Disabled, QPalette::ButtonText, brush7);
        palette6.setBrush(QPalette::Disabled, QPalette::Shadow, brush);
        shapeToolButton->setPalette(palette6);
        shapeToolButton->setStyleSheet(QString::fromUtf8(""));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/res/16x16_circle_outline.png"), QSize(), QIcon::Normal, QIcon::Off);
        shapeToolButton->setIcon(icon4);
        shapeToolButton->setCheckable(false);
        shapeToolButton->setPopupMode(QToolButton::InstantPopup);
        shapeToolButton->setAutoRaise(true);

        horizontalLayout->addWidget(shapeToolButton);

        line_2 = new QFrame(toolbarWidget);
        line_2->setObjectName(QString::fromUtf8("line_2"));
        line_2->setMinimumSize(QSize(1, 16));
        line_2->setMaximumSize(QSize(1, 16));
        line_2->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 0);"));
        line_2->setFrameShadow(QFrame::Plain);
        line_2->setFrameShape(QFrame::VLine);

        horizontalLayout->addWidget(line_2);

        blurToolButton = new SimpleButton(toolbarWidget);
        blurToolButton->setObjectName(QString::fromUtf8("blurToolButton"));
        blurToolButton->setMinimumSize(QSize(22, 22));
        blurToolButton->setMaximumSize(QSize(22, 22));
        QPalette palette7;
        palette7.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette7.setBrush(QPalette::Active, QPalette::Button, brush);
        palette7.setBrush(QPalette::Active, QPalette::Light, brush5);
        palette7.setBrush(QPalette::Active, QPalette::Midlight, brush6);
        palette7.setBrush(QPalette::Active, QPalette::Dark, brush7);
        palette7.setBrush(QPalette::Active, QPalette::Mid, brush8);
        palette7.setBrush(QPalette::Active, QPalette::Text, brush);
        palette7.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        palette7.setBrush(QPalette::Active, QPalette::Shadow, brush);
        palette7.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette7.setBrush(QPalette::Inactive, QPalette::Button, brush);
        palette7.setBrush(QPalette::Inactive, QPalette::Light, brush5);
        palette7.setBrush(QPalette::Inactive, QPalette::Midlight, brush6);
        palette7.setBrush(QPalette::Inactive, QPalette::Dark, brush7);
        palette7.setBrush(QPalette::Inactive, QPalette::Mid, brush8);
        palette7.setBrush(QPalette::Inactive, QPalette::Text, brush);
        palette7.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        palette7.setBrush(QPalette::Inactive, QPalette::Shadow, brush);
        palette7.setBrush(QPalette::Disabled, QPalette::WindowText, brush7);
        palette7.setBrush(QPalette::Disabled, QPalette::Button, brush);
        palette7.setBrush(QPalette::Disabled, QPalette::Light, brush5);
        palette7.setBrush(QPalette::Disabled, QPalette::Midlight, brush6);
        palette7.setBrush(QPalette::Disabled, QPalette::Dark, brush7);
        palette7.setBrush(QPalette::Disabled, QPalette::Mid, brush8);
        palette7.setBrush(QPalette::Disabled, QPalette::Text, brush7);
        palette7.setBrush(QPalette::Disabled, QPalette::ButtonText, brush7);
        palette7.setBrush(QPalette::Disabled, QPalette::Shadow, brush);
        blurToolButton->setPalette(palette7);
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/res/16x16_blur.png"), QSize(), QIcon::Normal, QIcon::Off);
        blurToolButton->setIcon(icon5);
        blurToolButton->setCheckable(false);
        blurToolButton->setPopupMode(QToolButton::InstantPopup);
        blurToolButton->setAutoRaise(true);

        horizontalLayout->addWidget(blurToolButton);

        cropToolButton = new SimpleButton(toolbarWidget);
        cropToolButton->setObjectName(QString::fromUtf8("cropToolButton"));
        cropToolButton->setMinimumSize(QSize(22, 22));
        cropToolButton->setMaximumSize(QSize(22, 22));
        QPalette palette8;
        palette8.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette8.setBrush(QPalette::Active, QPalette::Button, brush);
        palette8.setBrush(QPalette::Active, QPalette::Light, brush5);
        palette8.setBrush(QPalette::Active, QPalette::Midlight, brush6);
        palette8.setBrush(QPalette::Active, QPalette::Dark, brush7);
        palette8.setBrush(QPalette::Active, QPalette::Mid, brush8);
        palette8.setBrush(QPalette::Active, QPalette::Text, brush);
        palette8.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        palette8.setBrush(QPalette::Active, QPalette::Shadow, brush);
        palette8.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette8.setBrush(QPalette::Inactive, QPalette::Button, brush);
        palette8.setBrush(QPalette::Inactive, QPalette::Light, brush5);
        palette8.setBrush(QPalette::Inactive, QPalette::Midlight, brush6);
        palette8.setBrush(QPalette::Inactive, QPalette::Dark, brush7);
        palette8.setBrush(QPalette::Inactive, QPalette::Mid, brush8);
        palette8.setBrush(QPalette::Inactive, QPalette::Text, brush);
        palette8.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        palette8.setBrush(QPalette::Inactive, QPalette::Shadow, brush);
        palette8.setBrush(QPalette::Disabled, QPalette::WindowText, brush7);
        palette8.setBrush(QPalette::Disabled, QPalette::Button, brush);
        palette8.setBrush(QPalette::Disabled, QPalette::Light, brush5);
        palette8.setBrush(QPalette::Disabled, QPalette::Midlight, brush6);
        palette8.setBrush(QPalette::Disabled, QPalette::Dark, brush7);
        palette8.setBrush(QPalette::Disabled, QPalette::Mid, brush8);
        palette8.setBrush(QPalette::Disabled, QPalette::Text, brush7);
        palette8.setBrush(QPalette::Disabled, QPalette::ButtonText, brush7);
        palette8.setBrush(QPalette::Disabled, QPalette::Shadow, brush);
        cropToolButton->setPalette(palette8);
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/res/16x16_crop.png"), QSize(), QIcon::Normal, QIcon::Off);
        cropToolButton->setIcon(icon6);
        cropToolButton->setCheckable(false);
        cropToolButton->setPopupMode(QToolButton::DelayedPopup);
        cropToolButton->setAutoRaise(true);

        horizontalLayout->addWidget(cropToolButton);

        line = new QFrame(toolbarWidget);
        line->setObjectName(QString::fromUtf8("line"));
        line->setMinimumSize(QSize(1, 16));
        line->setMaximumSize(QSize(1, 16));
        line->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 0);"));
        line->setFrameShadow(QFrame::Plain);
        line->setFrameShape(QFrame::VLine);

        horizontalLayout->addWidget(line);

        saveToolButton = new SimpleButton(toolbarWidget);
        saveToolButton->setObjectName(QString::fromUtf8("saveToolButton"));
        saveToolButton->setMinimumSize(QSize(0, 22));
        saveToolButton->setMaximumSize(QSize(16777215, 22));
        QPalette palette9;
        palette9.setBrush(QPalette::Active, QPalette::WindowText, brush2);
        palette9.setBrush(QPalette::Active, QPalette::Button, brush);
        palette9.setBrush(QPalette::Active, QPalette::Light, brush5);
        palette9.setBrush(QPalette::Active, QPalette::Midlight, brush6);
        palette9.setBrush(QPalette::Active, QPalette::Dark, brush7);
        palette9.setBrush(QPalette::Active, QPalette::Mid, brush8);
        palette9.setBrush(QPalette::Active, QPalette::Text, brush2);
        palette9.setBrush(QPalette::Active, QPalette::ButtonText, brush2);
        palette9.setBrush(QPalette::Active, QPalette::Shadow, brush);
        palette9.setBrush(QPalette::Inactive, QPalette::WindowText, brush2);
        palette9.setBrush(QPalette::Inactive, QPalette::Button, brush);
        palette9.setBrush(QPalette::Inactive, QPalette::Light, brush5);
        palette9.setBrush(QPalette::Inactive, QPalette::Midlight, brush6);
        palette9.setBrush(QPalette::Inactive, QPalette::Dark, brush7);
        palette9.setBrush(QPalette::Inactive, QPalette::Mid, brush8);
        palette9.setBrush(QPalette::Inactive, QPalette::Text, brush2);
        palette9.setBrush(QPalette::Inactive, QPalette::ButtonText, brush2);
        palette9.setBrush(QPalette::Inactive, QPalette::Shadow, brush);
        palette9.setBrush(QPalette::Disabled, QPalette::WindowText, brush7);
        palette9.setBrush(QPalette::Disabled, QPalette::Button, brush);
        palette9.setBrush(QPalette::Disabled, QPalette::Light, brush5);
        palette9.setBrush(QPalette::Disabled, QPalette::Midlight, brush6);
        palette9.setBrush(QPalette::Disabled, QPalette::Dark, brush7);
        palette9.setBrush(QPalette::Disabled, QPalette::Mid, brush8);
        palette9.setBrush(QPalette::Disabled, QPalette::Text, brush7);
        palette9.setBrush(QPalette::Disabled, QPalette::ButtonText, brush7);
        palette9.setBrush(QPalette::Disabled, QPalette::Shadow, brush);
        saveToolButton->setPalette(palette9);
        QFont font1;
        font1.setFamily(QString::fromUtf8("Arial"));
        font1.setPointSize(13);
        font1.setBold(true);
        font1.setWeight(75);
        saveToolButton->setFont(font1);
        saveToolButton->setStyleSheet(QString::fromUtf8(""));
        saveToolButton->setPopupMode(QToolButton::InstantPopup);
        saveToolButton->setAutoRaise(true);

        horizontalLayout->addWidget(saveToolButton);


        verticalLayout->addWidget(toolbarWidget);

        graphicsView = new QGraphicsView(ImageEditor);
        graphicsView->setObjectName(QString::fromUtf8("graphicsView"));
        QPalette palette10;
        palette10.setBrush(QPalette::Active, QPalette::Base, brush2);
        palette10.setBrush(QPalette::Active, QPalette::Window, brush);
        palette10.setBrush(QPalette::Inactive, QPalette::Base, brush2);
        palette10.setBrush(QPalette::Inactive, QPalette::Window, brush);
        palette10.setBrush(QPalette::Disabled, QPalette::Base, brush);
        palette10.setBrush(QPalette::Disabled, QPalette::Window, brush);
        graphicsView->setPalette(palette10);
        graphicsView->viewport()->setProperty("cursor", QVariant(QCursor(Qt::CrossCursor)));
        graphicsView->setStyleSheet(QString::fromUtf8("border: 4px solid #262626;"));
        graphicsView->setFrameShape(QFrame::Box);
        graphicsView->setFrameShadow(QFrame::Plain);
        graphicsView->setLineWidth(4);
        graphicsView->setMidLineWidth(0);
        QBrush brush9(QColor(38, 38, 38, 255));
        brush9.setStyle(Qt::NoBrush);
        graphicsView->setBackgroundBrush(brush9);
        graphicsView->setAlignment(Qt::AlignCenter);
        graphicsView->setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing);
        graphicsView->setResizeAnchor(QGraphicsView::AnchorViewCenter);

        verticalLayout->addWidget(graphicsView);


        retranslateUi(ImageEditor);

        QMetaObject::connectSlotsByName(ImageEditor);
    } // setupUi

    void retranslateUi(QWidget *ImageEditor)
    {
        ImageEditor->setWindowTitle(QApplication::translate("ImageEditor", "Box SimpleShare - Annotate", 0, QApplication::UnicodeUTF8));
        colorAction->setText(QString());
#ifndef QT_NO_TOOLTIP
        colorAction->setToolTip(QApplication::translate("ImageEditor", "Color", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionShapeButton->setText(QString());
        actionSave->setText(QApplication::translate("ImageEditor", "Save", 0, QApplication::UnicodeUTF8));
        imageNameLineEdit->setText(QApplication::translate("ImageEditor", "NAME", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        colorToolButton->setToolTip(QApplication::translate("ImageEditor", "Color", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        colorToolButton->setText(QString());
#ifndef QT_NO_TOOLTIP
        highlightToolButton->setToolTip(QApplication::translate("ImageEditor", "Highliter tool", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        highlightToolButton->setText(QString());
#ifndef QT_NO_TOOLTIP
        textToolButton->setToolTip(QApplication::translate("ImageEditor", "Text tool", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        textToolButton->setText(QString());
#ifndef QT_NO_TOOLTIP
        arrowToolButton->setToolTip(QApplication::translate("ImageEditor", "Draw Arrow", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        arrowToolButton->setText(QApplication::translate("ImageEditor", "Arrow", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        shapeToolButton->setToolTip(QApplication::translate("ImageEditor", "Draw Circle", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        shapeToolButton->setText(QString());
#ifndef QT_NO_TOOLTIP
        blurToolButton->setToolTip(QApplication::translate("ImageEditor", "Blur tool", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        blurToolButton->setText(QString());
#ifndef QT_NO_TOOLTIP
        cropToolButton->setToolTip(QApplication::translate("ImageEditor", "Crop tool", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        cropToolButton->setText(QApplication::translate("ImageEditor", "Crop", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        saveToolButton->setToolTip(QApplication::translate("ImageEditor", "Saving image to Box.net", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        saveToolButton->setText(QApplication::translate("ImageEditor", "Save", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ImageEditor: public Ui_ImageEditor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_IMAGEEDITOR_H
