#ifndef FOLDERDIALOG_H
#define FOLDERDIALOG_H

#include <QDialog>
#include <QTreeWidgetItem>
#include <QPointer>
#include "bxnet.h"

namespace Ui {
class FolderDialog;
}

class FolderDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit FolderDialog(BxNet* bxnet, QWidget *parent = 0);
    ~FolderDialog();

    QString getSelectedPath() const;
    QString getPathForItem(QTreeWidgetItem* item) const;

private slots:
    void createFolderFinished();
    void folderCreated(const QString folderName, const QString folderId);
    void treeFolder(QString id, QString parentId, QString name, QString link, QVariant tag);
    void treeFile(QString id, QString parentId, QString name, QString link, QVariant tag);
    void treeFinished(QVariant tag);

    void on_pushButton_clicked();
    void on_foldersTreeWidget_itemExpanded(QTreeWidgetItem *item);
    void on_foldersTreeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void on_buttonBox_accepted();
    void on_pushButton_2_clicked();

private:
    void updateFoldersList(const QString folderId = "0");
    void deleteItem(const QString& id);
    void removeMarkedItems();
    void putItemsToMarkedList(QTreeWidgetItem* item);

private:
    Ui::FolderDialog *ui;
    BxNet* m_bxnet;

    QMap<QString, QTreeWidgetItem* >  m_folders;
    QMap<QString, QTreeWidgetItem* >  m_markedItems;
};

#endif // FOLDERDIALOG_H
