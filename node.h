#ifndef NODE_H
#define NODE_H

#include <QTreeWidgetItem>
#include <QVariantMap>
#include "configuration.h"

class Node : public QTreeWidgetItem {
public:
    //    Node(QTreeWidgetItem* parent, const QString& name, const Configuration& config);
    Node(QTreeWidget* parent, const QString& name, const Configuration& config);
    Node(QTreeWidgetItem* parent, const QString& name, const Configuration& config);
    Configuration config() const;
    QString text(int column) const;
    void inheritConfig();
    void setConfig(const Configuration& config);

private:
    Configuration m_config;
};

#endif // NODE_H
