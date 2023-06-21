#ifndef TREE_H
#define TREE_H

#include <QWidget>
#include <QTreeWidget>

namespace Ui {
class Tree;
}

class Tree : public QWidget
{
    Q_OBJECT

public:
    explicit Tree(QWidget *parent = nullptr);
    Tree(Tree const &tree);
    ~Tree();
    QTreeWidget* getTreeWidget();


signals:
    void NodeChanged(QString Key, QVariant newValue);
    void FileSelectedToView(QString path);
    void ItemSelectionChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

public slots:
    void on_NodeChange(QString Key, QVariant newValue);


private slots:
    void SelectDirCallBack();
    void removeItem();
    void on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);

private:
    Ui::Tree *ui;
};

#endif // TREE_H
