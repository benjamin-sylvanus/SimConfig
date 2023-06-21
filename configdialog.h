#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>
#include <QVariantMap>

QT_BEGIN_NAMESPACE
namespace Ui { class ConfigDialog; }
QT_END_NAMESPACE

class ConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigDialog(QWidget *parent = nullptr);

    ConfigDialog(QWidget* parent = nullptr, const QVariantMap& config = QVariantMap());
    QVariantMap editedConfig() const;
    ~ConfigDialog();

private:
    Ui::ConfigDialog *ui;
    QVariantMap m_editedConfig;
};

#endif // CONFIGDIALOG_H
