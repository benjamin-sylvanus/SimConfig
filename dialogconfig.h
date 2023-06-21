#ifndef DIALOGCONFIG_H
#define DIALOGCONFIG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class DialogConfig; }
QT_END_NAMESPACE


class DialogConfig : public QDialog
{
    Q_OBJECT

public:
    DialogConfig(QWidget *parent = nullptr);
    DialogConfig(QWidget* parent, QVariantMap &config);
    void setConfig();
    QVariantMap getConfig();
    ~DialogConfig();

private slots:

private:
    Ui::DialogConfig *ui;
    QVariantMap config;
};

#endif // DIALOGCONFIG_H
