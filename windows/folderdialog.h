/* folderdialog.h
 * Author: Evgeniy Sergeev, <evgeniy.sereev@gmail.com>
 */

#ifndef FOLDERDIALOG_H
#define FOLDERDIALOG_H

#include <QDialog>
#include <QTreeWidgetItem>
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

    QString selectedPath() const;
    void updateFoldersList(const QString folderId = "0");

private slots:
    void onAccept();
    void onReject();
    void on_pushButton_clicked();

    void treeFolder(QString id, QString parentId, QString name, QString link, QVariant tag);
    void treeFile(QString id, QString parentId, QString name, QString link, QVariant tag);
    void treeFinished(QVariant tag);
    void onFolderCreated(const QString folderName, const QString folderId);

    void on_foldersTreeWidget_itemExpanded(QTreeWidgetItem *item);

private:
    void removeMarkedItems();
    void putItemsToMarkedList(QTreeWidgetItem* item);

private:
    Ui::FolderDialog *ui;
    BxNet* m_bxnet;
    QString m_selectedPath;

    QMap<QString, QTreeWidgetItem* >  m_folders;
    QMap<QString, QTreeWidgetItem*>   m_markedItems;
};

#endif // FOLDERDIALOG_H
