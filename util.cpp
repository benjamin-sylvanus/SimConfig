//
// Created by Benjamin Sylvanus on 5/8/23.
//

#include "QtCore/qprocess.h"
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
#include "util.h"

//DataNode* parseNode(const QJsonObject &nodeObj) {

//    QString nodeName = nodeObj["name"].toString();
//    qInfo() << "Node Name: " <<  nodeName;

//    QJsonObject configObj = nodeObj["config"].toObject();
//    QVariantMap configMap = configObj.toVariantMap();

//    Configuration nodeConfig(configMap);
//    DataNode * dnode = new DataNode(nodeName, nodeConfig);

//    QJsonArray children = nodeObj["children"].toArray();
//    for (const QJsonValue &childValue : children) {
//        QJsonObject childObj = childValue.toObject();
//        DataNode *childNode = parseNode(childObj);
//        dnode->addChild(*childNode);
//    }

//    return dnode;
//}

//void onSelectJson() {
//    QString filename = QFileDialog::getOpenFileName(nullptr, "Open JSON File", "", "JSON Files (*.json)");

//    QFile file(filename);
//    if (!file.open(QIODevice::ReadOnly)) {
//        QMessageBox::information(nullptr, "Unable to open file",
//                                 file.errorString());
//        return;
//    }

//    QTextStream in(&file);
//    QString jsonStr = in.readAll();
//    file.close();

//    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonStr.toUtf8());

//    // contains  [OutRoot, Configuration Structure]
//    QJsonObject rootNodeObj = jsonDoc.object();

//    // Extract the OutRoot
//    QJsonValue outdir = rootNodeObj.value("OutRoot");
//    qInfo() << outdir.toString();

//    // Extract the Configurations Top Level Structure
//    QJsonValue top = rootNodeObj.value("topLevelNodes");
//    // Convert to an Object
//    QJsonObject topObj = top.toObject();

//    // Parse the Object and Convert to a node.
//    DataNode *rootNode = parseNode(topObj);
//    rootNode->printNode();
//}



QJsonObject nodeToJson(Node *node) {
    QJsonObject obj;
    obj["name"] = node->text(0);

    Configuration nodeConfig = node->config();
    Node *parentNode = static_cast<Node *>(node->parent());
    QVariantMap nodeConfigMap = nodeConfig.toVariantMap();

    if (parentNode != nullptr) {
        Configuration parentConfig = parentNode->config();
        QVariantMap parentConfigMap = parentConfig.toVariantMap();

        // Filter out redundant parameters
        for (auto it = nodeConfigMap.begin(); it != nodeConfigMap.end();) {

            if (parentConfigMap.contains(it.key()) &&
                parentConfigMap[it.key()] == it.value()) {
                it = nodeConfigMap.erase(it);
            } else {
                ++it;
            }
        }
    }

    obj["config"] = QJsonObject::fromVariantMap(nodeConfigMap);

    QJsonArray children;
    for (int i = 0; i < node->childCount(); i++) {
        children.append(nodeToJson(static_cast<Node *>(node->child(i))));
    }

    obj["children"] = children;

    return obj;
}

void addDirectory(const QString &path, Node *parentNode, Configuration config) {
    QDir dir(path);
    QFileInfoList entries =
        dir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries );

    // copy the configuration passed.
    Configuration parentConfig = config;
    for (const QFileInfo &entry : entries) {

        if (entry.isDir()) {
            config = parentConfig;
            config.in = entry.path();
            Node *newNode = new Node(parentNode, entry.fileName(), config); // does this do anything if its not a directory?
            addDirectory(entry.absoluteFilePath(), newNode, config);
        }
        if (entry.isFile() && entry.suffix() == "swc")
        {
            config = parentConfig;
            config.in = entry.path();
            Node *newNode = new Node(parentNode, entry.fileName(), config); // does this do anything if its not a directory?
        }
    }
}

void updateMap(QVariantMap &old, QVariantMap &changes)
{

    // loop over configuration and new configuration;
    for (auto it = changes.begin(); it != changes.end(); it++ ) {
//        qInfo() <<  "Key:" << it.key() << "Value:"<<it.value();

        old[it.key()] = it.value();
    }
}

void propagateConfigChange(Node *node, QVariantMap &changedParameters)
{
    // extract configuration
    Configuration cfg = node->config();

    // convert to map
    QVariantMap NewCfgMap = cfg.toVariantMap();

    // update values
    updateMap(NewCfgMap,changedParameters);

    // convert back to config
    cfg.fromVariantMap(NewCfgMap);
    node->setConfig(cfg);
    for (int i = 0; i < node->childCount(); ++i) {
        if (Node *childNode = dynamic_cast<Node *>(node->child(i))) {
            propagateConfigChange(childNode, changedParameters);
        }
    }
}

void runMatlabFunction(QPushButton *button,QTextBrowser *browser) {
    QProcess* process = new QProcess();

    QObject::connect(process, &QProcess::readyReadStandardOutput, [process,browser] () {
        QString output = process->readAllStandardOutput();
        // Now you can do something with the output...
        browser->append(output);
    });

    QObject::connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                     [=](int exitCode, QProcess::ExitStatus exitStatus){
                         // handle process finish here, for example:
                         browser->append("Process finished");
                         button->setEnabled(true);
                         process->deleteLater();

                     });
    QString program = "/Applications/MATLAB_R2022b.app/Contents/MacOS/MATLAB";

    QString path = "/Users/benjaminsylvanus/Desktop/out/text.json";
    QString filename =
        QFileDialog::getOpenFileName(nullptr,"Select Directory");

    if (!filename.isEmpty()) {
        path=filename;
    }

    QStringList arguments;
    arguments << "-nodisplay" << "-nosplash" << "-nodesktop" << "-r"
              << QString("addpath(genpath('/Users/benjaminsylvanus/Documents/work/server_code/combined/trees-prep/')); "
                         "preprocess('%1'); exit;").arg(path);

    process->start(program, arguments);

    if (!process->waitForStarted()) {
        // Handle error here
    }

    else
    {
        button->setEnabled(false);
    }
}

void RunGLWIDGET(QTextBrowser *browser,QString path)
{
    QProcess* process = new QProcess();

    QObject::connect(process, &QProcess::readyReadStandardOutput, [browser,process] () {
        QString output = process->readAllStandardOutput();
        // Now you can do something with the output...
        browser->append(output);
    });

    QObject::connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                     [=](int exitCode, QProcess::ExitStatus exitStatus){
                         // handle process finish here, for example:
                         browser->append("Process finished");
                         process->deleteLater();
                     });
    QString program = "/Users/benjaminsylvanus/Documents/work/OpenGLSphereCylinderPipe/combined/bin/sphere";
    QStringList arguments;
    arguments << path;
    qDebug() << program;
    qDebug() << arguments;
    process->start(program, arguments);
    if (!process->waitForStarted()) {
        // Handle error here
    }

    else
    {

    }
}

int s2i(std::vector<int> v, std::vector<int> d) {
    int bx = d[0];
    int by = d[1];
    int bz = d[2];
    int ix = v[0];
    int iy = v[1];
    int iz = v[2];

    return 0 + bx * (by * iz + iy) + ix;
}


