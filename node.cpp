#include "node.h"

Node::Node(QTreeWidget* parent, const QString& name, const Configuration& config)
    : QTreeWidgetItem(parent, QStringList(name)), m_config(config) {
    setText(0,name);
}

Node::Node(QTreeWidgetItem* parent, const QString& name, const Configuration& config)
    : QTreeWidgetItem(parent, QStringList(name)), m_config(config) {
    setText(0,name);
}

QString Node::text(int column) const {
    return QTreeWidgetItem::text(column);
}

void Node::inheritConfig() {
    if (Node* parentNode = dynamic_cast<Node*>(parent())) {
        m_config = parentNode->config();
    }
}

Configuration Node::config() const {
    return m_config;
}

void Node::setConfig(const Configuration& config) {
    m_config = config;
    emitDataChanged();
}
