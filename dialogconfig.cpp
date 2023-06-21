#include "dialogconfig.h"
#include "./ui_dialogconfig.h"
#include <QLabel>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>

DialogConfig::DialogConfig(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogConfig)
{
    ui->setupUi(this);
    this->config = QVariantMap();
    connect(ui->pushButton, &QPushButton::clicked, this, &QDialog::accept);
}

DialogConfig::DialogConfig(QWidget* parent, QVariantMap& config)
    : QDialog(parent), config(config),
    ui(new Ui::DialogConfig)
{
    ui->setupUi(this);
    connect(ui->pushButton, &QPushButton::clicked, this, &QDialog::accept);
    QVBoxLayout * layout = ui->verticalLayout;

    for (auto it = config.begin(); it != config.end(); ++it) {
        QHBoxLayout* hbox = new QHBoxLayout;
        QLabel* label = new QLabel(it.key());
        hbox->addWidget(label);

        if (it.value().typeId() == QMetaType::Int) {
            QSpinBox* spinBox = new QSpinBox();
            spinBox->setRange(0, 10000);
            spinBox->setValue(it.value().toInt());
            hbox->addWidget(spinBox);

            connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged), [this, key = it.key()](int newValue) {
                this->config[key] = newValue;
            });


        } else if (it.value().typeId() == QMetaType::QString) {
            qInfo() << it->metaType();
            QLineEdit* edit = new QLineEdit(it.value().toString());
            hbox->addWidget(edit);

            connect(edit, &QLineEdit::textChanged, [this, key = it.key()](const QString& newValue) {
                this->config[key] = newValue;
            });

        }

        else if (it.value().typeId() == QMetaType::Double)
        {
            QDoubleSpinBox* spinBox = new QDoubleSpinBox();
            spinBox->setRange(0, 10000);
            spinBox->setValue(it.value().toInt());
            hbox->addWidget(spinBox);
            connect(spinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this, key = it.key()](double newValue) {
                this->config[key] = newValue;
            });
        }

        else if (it.value().typeId() == QMetaType::Bool)
        {
            QCheckBox* checkBox = new QCheckBox();
            checkBox->setCheckState(Qt::Checked);
            hbox->addWidget(checkBox);

            connect(checkBox, &QCheckBox::clicked, [this, key = it.key()](bool newValue) {
                this->config[key] = newValue;
            });
        }
        layout->addLayout(hbox);
    }
}

void DialogConfig::setConfig()
{

}

QVariantMap DialogConfig::getConfig()
{
    return config;
}

DialogConfig::~DialogConfig()
{
    delete ui;
}


