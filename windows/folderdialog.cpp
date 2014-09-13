#include "folderdialog.h"
#include "ui_folderdialog.h"
#include <QStringList>

FolderDialog::FolderDialog(BxNet* bxnet, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FolderDialog),
    m_bxnet(bxnet)
{
    ui->setupUi(this);

    connect(this, SIGNAL(accepted()), this, SLOT(onAccept()));
    connect(this, SIGNAL(rejected()), this, SLOT(onReject()));

    connect(m_bxnet, SIGNAL(folderCreated(QString, QString)), this, SLOT(onFolderCreated(QString,QString)));
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

void FolderDialog::onFolderCreated(const QString folderName, const QString folderId)
{
    updateFoldersList();
}

void FolderDialog::onAccept()
{
    m_selectedPath = "";

    if (ui->foldersTreeWidget->selectedItems().count() > 0)
    {
        QTreeWidgetItem* selectedItem = ui->foldersTreeWidget->selectedItems().at(0);
        while (selectedItem)
        {
            if (m_selectedPath.isEmpty())
            {
                m_selectedPath = selectedItem->text(0);
            }
            else
            {
                m_selectedPath = selectedItem->text(0) + "/" + m_selectedPath;
            }
            selectedItem = selectedItem->parent();
        }

        if (m_selectedPath.length() > 0 && m_selectedPath.at(0) == '/')
        {
            m_selectedPath = m_selectedPath.right(m_selectedPath.length() - 1);
        }
    }
}

void FolderDialog::onReject()
{
    m_selectedPath.clear();
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
        delete item;
        m_folders.remove(id);
        qDebug() << "removed folder with id = " << id;
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

QString FolderDialog::selectedPath() const
{
    return m_selectedPath;
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
                delete existsItem;
                m_folders.remove(id);
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
    //ui->foldersTreeWidget->sortItems(0, Qt::AscendingOrder);
    ui->foldersTreeWidget->expandToDepth(1);
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
