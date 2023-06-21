#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "QtCore/qobjectdefs.h"
#include "QtWidgets/qcheckbox.h"
#include "QtWidgets/qlineedit.h"
#include "QtWidgets/qspinbox.h"
#include "util.h"
#include <QDialog>
#include <QFileInfo>
#include <QFileDialog>
#include <QLabel>
#include "tree.h"

// TODO Fix the output directory.
// This used to be a button. Never implemented it in this version.
MainWindow::MainWindow(QWidget *parent, QString out)
    : QMainWindow(parent), out(out)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->tree = new Tree();

    ui->TreeContainHLayout->insertWidget(0,tree);
    ui->TreeContainHLayout->setStretch(0,2);
    ui->TreeContainHLayout->setStretch(1,1);

    // Run Buttons
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(RunMatlab()));

    // Tree Buttons
    connect(ui->addItemButton, SIGNAL(clicked()), tree, SLOT(SelectDirCallBack()));
    connect(ui->removeItemButton, SIGNAL(clicked()),tree, SLOT(removeItem()));

    // Json Buttons
    connect(ui->saveJsonButton, SIGNAL(clicked()),this, SLOT(on_saveJson()));
    connect(ui->openJsonButton, SIGNAL(clicked()),this, SLOT(on_openJson()));

    // Tree -> Main Window --------- SIGNAL -> SLOT
    connect(tree, SIGNAL(FileSelectedToView(QString)), this, SLOT(on_FileSelectedToView(QString)));
    connect(tree, SIGNAL(ItemSelectionChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this,
            SLOT(on_TreeWidgetSelectionChange(QTreeWidgetItem*,QTreeWidgetItem*)));

    // MainWindow -> Tree ------------------ SIGNAL -> SLOT
    connect(this,SIGNAL(NodeChanged(QString,QVariant)), tree, SLOT(on_NodeChange(QString,QVariant)));

    // MainWindow -> Viewer ---------------- SIGNAL -> SLOT
    connect(this,SIGNAL(ViewerCheckBoxChanged(int)), ui->openGLWidget, SLOT(slot1(int)));
    connect(ui->openGLWidget,SIGNAL(viewerActive()),this, SLOT(viewerActivated()));
    connect(ui->openGLWidget,SIGNAL(viewerInactive()),this, SLOT(viewerDeactivated()));
    initFormData();
}

void MainWindow::RunMatlab()
{
    runMatlabFunction(ui->pushButton,ui->textBrowser);
}

void MainWindow::on_openJson()
{

}

void MainWindow::on_saveJson()
{
    QString fileName = QFileDialog::getSaveFileName(
        this->window(), "Save Tree to JSON", "", "JSON files (*.json)");
    if (!fileName.isEmpty()) {

        QTreeWidget *t = tree->getTreeWidget();
        qInfo() << t->topLevelItemCount();
        QJsonArray topLevelNodes;

        for (int i = 0; i < t->topLevelItemCount(); ++i) {

            topLevelNodes.append(
                nodeToJson(static_cast<Node *>(t->topLevelItem(i))));
            qInfo() << topLevelNodes[i];
        }
        QJsonObject treeJson;
        treeJson["topLevelNodes"] = topLevelNodes;

        // ADD THE OUT PARAMETER;
        treeJson["Out Root"] = out;

        QJsonDocument doc(treeJson);
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(doc.toJson());
            file.close();
        } else {
            QMessageBox::critical(this->window(), "Error",
                                  "Unable to save the JSON file.");
        }
    }
}


MainWindow::~MainWindow()
{
    delete ui;
}




void MainWindow::on_viewButton()
{
    qDebug() << "Viewer Selected";

    QString fileName = QFileDialog::getOpenFileName(
        this->window(), "Select an SWC", "", "SWC Files (*.swc)");

    if (!fileName.isEmpty())
    {
        RunGLWIDGET(ui->textBrowser,fileName);
    }
}

void MainWindow::on_FileSelectedToView(QString path)
{
    qDebug() << "Signal Recieved";
    RunGLWIDGET(ui->textBrowser,path);
}

void MainWindow::on_TreeWidgetSelectionChange(QTreeWidgetItem * current, QTreeWidgetItem * previous)
{
    qInfo() << "Signal Recieved";

    Node *node = dynamic_cast<Node *>(current);
    if (node) {
        // probably a better way to do this.
        // need to replace each row but for now we delete each row then add them back
        QFormLayout * layout = ui->formLayout;
        int rowCount = layout->rowCount();
        for (int i = 0; i<rowCount; i++)
        {
            layout->removeRow(0);
        }

        // Get the Nodes Configuration
        Configuration cfg = node->config();
        QVariantMap map = cfg.toVariantMap();

        for (auto it = map.begin(); it != map.end(); ++it) {
            QLabel* label = new QLabel(it.key());
            if (it.value().typeId() == QMetaType::Int) {
                QSpinBox* spinBox = new QSpinBox();
                spinBox->setRange(0, 10000);
                spinBox->setValue(it.value().toInt());
                layout->addRow(label,spinBox);

                connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged), [this,node, key = it.key()](int newValue) {
                    emit NodeChanged(key,QVariant(newValue));
                });
            }

            else if (it.value().typeId() == QMetaType::QString) {
                QLineEdit* edit = new QLineEdit(it.value().toString());
                layout->addRow(label,edit);
                connect(edit, &QLineEdit::textChanged, [this, key = it.key()](const QString& newValue) {
                    QVariant n_value = QVariant(newValue);
                    emit NodeChanged(key,n_value);
                });
            }

            else if (it.value().typeId() == QMetaType::Double)
            {
                QDoubleSpinBox* spinBox = new QDoubleSpinBox();
                spinBox->setRange(0, 10000);
                spinBox->setValue(it.value().toDouble());
                layout->addRow(label,spinBox);
                connect(spinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this, node, key = it.key()](double newValue) {
                    emit NodeChanged(key,QVariant(newValue));
                });

            }

            else if (it.value().typeId() == QMetaType::Bool)
            {
                QCheckBox* checkBox = new QCheckBox();
                checkBox->setCheckState((it.value().toBool()) ? Qt::Checked : Qt::Unchecked);
                layout->addRow(label,checkBox);

                connect(checkBox, &QCheckBox::clicked, [node, key = it.key(), this] (bool newValue) {
                    emit NodeChanged(key,QVariant(newValue));
                });
            }
        }
    }
}

void MainWindow::initFormData()
{
    Configuration defaultConfig("in", "out", 200, 200, 200, 100, 1000, 1.0, 0.1,
                             1.0, 0.5, 0.5, 0.1, 1.0, 0, true);
    QFormLayout * layout = ui->formLayout;
    QVariantMap map = defaultConfig.toVariantMap();

    for (auto it = map.begin(); it != map.end(); ++it) {
        QLabel* label = new QLabel(it.key());
        if (it.value().typeId() == QMetaType::Int) {
            QSpinBox* spinBox = new QSpinBox();
            spinBox->setRange(0, 10000);
            spinBox->setValue(it.value().toInt());
            layout->addRow(label,spinBox);
        }

        else if (it.value().typeId() == QMetaType::QString) {
            QLineEdit* edit = new QLineEdit(it.value().toString());
            layout->addRow(label,edit);
        }

        else if (it.value().typeId() == QMetaType::Double)
        {
            QDoubleSpinBox* spinBox = new QDoubleSpinBox();
            spinBox->setRange(0, 10000);
            spinBox->setValue(it.value().toInt());
            layout->addRow(label,spinBox);

        }

        else if (it.value().typeId() == QMetaType::Bool)
        {
            QCheckBox* checkBox = new QCheckBox();
            checkBox->setCheckState((it.value().toBool()) ? Qt::Checked : Qt::Unchecked);
            layout->addRow(label,checkBox);
        }
    }
}

void MainWindow::on_pushButton_4_clicked()
{
    ui->openGLWidget->load();
}


void MainWindow::on_resetGLButton_clicked()
{
    ui->openGLWidget->reset_cb();
}


void MainWindow::on_GLFileButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(
        this->window(), "Select an SWC", "", "SWC Files (*.swc)");

    if (!fileName.isEmpty())
    {
        ui->openGLWidget->setFile(fileName);
    }
}


void MainWindow::on_checkBox_2_stateChanged(int arg1)
{
    emit ViewerCheckBoxChanged(arg1);
}

void MainWindow::viewerActivated()
{
    ui->checkBox_2->setEnabled(false);
}

void MainWindow::viewerDeactivated()
{
    ui->checkBox_2->setEnabled(true);
}

