#include "folderdialog.h"
#include "ui_folderdialog.h"
#include <QStringList>
#include <QInputDialog>

FolderDialog::FolderDialog(BxNet* bxnet, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FolderDialog),
    m_bxnet(bxnet)
{
    ui->setupUi(this);

    connect(m_bxnet, SIGNAL(folderCreated(QString, QString)), this, SLOT(folderCreated(QString, QString)));
    connect(m_bxnet, SIGNAL(createFolderFinished()), this, SLOT(createFolderCreated()));
    connect(m_bxnet, SIGNAL(treeFolder(QString,QString,QString,QString,QVariant)), this, SLOT(treeFolder(QString,QString,QString,QString,QVariant)));
    connect(m_bxnet, SIGNAL(treeFile(QString,QString,QString,QString,QVariant)), this, SLOT(treeFile(QString,QString,QString,QString,QVariant)));
    connect(m_bxnet, SIGNAL(treeFinished(QVariant)), this, SLOT(treeFinished(QVariant)));

    m_folders.clear();
    QTreeWidgetItem* item = new QTreeWidgetItem(QStringList() << "" << "0" << "0");
    Q_ASSERT(item);
    if (item)
    {
        item->setIcon(0, QIcon(":/res/32x32_box-logo.png"));
        ui->foldersTreeWidget->addTopLevelItem(item);
    }
    m_folders["0"] = item; // id always "0" here

    updateFoldersList();
}

FolderDialog::~FolderDialog()
{
    delete ui;
}

void FolderDialog::on_pushButton_clicked()
{
    updateFoldersList();
}

void FolderDialog::removeMarkedItems()
{
    foreach(const QString id, m_markedItems.keys())
    {
        QTreeWidgetItem* item = m_markedItems[id];
        deleteItem(id);
    }

    m_markedItems.clear();
}

void FolderDialog::putItemsToMarkedList(QTreeWidgetItem* item)
{
    if (item == NULL)
    {
        return;
    }
    for(int i = 0; i < item->childCount(); ++i)
    {
        QTreeWidgetItem* child = item->child(i);
        m_markedItems[child->text(1)] = child;
    }
}

void FolderDialog::updateFoldersList(const QString folderId)
{
    ui->foldersTreeWidget->setCursor(Qt::WaitCursor);

    // prepeare ids for removing.
    m_markedItems.clear();
    putItemsToMarkedList(m_folders[folderId]);
    m_bxnet->readTree(folderId, true, false, QVariant(0));
}

void FolderDialog::treeFile(QString id, QString parentId, QString name, QString link, QVariant tag)
{
    Q_UNUSED(id);
    Q_UNUSED(parentId);
    Q_UNUSED(name);
    Q_UNUSED(link);
    Q_UNUSED(tag);
}

void FolderDialog::deleteItem(const QString& id)
{
    if (id.isEmpty() || !m_folders.contains(id))
    {
        return;
    }
    foreach(QTreeWidgetItem* item, m_folders.values())
    {
        if (item && item->text(2) == id) // parent...
        {
            deleteItem(item->text(1)); // id...
        }
    }

    qDebug() << "removed folder with id = " << id << m_folders[id];
    delete m_folders[id];
    m_folders.remove(id);
}

void FolderDialog::treeFolder(QString id, QString parentId, QString name, QString link, QVariant tag)
{
    if (!id.isEmpty())
    {
        m_markedItems.remove(id);
    }
    if (m_folders.contains(id))
    {
        QTreeWidgetItem* existsItem = m_folders[id];
        if (existsItem)
        {
            const QString xName = existsItem->text(0);
            const QString xId   = existsItem->text(1);
            const QString xParentId = existsItem->text(2);

            if ((xName != name) || (xId != id) || (xParentId != parentId))
            {
                deleteItem(id);
            }
        }
    }
    if (!m_folders.contains(id))
    {
        QTreeWidgetItem* parent = NULL;
        if (m_folders.contains(parentId))
        {
            parent = m_folders[parentId];
        }
        QTreeWidgetItem* item = new QTreeWidgetItem(parent,
                                                    QStringList() << name << id << parentId);

        Q_ASSERT(item);
        if (item)
        {
            item->setIcon(0, QIcon(":/res/icon_folder.PNG"));
            ui->foldersTreeWidget->addTopLevelItem(item);
        }

        m_folders[id] = item;
    }

    //if (!id.isEmpty() && id != "0")
    //{
    //    // second level
    //    int level = tag.toInt(); // download only two levels of tree
    //    if (level > 0)
    //    {
    //        m_bxnet->readTree(id, true, true, QVariant(level - 1));
    //    }
    //}
}

void FolderDialog::treeFinished(QVariant tag)
{
    removeMarkedItems();
 //   ui->foldersTreeWidget->sortItems(0, Qt::AscendingOrder);
    //ui->foldersTreeWidget->expandAll();

    // set selection on current folder id

    Q_ASSERT(m_bxnet);
    if (m_bxnet != NULL)
    {
        const QString id = m_bxnet->uploadFolderId();
        if (m_folders.contains(id))
        {
            QTreeWidgetItem* item = m_folders[id];
            if (item != NULL)
            {
                ui->foldersTreeWidget->expandItem(item);
                ui->foldersTreeWidget->setCurrentItem(item, 0, QItemSelectionModel::ClearAndSelect);
            }
        }
    }

    ui->foldersTreeWidget->setCursor(Qt::ArrowCursor);
}

void FolderDialog::on_foldersTreeWidget_itemExpanded(QTreeWidgetItem *item)
{
    //QString folderId = item->text(1);
//
    //if (!folderId.isEmpty())
    //{
    //    updateFoldersList(folderId);
    //}
}

void FolderDialog::on_foldersTreeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    accept();
}

void FolderDialog::on_buttonBox_accepted()
{
    qDebug() << "1";
}

QString FolderDialog::getSelectedPath() const
{
    if (ui->foldersTreeWidget->selectedItems().count() > 0)
    {
        return getPathForItem(ui->foldersTreeWidget->selectedItems()[0]);
    }

    return "";
}

QString FolderDialog::getPathForItem(QTreeWidgetItem* item) const
{
    QString result = "";
    QTreeWidgetItem* rootItem = m_folders["0"];
    while (item)
    {
        if (result.isEmpty())
        {
            if (item == rootItem)
            {
                result = "/";
            }
            else
            {
                result = item->text(0);
            }
        }
        else
        {
            if (item != rootItem)
            {
                result = item->text(0) + "/" + result;
            }
        }
        item = item->parent();
    }

    return result;
}


void FolderDialog::on_pushButton_2_clicked()
{
    bool ok = false;
    QString newFolderName = QInputDialog::getText(this, tr("Enter New Folder Name"), "", QLineEdit::Normal, QString::null, &ok);
    if (ok && !newFolderName.isEmpty())
    {
        QTreeWidgetItem* item = ui->foldersTreeWidget->currentItem();
        QString currentPath = getPathForItem(item);

        if (currentPath == "/")
        {
            currentPath = newFolderName;
        }
        else
        {
            currentPath = currentPath + "/" + newFolderName;
        }

        ui->foldersTreeWidget->setCursor(Qt::WaitCursor);
        m_bxnet->createFolder(currentPath);
    }
}

void FolderDialog::folderCreated(const QString folderName, const QString folderId)
{
    updateFoldersList();
}

void FolderDialog::createFolderFinished()
{
    ui->foldersTreeWidget->setCursor(Qt::ArrowCursor);
}
