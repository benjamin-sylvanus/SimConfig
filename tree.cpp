#include "tree.h"
#include "node.h"
#include "configuration.h"
#include "ui_tree.h"
#include "util.h"
#include <QLayoutItem>
#include <QFormLayout>


Tree::Tree(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Tree)
{
    ui->setupUi(this);
    connect(this,SIGNAL(NodeChanged(QString,QVariant)),this,SLOT(on_NodeChange(QString,QVariant)));
}

Tree::Tree(const Tree &tree)
{
    this->ui  = tree.ui;
}




QTreeWidget * Tree::getTreeWidget()
{
    return ui->treeWidget;
}

Tree::~Tree()
{
    delete ui;
}

void Tree::SelectDirCallBack()
{
    QString dirPath = QFileDialog::getExistingDirectory(ui->treeWidget, "Select Directory");

    if (!dirPath.isEmpty()) {
        Configuration rootConfig("in", "out", 200, 200, 200, 100, 1000, 1.0, 0.1,
                                 1.0, 0.5, 0.5, 0.1, 1.0, 0, true);
        QFileInfo info = QFileInfo(dirPath);

        rootConfig.in = info.path();

        Node *newDirNode =
            new Node(ui->treeWidget, QFileInfo(dirPath).fileName(), rootConfig);
        addDirectory(QFileInfo(dirPath).filePath(), newDirNode, rootConfig);
        ui->treeWidget->expandItem(newDirNode);
    }
}

void Tree::removeItem()
{
    QTreeWidgetItem *currentItem = ui->treeWidget->currentItem();
    if (currentItem) {
        delete currentItem;
    }
}

void Tree::on_NodeChange(QString Key,QVariant newValue)
{
    // Current Selection
    QTreeWidgetItem * item = ui->treeWidget->currentItem();
    Node *node = dynamic_cast<Node *>(item);
    QVariantMap map;
    map[Key] = newValue;
    propagateConfigChange(node,map);
}


void Tree::on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    emit ItemSelectionChanged(current,previous);
}


void Tree::on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    Node *node = dynamic_cast<Node *>(item);

    qDebug() << node->text(0);
    QString path = node->config().in;
    path.append("/"+node->text(0));
    qDebug() << path;
    QFileInfo info(path);
    qDebug() << info;
    if (info.isFile())
    {
        qInfo() << "Emitting Signal";
        emit FileSelectedToView(path);
    }

}

