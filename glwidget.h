#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMouseEvent>
#include <QWheelEvent>
#include <iostream>
#include <fstream>
#include <sstream>
#include "datatemplate.h"
#include "util.h"
#ifdef __APPLE__
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/glu.h>
#endif

#pragma once
#pragma clang diagnostic ignored "-Wdeprecated-declarations"


struct Branch {
    QVector3D start;
    QVector3D end;
    float startRadius;
    float endRadius;
};

struct GLNode {
    QVector3D position;
    float radius;
};

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    GLWidget(QWidget *parent = nullptr);
    void drawCones(const QVector<QPair<GLNode, GLNode>> &points);
    //    void drawStructure();
    void createTree();
    void initLights();
    void drawSpheres();
    void drawCubes();
    void load();
    void reset_cb();
    void drawString(const char *str, int x, int y, float color[4], void *pVoid);
    void showInfo();
    void *font = GLUT_BITMAP_8_BY_13;
    void setFile(const QString filename);
    void preprocessLUT();


protected:
    void initializeGL() override;

    void resizeGL(int w, int h) override;
    void paintGL() override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    QPoint lastMousePos;
    float cameraAngleX = 0.0f;
    float cameraAngleY = 0.0f;
    float cameraDistance = 5.0f;

    QVector<Branch> branches;
    void drawTree(const QVector<Branch> &branches);

    QVector<QPair <GLNode,GLNode>> connections;
    QMap<int,GLNode> nodes;
    QVector<QPair<int,int>> edges;
    QVector<float> radii;
    bool isloaded = false;
    QString filename = "/Users/benjaminsylvanus/Documents/work/LUTViewer/src/data/cell.swc";
    bool loadWithTable;
    std::vector<std::uint64_t> lut;
    std::vector<std::vector<std::vector<int> > > lutdata;
    std::vector<int> lutindex;
    std::vector<std::vector<uint64_t>> dims;
    QVector3D center;
    std::vector<QVector3D> cubes;


public slots:
    void slot1(int arg1);

signals:
    void viewerActive();
    void viewerInactive();
};


