//
// Created by Benjamin Sylvanus on 5/8/23.
//

#ifndef GPTQT_UTIL_H
#define GPTQT_UTIL_H
#include "node.h"
#include <QApplication>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QMessageBox>
#include <QPushButton>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QProcess>
#include <QTextBrowser>

//DataNode* parseNode(const QJsonObject &nodeObj);

//void onSelectJson();

QJsonObject nodeToJson(Node *node);

void addDirectory(const QString &path, Node *parentNode, Configuration config);

void updateMap(QVariantMap &old, QVariantMap &changes);

void propagateConfigChange(Node *node, QVariantMap &changedParameters);

void runMatlabFunction(QPushButton *button,QTextBrowser *browser);

void RunGLWIDGET(QTextBrowser *browser,QString path);

int s2i(std::vector<int> v, std::vector<int> dims);

#endif //GPTQT_UTIL_H
