#ifndef VIEWER_H
#define VIEWER_H

#include <QWidget>

namespace Ui {
class Viewer;
}

class Viewer : public QWidget
{
    Q_OBJECT

public:
    explicit Viewer(QWidget *parent = nullptr);
    ~Viewer();

private:
    Ui::Viewer *ui;
};

#endif // VIEWER_H
