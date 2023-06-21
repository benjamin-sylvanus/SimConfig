#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "tree.h"
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr, QString out = "out");
    ~MainWindow();

signals:
    void selectDir();
    void NodeChanged(QString,QVariant);
    void ViewerCheckBoxChanged(int);

private slots:
    void RunMatlab();
    void on_openJson();
    void on_saveJson();
    void on_viewButton();
    void on_FileSelectedToView(QString path);
    void on_TreeWidgetSelectionChange(QTreeWidgetItem*,QTreeWidgetItem*);
    void on_pushButton_4_clicked();
    void on_resetGLButton_clicked();
    void on_GLFileButton_clicked();
    void on_checkBox_2_stateChanged(int arg1);
    void viewerActivated();
    void viewerDeactivated();

private:
    Ui::MainWindow *ui;
    Tree* tree;
    QString out;

    void initFormData();

};
#endif // MAINWINDOW_H
