/* imageeditor.h
 * Author: Evgeniy Sergeev, <evgeniy.sereev@gmail.com>
 */

#ifndef IMAGEEDITOR_H
#define IMAGEEDITOR_H

#include <QWidget>
#include <QButtonGroup>
#include "imageeditorscene.h"
#include "simplemenu.h"

namespace Ui {
    class ImageEditor;
}

const int ItemIcanHas   = ImageEditorScene::ItemUser + 1;
const int ItemMustache  = ImageEditorScene::ItemUser + 2;

class ImageEditor : public QWidget
{
    Q_OBJECT

public:
    explicit ImageEditor(QWidget *parent = 0);
    virtual ~ImageEditor();

    void clear();
    void setPixmap(const QPixmap& pixmap, const QString& imageName);
    ImageEditorScene* scene() const;

    QString imageName() const;
    void setImageName(const QString& name);

protected:
    virtual void closeEvent(QCloseEvent* event);

signals:
    void saveToFile(QPixmap& pixmap, const QString& fileName);
    void copyToClipboard(QPixmap& pixmap);
    void uploadToNet(QPixmap& pixmap, bool facebook, bool twitter);

private:
    QPixmap& resultPixmap();
    QIcon createColorIcon(QColor color, bool border = true);
    QPixmap createHighlightCursor();
    void updateColorButton();
    void initPalette();
    void initShapeButton();
    void initSaveMenu();
    void updateActionIcon();
    void saveSettings();
    void loadSettings();
    void setShapeFromAction(QAction* action);

public slots:
    void onModeChanged(int mode);
    void onNewItemCreated(QGraphicsItem* item);
    void onSelectionChanged();
    void onSetColor();
    void onSetShape();
    void onSetShapeMenu();

    void onUploadClicked();
    void onSaveToBox();
    void onSaveToDesktop();
    void onSaveToDirectory();
    void onSaveToClipboard();
    void onSaveToFileClicked();
    void onCopyToClipboardClicked();
    void onTwitterClicked();
    void onFacebookClicked();

private slots:
    void on_arrowToolButton_clicked();
    void on_highlightToolButton_clicked();
    void on_textToolButton_clicked();
    void on_blurToolButton_clicked();
    void on_cropToolButton_clicked();
    void on_colorToolButton_clicked();
    void on_shapeToolButton_clicked();
    void on_saveToolButton_clicked();
    void on_imageNameLineEdit_editingFinished();
    void on_colorAction_triggered();

    void on_actionShapeButton_triggered();

    void on_actionSave_triggered();

private:
    Ui::ImageEditor*    ui;

    SimpleMenuEx*       m_paletteMenu;
    SimpleMenuEx*       m_shapeMenu;
    SimpleMenu*         m_saveMenu;
    QStyle*             m_style;
    int                 m_saveMode;
    QIcon               m_checkedIcon;
    QCursor*            m_defaultCursor;

    ImageEditorScene*   m_scene;

    QString             m_imageName;
    QString             m_dirName;

    QPixmap             m_resultPixmap; // result stored here

    QAction*            m_saveToActions[4];
    QAction*            m_shapeActions[9];
    QAction*            m_currentShapeAction;

    ImageEditorScene::Mode  m_currentMode;
    QString                 m_currentItemImage;    
};

#endif // IMAGEEDITOR_H
