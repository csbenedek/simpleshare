/* imageeditor.cpp
 * Author: Evgeniy Sergeev, <evgeniy.sereev@gmail.com>
 */

#include "imageeditor.h"
#include "ui_imageeditor.h"
#include <QFileInfo>
#include <QDesktopServices>
#include <QDir>
#include <QFileDialog>
#include <QPlastiqueStyle>
#include <QSettings>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QCloseEvent>

ImageEditor::ImageEditor(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ImageEditor)
    , m_saveMode(0)
    , m_currentShapeAction(NULL)
    , m_currentMode(ImageEditorScene::ItemEmptyEllipse)
{
    ui->setupUi(this);

    m_defaultCursor = new QCursor(QPixmap(":/res/point_selector.png"));
    ui->graphicsView->viewport()->setCursor(*m_defaultCursor);

    m_scene = new ImageEditorScene(this);
    m_scene->setBackgroundBrush(QColor(38,38,38));

    connect(m_scene, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
    connect(m_scene, SIGNAL(modeChanged(int)), this, SLOT(onModeChanged(int)));
    connect(m_scene, SIGNAL(newItemCreated(QGraphicsItem*)), this, SLOT(onNewItemCreated(QGraphicsItem*)));

    initPalette();

    initShapeButton();

    ui->colorToolButton->setMenu(m_paletteMenu);
    ui->colorToolButton->setDefaultAction(ui->colorAction);
    ui->shapeToolButton->setMenu(m_shapeMenu);
    //ui->shapeToolButton->setDefaultAction(m_shapeActions[0]);
    ui->shapeToolButton->setDefaultAction(ui->actionShapeButton);

    //updateColorButton();
    loadSettings();

    m_scene->setMode(ImageEditorScene::Select);

    if (m_dirName.isEmpty())
    {
        m_dirName = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
    }

    initSaveMenu();

    ui->saveToolButton->setMenu(m_saveMenu);
    ui->saveToolButton->setDefaultAction(ui->actionSave);

    ui->graphicsView->setScene(m_scene);
}

ImageEditor::~ImageEditor()
{
    ui->graphicsView->viewport()->setCursor(Qt::ArrowCursor);
    delete m_defaultCursor;

    saveSettings();

    delete ui;
    delete m_style;
}

void ImageEditor::saveSettings()
{
    QSettings settings;
    settings.setValue("imageeditor/saveMode", m_saveMode);
    for(int i = 0; i < 9; ++i)
    {
        if (m_shapeActions[i] == m_currentShapeAction)
        {
            settings.setValue("imageeditor/shape", i);
            break;
        }
    }
    int c = (m_scene->fgColor().toRgb().red() << 16) + (m_scene->fgColor().toRgb().green() << 8) + m_scene->fgColor().toRgb().blue();
    settings.setValue("imageeditor/color", c);
    settings.setValue("imageeditor/directory", m_dirName);
}

void ImageEditor::loadSettings()
{
    QSettings settings;
    m_saveMode          = settings.value("imageeditor/saveMode", m_saveMode).toInt();

    int i = settings.value("imageeditor/shape", 0).toInt();
    if (i < 9)
    {
        m_shapeActions[i]->activate(QAction::Trigger);
    }

    int c = (m_scene->fgColor().toRgb().red() << 16) + (m_scene->fgColor().toRgb().green() << 8) + m_scene->fgColor().toRgb().blue();
    QColor color        = QRgb(settings.value("imageeditor/color", c).toInt());

    m_scene->setFgColor(color);
    m_scene->setBgColor(color);
    m_scene->setTextColor(color);

    m_dirName = settings.value("imageeditor/directory").toString();

    updateColorButton();
}

void ImageEditor::setPixmap(const QPixmap& pixmap, const QString& imageName)
{
    m_resultPixmap = QPixmap();

    QSize delta = frameSize() - size();
    delta.setHeight(delta.height() / 2 + ui->toolbarWidget->height() + 8);
    delta.setWidth(delta.width() / 2 + 8);

    QSize sz = pixmap.size() + delta;

    QSize screenSize = QApplication::desktop()->screen()->geometry().size();

    QPoint p (QCursor::pos().x() - sz.width(), QCursor::pos().y() - sz.height());
    if (p.x() < 0)
    {
        p.setX(0);
    }
    if (p.y() < 0)
    {
        p.setY(0);
    }

    p += QPoint(8, 8); // offset it or user will be confused. (changes on screen a little)

    resize(100,100);
    hide();
    if (sz.width() >= screenSize.width() || sz.height() >= screenSize.height())
    {
        showMaximized();
    }
    else
    {
        showNormal();
        resize(sz);
        move(p);
    }

    setImageName(imageName);
    m_scene->setPixmap(pixmap);
}

ImageEditorScene* ImageEditor::scene() const
{
    return m_scene;
}

void ImageEditor::clear()
{
    m_imageName.clear();
    m_scene->setPixmap(QPixmap());
}

QPixmap& ImageEditor::resultPixmap()
{
    QPixmap result(m_scene->width(), m_scene->height());
    QPainter painter(&result);
    m_scene->render(&painter);
    m_resultPixmap = result;
    return m_resultPixmap;
}

void ImageEditor::onUploadClicked()
{   
    m_scene->cancelCrop();
    m_scene->clearSelection();
    m_scene->clearFocus();

    switch(m_saveMode)
    {
    case 0:
        emit uploadToNet(resultPixmap(), false, false);
        break;
    case 1:
        emit saveToFile(resultPixmap(), QDesktopServices::storageLocation(QDesktopServices::DesktopLocation) + "/" + QFileInfo(m_imageName).fileName());
        break;
    case 2:
        emit saveToFile(resultPixmap(), m_dirName + "/" + QFileInfo(m_imageName).fileName());
        break;
    case 3:
        emit copyToClipboard(resultPixmap());
        break;
    }
}

void ImageEditor::onSaveToBox()
{
    m_saveMode = 0;
    updateActionIcon();

    onUploadClicked();
}

void ImageEditor::onSaveToDesktop()
{
    m_saveMode = 1;
    updateActionIcon();

    onUploadClicked();
}

void ImageEditor::onSaveToDirectory()
{
    m_saveMode = 2;
    updateActionIcon();

    m_dirName = QFileDialog::getExistingDirectory (this, tr("Select directory"), m_dirName);
    onUploadClicked();
}

void ImageEditor::onSaveToClipboard()
{
    m_saveMode = 3;
    updateActionIcon();

    onCopyToClipboardClicked();
}

void ImageEditor::onSaveToFileClicked()
{
    m_scene->cancelCrop();
    m_scene->clearSelection();
    m_scene->clearFocus();
    emit saveToFile(resultPixmap(), "");
}

void ImageEditor::onCopyToClipboardClicked()
{
    m_scene->cancelCrop();
    m_scene->clearSelection();
    m_scene->clearFocus();
    emit copyToClipboard(resultPixmap());
}

void ImageEditor::onTwitterClicked()
{
    m_scene->cancelCrop();
    m_scene->clearSelection();
    m_scene->clearFocus();
    emit uploadToNet(resultPixmap(), false, true);
}

void ImageEditor::onFacebookClicked()
{
    m_scene->cancelCrop();
    m_scene->clearSelection();
    m_scene->clearFocus();

    emit uploadToNet(resultPixmap(), true, false);
}


void ImageEditor::onNewItemCreated(QGraphicsItem* item)
{
    Q_UNUSED(item);
}

void ImageEditor::onSelectionChanged()
{
    Q_ASSERT(m_scene);
    if (m_scene)
    {
        int count = m_scene->selectedItems().count();
        if (count == 1)
        {
            m_scene->bringToFront(m_scene->selectedItems().at(0));
        }
    }
}


QString ImageEditor::imageName() const
{
    return m_imageName;
}

void ImageEditor::setImageName(const QString& name)
{
    m_imageName = name;

    QFileInfo fileInfo(name);

//  setWindowTitle(fileInfo.fileName()
//                   + (fileInfo.fileName().isEmpty()?"":" - ")
//                   + tr("Box SimpleShare - Image Editor"));

    ui->imageNameLineEdit->setText(fileInfo.completeBaseName());
}

void ImageEditor::on_arrowToolButton_clicked()
{
    m_scene->cancelCrop();
    m_scene->clearFocus();
    m_scene->clearSelection();

    if (m_scene->mode() == ImageEditorScene::ItemArrow)
    {
        m_scene->setMode(ImageEditorScene::Select);
    }
    else
    {
        m_scene->setMode(ImageEditorScene::ItemArrow);
    }
}

void ImageEditor::on_highlightToolButton_clicked()
{
    m_scene->cancelCrop();
    m_scene->clearFocus();
    m_scene->clearSelection();

    if (m_scene->mode() == ImageEditorScene::ItemPen)
    {
        m_scene->setMode(ImageEditorScene::Select);
    }
    else
    {
        m_scene->setMode(ImageEditorScene::ItemPen);
    }
}

void ImageEditor::on_textToolButton_clicked()
{
    m_scene->cancelCrop();
    m_scene->clearFocus();
    m_scene->clearSelection();

    if (m_scene->mode() == ImageEditorScene::ItemText)
    {
        m_scene->setMode(ImageEditorScene::Select);
    }
    else
    {
        m_scene->setMode(ImageEditorScene::ItemText);
    }
}

void ImageEditor::on_blurToolButton_clicked()
{
    m_scene->cancelCrop();
    m_scene->clearFocus();
    m_scene->clearSelection();

    if (m_scene->mode() == ImageEditorScene::ItemBlur)
    {
        m_scene->setMode(ImageEditorScene::Select);
    }
    else
    {
        m_scene->setMode(ImageEditorScene::ItemBlur);
    }
}

void ImageEditor::on_cropToolButton_clicked()
{
    m_scene->clearFocus();
    m_scene->clearSelection();

    if (m_scene->mode() == ImageEditorScene::Crop)
    {
        m_scene->setMode(ImageEditorScene::Select);
    }
    else
    {
        m_scene->setMode(ImageEditorScene::Crop);
    }
}

void ImageEditor::on_colorToolButton_clicked()
{
    QColor color = m_scene->fgColor();

    m_scene->setFgColor(color);
    m_scene->setBgColor(color);
    m_scene->setTextColor(color);

    updateColorButton();
}

QIcon ImageEditor::createColorIcon(QColor color, bool border)
{
    QPixmap pixmap(border?16:14, border?14:12);
    QPainter painter(&pixmap);
    painter.fillRect(pixmap.rect(), color);
    if (border)
    {
        painter.setPen(QPen(Qt::black, 1));
        painter.drawRect(QRect(0,0, 15,13));
    }

    return QIcon(pixmap);
}

void ImageEditor::updateColorButton()
{
    ui->colorToolButton->setIcon(createColorIcon(m_scene->fgColor()));

    if (m_scene->mode() == ImageEditorScene::ItemPen)
    {
        ui->graphicsView->viewport()->setCursor(QCursor(createHighlightCursor()));
    }
}

void ImageEditor::onModeChanged(int mode)
{
    if (mode == ImageEditorScene::ItemPen)
    {
        ui->graphicsView->viewport()->setCursor(QCursor(createHighlightCursor()));
    }
    else
    {
        ui->graphicsView->viewport()->setCursor(*m_defaultCursor);
    }

    QToolButton* button = NULL;
    switch(mode)
    {
    case ImageEditorScene::ItemPen:
        button = ui->highlightToolButton;
        break;
    case ImageEditorScene::ItemText:
        button = ui->textToolButton;
        break;
    case ImageEditorScene::ItemArrow:
        button = ui->arrowToolButton;
        break;
    case ImageEditorScene::ItemEmptyRect:
    case ImageEditorScene::ItemRect:
    case ImageEditorScene::ItemEmptyEllipse:
    case ImageEditorScene::ItemEllipse:
    case ImageEditorScene::ItemStar:
    case ImageEditorScene::ItemEmptyStar:
    case ImageEditorScene::ItemImage:
    case ImageEditorScene::ItemLine:
        button = ui->shapeToolButton;
        break;
    case ImageEditorScene::ItemBlur:
        button = ui->blurToolButton;
        break;
    case ImageEditorScene::Crop:
        button = ui->cropToolButton;
        break;
    }

    ui->highlightToolButton->setCurrentTool(button == ui->highlightToolButton);
    ui->textToolButton->setCurrentTool(button == ui->textToolButton);
    ui->arrowToolButton->setCurrentTool(button == ui->arrowToolButton);
    ui->shapeToolButton->setCurrentTool(button == ui->shapeToolButton);
    ui->blurToolButton->setCurrentTool(button == ui->blurToolButton);
    ui->cropToolButton->setCurrentTool(button == ui->cropToolButton);
}

void ImageEditor::onSetColor()
{
    QAction* action = qobject_cast<QAction*>(sender());
    Q_ASSERT(action);
    if (action)
    {
        QColor color = action->data().toInt();
        m_scene->setFgColor(color);
        m_scene->setBgColor(color);
        m_scene->setTextColor(color);

        updateColorButton();
    }
}

#define countof(a) (sizeof(a) / sizeof(a[0]))

void ImageEditor::initPalette()
{
    static int s_colors[] = {
            0x000000,  0x853d00,  0x333200,  0x193000,  0x143366,  0x000c80,  0x2d379a,  0x323232,
            0x6d1b00,  0xe17500,  0x817f00,  0x497b00,  0x467d80,  0x0025ff,  0x646799,  0x808080,
            0xdb4102,  0xe89f00,  0xacc800,  0x619565,  0x7bc7cc,  0x3a6aff,  0x6b2181,  0x999999,
            0xd84cff,  0xf3ce00,  0xfffd00,  0x97f700,  0x92f9ff,  0x71c8ff,  0x853f66,  0xc0c0c0,
            0xe7a2cd,  0xf2ce98,  0xfffd97,  0xdefccb,  0xdcfdff,  0xa8cbff,  0xbd9fff,  0xffffff };


    m_paletteMenu = new SimpleMenuEx(ui->colorToolButton, this);

    for(unsigned int i = 0; i < countof(s_colors); ++i)
    {
        QAction* action = m_paletteMenu->addAction(createColorIcon(s_colors[i], false), "", this, SLOT(onSetColor()));
        action->setData(s_colors[i]);
    }

    m_paletteMenu->setColumnCount(8);
}

void ImageEditor::setShapeFromAction(QAction* action)
{
    Q_ASSERT(action);
    if (action)
    {
        m_currentShapeAction  = action;
        action->setChecked(true);

        int mode = action->data().toInt();

        if (mode == ItemIcanHas)
        {
            m_currentMode = ImageEditorScene::ItemImage;
            m_currentItemImage = ":/res/shape_icanhas.png";
        }
        else if (mode == ItemMustache)
        {
            m_currentMode = ImageEditorScene::ItemImage;
            m_currentItemImage = ":/res/shape_mustache.png";
        }
        else
        {
            m_currentMode = static_cast<ImageEditorScene::Mode>(mode);
        }
        ui->shapeToolButton->setIcon(action->icon());
        ui->shapeToolButton->setToolTip(action->toolTip());
        //ui->shapeToolButton->setDefaultAction(action);
    }
}

void ImageEditor::onSetShape()
{
    QAction* action = qobject_cast<QAction*>(sender());
    Q_ASSERT(action);
    if (action)
    {
        setShapeFromAction(action);

        m_scene->cancelCrop();
        m_scene->clearFocus();
        m_scene->clearSelection();

        if (!m_currentItemImage.isEmpty())
        {
            m_scene->setItemImage(m_currentItemImage);
        }
        if (m_scene->mode() == m_currentMode)
        {
            m_currentMode = ImageEditorScene::Select;
        }
        m_scene->setMode(m_currentMode);
    }
}

void ImageEditor::onSetShapeMenu()
{
    QAction* action = qobject_cast<QAction*>(sender());

    Q_ASSERT(action);
    if (action)
    {
        setShapeFromAction(action);

        m_scene->cancelCrop();
        m_scene->clearFocus();
        m_scene->clearSelection();

        if (!m_currentItemImage.isEmpty())
        {
            m_scene->setItemImage(m_currentItemImage);
        }

        m_scene->setMode(m_currentMode);
    }
}

void ImageEditor::on_shapeToolButton_clicked()
{
    if (m_currentShapeAction)
    {
        setShapeFromAction(m_currentShapeAction);
    }
    else
    {
        m_currentMode = ImageEditorScene::Select;
    }

    m_scene->cancelCrop();
    m_scene->clearFocus();
    m_scene->clearSelection();

    if (!m_currentItemImage.isEmpty())
    {
        m_scene->setItemImage(m_currentItemImage);
    }
    if (m_scene->mode() == m_currentMode)
    {
        m_currentMode = ImageEditorScene::Select;
    }
    m_scene->setMode(m_currentMode);
}

void ImageEditor::initShapeButton()
{
    m_shapeMenu = new SimpleMenuEx(ui->shapeToolButton, this);

    m_shapeActions[0] = m_shapeMenu->addAction(QIcon(":/res/16x16_circle_outline.png"), "", this, SLOT(onSetShapeMenu()));
    m_shapeActions[0]->setToolTip(tr("Draw Outlined Circle"));
    m_shapeActions[0]->setData(ImageEditorScene::ItemEmptyEllipse);
    m_shapeActions[1] = m_shapeMenu->addAction(QIcon(":/res/16x16_circle_filled.png"), "", this, SLOT(onSetShapeMenu()));
    m_shapeActions[1]->setToolTip(tr("Draw Circle"));
    m_shapeActions[1]->setData(ImageEditorScene::ItemEllipse);
    m_shapeActions[2] = m_shapeMenu->addAction(QIcon(":/res/16x16_line.png"), "", this, SLOT(onSetShapeMenu()));
    m_shapeActions[2]->setToolTip(tr("Draw Line"));
    m_shapeActions[2]->setData(ImageEditorScene::ItemLine);
    m_shapeActions[3] = m_shapeMenu->addAction(QIcon(":/res/16x16_square_outline.png"), "", this, SLOT(onSetShapeMenu()));
    m_shapeActions[3]->setToolTip(tr("Draw Outlined Square"));
    m_shapeActions[3]->setData(ImageEditorScene::ItemEmptyRect);
    m_shapeActions[4] = m_shapeMenu->addAction(QIcon(":/res/16x16_square_filled.png"), "", this, SLOT(onSetShapeMenu()));
    m_shapeActions[4]->setToolTip(tr("Draw Square"));
    m_shapeActions[4]->setData(ImageEditorScene::ItemRect);
    m_shapeActions[5] = m_shapeMenu->addAction(QIcon(":/res/16x16_icanhas.png"), "", this, SLOT(onSetShapeMenu()));
    m_shapeActions[5]->setToolTip(tr("Draw 'I Can Has...' image"));
    m_shapeActions[5]->setData(ItemIcanHas);
    m_shapeActions[6] = m_shapeMenu->addAction(QIcon(":/res/16x16_star_outline.png"), "", this, SLOT(onSetShapeMenu()));
    m_shapeActions[6]->setToolTip(tr("Draw Outlined Star"));
    m_shapeActions[6]->setData(ImageEditorScene::ItemEmptyStar);
    m_shapeActions[7] = m_shapeMenu->addAction(QIcon(":/res/16x16_star_filled.png"), "", this, SLOT(onSetShapeMenu()));
    m_shapeActions[7]->setToolTip(tr("Draw Star"));
    m_shapeActions[7]->setData(ImageEditorScene::ItemStar);
    m_shapeActions[8] = m_shapeMenu->addAction(QIcon(":/res/16x16_mustache.png"), "", this, SLOT(onSetShapeMenu()));
    m_shapeActions[8]->setToolTip(tr("Draw 'Mustache'"));
    m_shapeActions[8]->setData(ItemMustache);

    m_currentShapeAction = m_shapeActions[0];

    m_shapeMenu->setItemSize(QSize(22,22));
    m_shapeMenu->setColumnCount(3);
}

void ImageEditor::on_saveToolButton_clicked()
{
    onUploadClicked();
}

void ImageEditor::updateActionIcon()
{
    for(int i = 0; i < 4; ++i)
    {
        if (i == m_saveMode)
        {
#ifdef Q_OS_WIN
            m_saveToActions[i]->setIcon(m_checkedIcon);
#else
            m_saveToActions[i]->setCheckable(true);
            m_saveToActions[i]->setChecked(true);
#endif
        }
        else
        {
#ifdef Q_OS_WIN
            m_saveToActions[i]->setIcon(QIcon());
#else
            m_saveToActions[i]->setCheckable(true);
            m_saveToActions[i]->setChecked(false);
#endif
        }
    }
}


void ImageEditor::initSaveMenu()
{
    m_saveMenu = new SimpleMenu(ui->saveToolButton, this);

    m_saveToActions[0] = m_saveMenu->addAction(QIcon(), tr("Save to Box"), this, SLOT(onSaveToBox()));
    m_saveToActions[0]->setToolTip(ui->saveToolButton->toolTip());
    m_saveToActions[1] = m_saveMenu->addAction(QIcon(), tr("Save to desktop"), this, SLOT(onSaveToDesktop()));
    m_saveToActions[1]->setToolTip(tr("Save image to desktop"));
    m_saveToActions[2] = m_saveMenu->addAction(QIcon(), tr("Select folder to save to"), this, SLOT(onSaveToDirectory()));
    m_saveToActions[2]->setToolTip(tr("Save image to selected folder"));
    m_saveToActions[3] = m_saveMenu->addAction(QIcon(), tr("Copy to clipboard"), this, SLOT(onSaveToClipboard()));
    m_saveToActions[3]->setToolTip(tr("Copy to clipboard"));

    m_checkedIcon.addPixmap(QPixmap(":/res/checkmark.png"));
    updateActionIcon();

    m_style = new QPlastiqueStyle();
    if(m_style)
    {
        m_saveMenu->setStyle(m_style);
    }

    QPalette palette = m_saveMenu->palette();
    palette.setColor(QPalette::Window, Qt::black);
    palette.setColor(QPalette::WindowText, Qt::white);
    m_saveMenu->setPalette(palette);
    QFont font  = ui->saveToolButton->font();
    font.setBold(false);
    m_saveMenu->setFont(font);   
}

void ImageEditor::on_imageNameLineEdit_editingFinished()
{
    QFileInfo info(m_imageName);

    m_imageName = info.absoluteDir().path() + "/" + ui->imageNameLineEdit->text() + "." + info.suffix();
}

QPixmap ImageEditor::createHighlightCursor()
{
    QPixmap pixmap(27,27);

    pixmap.fill(Qt::transparent);

    QPainter p(&pixmap);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(QColor(115,115,115,255), 2));
    QColor color(m_scene->fgColor());
    color.setAlpha(150);
    p.setBrush(color);
    p.drawEllipse(QRectF(1,1,25,25));
    p.end();

    return pixmap;
}


void ImageEditor::on_colorAction_triggered()
{
    on_colorToolButton_clicked();
}

extern const QString fullAppName();

void ImageEditor::closeEvent(QCloseEvent* event)
{
    if (!m_scene->backgroundPixmap().isNull() && m_resultPixmap.isNull())
    {
        if (isMinimized())
        {
            showNormal();
        }

        QString message = tr("The image that you editing not saved. Force close?");

        QMessageBox messageBox;

        messageBox.setWindowTitle(fullAppName());
        messageBox.setText(tr("About to Close"));
        messageBox.setInformativeText(message);

        messageBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel | QMessageBox::Abort );

        messageBox.setDefaultButton(QMessageBox::Cancel);

        messageBox.setIconPixmap(QIcon(":/res/icon.ico").pixmap(64));
        messageBox.setButtonText( QMessageBox::Ok, tr("Continue editing")); // 1
        messageBox.setButtonText( QMessageBox::Abort, tr("Force Close")); // 2
        messageBox.setButtonText( QMessageBox::Cancel, tr("Save and Close")); // 3

        switch (messageBox.exec())
        {
        case QMessageBox::Cancel:            
            onUploadClicked();
            m_scene->setPixmap(QPixmap());
            event->accept();
            break;
        case QMessageBox::Abort:
            m_scene->setPixmap(QPixmap());
            event->accept();
            break;
        case QMessageBox::Ok:
            raise();
            activateWindow();
            event->ignore();
            break;
        }
    }
}


void ImageEditor::on_actionShapeButton_triggered()
{
    on_shapeToolButton_clicked();
}

void ImageEditor::on_actionSave_triggered()
{
    onUploadClicked();
}
