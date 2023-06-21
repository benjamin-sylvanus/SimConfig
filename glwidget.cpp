#include "glwidget.h"
#include "QtWidgets/qapplication.h"
#include "QtWidgets/qcheckbox.h"
#include "QtWidgets/qprogressbar.h"
#include <QRandomGenerator>
#include <QTime>

void preprocessData(std::vector<std::vector<float> > &data, QMap<int, GLNode> &nodes, QVector<QPair<int, int> > &edges,
                    QVector3D &center);

GLWidget::GLWidget(QWidget *parent) : QOpenGLWidget(parent) {

    // Constructor implementation goes here
}

void GLWidget::initializeGL() {
    initializeOpenGLFunctions();
    glClearColor(0.1, 0.0, 0.1, 1.0f);
    glMatrixMode(GL_PROJECTION);
    gluPerspective(45.0f, 1.0f, 3.0f, 50.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -5.0);
    initLights();


    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void GLWidget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
}

void GLWidget::paintGL() {
    if (isloaded) {
        glLoadIdentity();
        glTranslatef(0.0f, 0.0f, -cameraDistance);
        glRotatef(cameraAngleX, 1.0f, 0.0f, 0.0f);
        glRotatef(cameraAngleY, 0.0f, 1.0f, 0.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);

        // Draw the scene
        drawCones(connections);
        drawSpheres();
        if (loadWithTable) {
            drawCubes();
        }
        showInfo();
        update();
    }
}

void GLWidget::drawCones(const QVector<QPair<GLNode, GLNode>> &points) {
    GLUquadric *quad = gluNewQuadric();

    for (int i = 0; i < points.size(); ++i) {
        QPair<GLNode, GLNode> p = points[i];

        QVector3D start = p.first.position;
        QVector3D end = p.second.position;
        float startRadius = p.first.radius;
        float endRadius = p.second.radius;

        QVector3D direction = end - start;
        float height = direction.length();
        direction.normalize();

        QVector3D up(0.0f, 0.0f, 1.0f);
        QVector3D right = QVector3D::crossProduct(direction, up);
        up = QVector3D::crossProduct(right, direction);

        float m[16] = {right.x(), right.y(), right.z(), 0.0f, up.x(), up.y(), up.z(), 0.0f, direction.x(),
                       direction.y(), direction.z(), 0.0f, start.x(), start.y(), start.z(), 1.0f};

        glPushMatrix();
        glMultMatrixf(m);
        gluCylinder(quad, startRadius, endRadius, height, 16, 4);
        glPopMatrix();
    }

    gluDeleteQuadric(quad);
}

void GLWidget::mousePressEvent(QMouseEvent *event) {
    lastMousePos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event) {
    int dx = event->x() - lastMousePos.x();
    int dy = event->y() - lastMousePos.y();

    if (event->buttons() & Qt::LeftButton) {
        cameraAngleX += dy;
        cameraAngleY += dx;
    }

    lastMousePos = event->pos();

    update();
}

void GLWidget::wheelEvent(QWheelEvent *event) {
    cameraDistance -= event->angleDelta().y() / 800.0f;
    update();
}

void GLWidget::slot1(int arg1) {

    qDebug() << arg1 << "Recieved";

    switch (arg1) {
        case (Qt::CheckState::Checked):
            loadWithTable = true;
            break;
        case (Qt::CheckState::Unchecked):
            loadWithTable = false;
            break;

        default:
            qDebug() << "Unexpected Value";
            exit(1);
    }

}

std::vector<std::vector<float> > loadSWC(std::string basicString) {
    std::ifstream file(basicString);
    std::string str;
    std::vector<std::vector<float> > swc;
    while (std::getline(file, str)) {
        std::istringstream iss(str);
        if (str.empty())
            continue;
        else if (str[0] == '#' || str[0] == ' ' || str[0] == '\n') {
            continue;
        } else {
            std::vector<float> node;
            std::vector<int> edge;
            float x, y, z, radius, parent, id;
            if (!(iss >> id >> x >> y >> z >> radius >> parent)) {
                break;
            } // error
            node.push_back(id);
            node.push_back(x);
            node.push_back(y);
            node.push_back(z);
            node.push_back(radius);
            node.push_back(parent);
            swc.push_back(node);
        }
    }
    return swc;
}

void
preprocessData(std::vector<std::vector<float> > &data, QMap<int, GLNode> &nodes, QVector<QPair<int, int> > &edges,
               QVector3D &center) {

    nodes.clear();
    edges.clear();
    // set the first connections parent to 1
    data[0][5] = 1;

    // add each connection to the global variables gstart and gend
    for (int i = 0; i < data.size(); i++) {
        GLNode n;
        n.position = QVector3D(data[i][1], data[i][2], data[i][3]);
        n.radius = data[i][4];
        nodes.insert(i, n);

        int cid = (int) data[i][0] - 1;
        int cparent = (int) data[i][5] - 1;

        // add the connection to the global variables
        // if the radius of the parent is smaller than the child reverse the connection
        if (nodes[cparent].radius < nodes[cid].radius) {
            QPair<int, int> edge(cparent, cid);
            edges.push_back(edge);
        } else {
            QPair<int, int> edge(cid, cparent);
            edges.push_back(edge);
        }
    }

    // calculate the center of the data
    for (int i = 0; i < data.size(); i++) {
        center += nodes[i].position;
    }
    center /= data.size();

    // translate the data to the center
    for (int i = 0; i < data.size(); i++) {
        nodes[i].position -= center;
    }


    // threshold the radius at 10% of the average radius
    float averageRadius = 0;
    for (int i = 0; i < data.size(); i++) {
        averageRadius += nodes[i].radius;
    }
    averageRadius /= data.size();
    float threshold = averageRadius * 0.2;
    for (int i = 0; i < data.size(); i++) {
        if (nodes[i].radius < threshold) {
            nodes[i].radius = threshold;
        }
    }

    // print the nodes
    for (int i = 0; i < data.size(); i++) {
        qDebug() << nodes[i].position << nodes[i].radius;
    }
}

void GLWidget::createTree() {

    connections.clear();
    std::vector<std::vector<float> > swc = loadSWC(filename.toStdString());

    preprocessData(swc, nodes, edges, center);
    if (loadWithTable) {
        // load the lookup table using the datatemplate class
        datatemplate<std::uint64_t> lutreader("/Users/benjaminsylvanus/Documents/work/LUTViewer/src/data/lut.bin");
        lut = lutreader.data;

        // load the index table using the datatemplate class
        datatemplate<int> indexreader("/Users/benjaminsylvanus/Documents/work/LUTViewer/src/data/index.bin");
        lutindex = indexreader.data;

        dims = readdims("/Users/benjaminsylvanus/Documents/work/LUTViewer/src/data/dims.txt");

        preprocessLUT();

    }
    for (int i = 0; i < edges.size(); i++) {

        QPair<GLNode, GLNode> connection;
        connection.first = nodes[edges[i].first];
        connection.second = nodes[edges[i].second];
        connections.push_back(connection);
    }
    isloaded = true;

}

void GLWidget::initLights() {
    // set up light colors (ambient, diffuse, specular)
    GLfloat lightKa[] = {1.0f, 1.0f, 1.0f, 1.0f};  // ambient light
    GLfloat lightKd[] = {1.0f, 1.0f, 1.0f, 0.10f};  // diffuse light
    GLfloat lightKs[] = {0.1f, 0.1f, 0.1f, 1.0f};     // specular light
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightKa);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightKd);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightKs);
    // set the linear attenuation factor
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 1.0f);

    // position the light
    float lightPos[4] = {50, -50, 0, 0}; // positional light
//    glLightfv(GL_LIGHT0,GL_AMBIENT, lightPos);
    glLightfv(GL_LIGHT0,GL_LIGHT_MODEL_AMBIENT, lightPos);
    glEnable(GL_LIGHT0);                        // MUST enable each light source after configuration
}

void GLWidget::drawSpheres() {
    for (auto it: nodes) {
        glPushMatrix();
        glTranslatef(it.position.x(), it.position.y(), it.position.z());
        glutWireSphere(it.radius, 8, 8);
        glPopMatrix();
    }
}

void GLWidget::drawCubes() {
    float ambient[] = {1.0f, 1.0f, 0.1f, 0.1f};
    float diffuse[] = {0.5f, 0.1f, 0.1f, 0.1f};
    float specular[] = {1.0, 1.0, 1.0, 1.0f};
    float shininess = 8;
    glMaterialfv(GL_LEFT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
//    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
//    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
//
//    glDepthFunc(GL_LEQUAL);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//    glBlendFunc(GL_SRC0_ALPHA, GL_ONE_MINUS_DST_ALPHA);


//    glDisable(GL_LIGHTING);
//    glDisable(GL_DEPTH_TEST);
    for (auto it: cubes) {
        glPushMatrix();
        glColor4f(1.0f, 0.1f, 0.1f, 0.01f);
        glTranslatef(it.x(), it.y(), it.z());
        glutSolidCube(1);
        glPopMatrix();
    }

    // reset diffuse to default white
    float diffuse2[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse2);
    glMaterialfv(GL_LEFT, GL_AMBIENT, diffuse2);


    glLineWidth(3.0f);
    // draw a cube at each point
    for (auto it: cubes) {
        glPushMatrix();
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        glTranslatef(it.x(), it.y(), it.z());
        glutWireCube(1);
        glPopMatrix();
    }
    glLineWidth(1.0f);

//    glEnable(GL_LIGHTING);
//    glEnable(GL_DEPTH_TEST)
}


void GLWidget::load() {
    emit viewerActive();
    createTree();
    update();
}

void GLWidget::reset_cb() {
    emit viewerInactive();
    this->clearMask();
    this->isloaded = false;
    update();
}

void GLWidget::drawString(const char *str, int x, int y, float color[], void *pVoid) {
    glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT); // lighting and color mask
    glDisable(GL_LIGHTING);     // need to disable lighting for proper text color
    glDisable(GL_TEXTURE_2D);

    glColor4fv(color);          // set text color
    glRasterPos2i(x, y);        // place text position

    // loop all characters in the string
    while (*str) {
        glutBitmapCharacter(pVoid, *str);
        ++str;
    }

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    glPopAttrib();

}

void GLWidget::showInfo() {

    // backup current model-view matrix
    glPushMatrix();                     // save current modelview matrix
    glLoadIdentity();                   // reset modelview matrix

    // set to 2D orthogonal projection
    glMatrixMode(GL_PROJECTION);        // switch to projection matrix
    glPushMatrix();                     // save current projection matrix
    glLoadIdentity();                   // reset projection matrix
    gluOrtho2D(0, this->size().width(), 0, this->size().height());  // set to orthogonal projection

    const int FONT_HEIGHT = 14;
    float color[4] = {1, 1, 1, 1};

    // for print infos
    std::stringstream ss;
    ss << std::fixed << std::setprecision(3);
    ss << "Press D to change drawing mode." << std::ends;
    drawString(ss.str().c_str(), 1, 1, color, font);
    ss.str("");

    ss << "Press B to toggle Lookup Table." << std::ends;
    drawString(ss.str().c_str(), 1, 1 + FONT_HEIGHT, color, font);
    ss.str("");

    // unset floating format
    ss << std::resetiosflags(std::ios_base::fixed | std::ios_base::floatfield);

    // restore projection matrix
    glPopMatrix();                   // restore to previous projection matrix

    // restore modelview matrix
    glMatrixMode(GL_MODELVIEW);      // switch to modelview matrix
    glPopMatrix();                   // restore to previous modelview matrix
}


void GLWidget::setFile(const QString name) {

    // set a new filename
    filename = name;

    // load the file
    load();
}

void GLWidget::preprocessLUT() {
    cubes.clear();

    // extract the dimensions of the swc lut and index from the dims
    int SWCDIMS[2] = {static_cast<int>(dims[0][0]), static_cast<int>(dims[0][1])};
    int LUTDIMS[3] = {static_cast<int>(dims[1][0]), static_cast<int>(dims[1][1]), static_cast<int>(dims[1][2])};
    int INDDIMS[3] = {static_cast<int>(dims[2][0]), static_cast<int>(dims[2][1]), static_cast<int>(dims[2][2])};

    std::cout << "SWC dims: " << SWCDIMS[0] << " " << SWCDIMS[1] << std::endl;
    std::cout << "LUT dims: " << LUTDIMS[0] << " " << LUTDIMS[1] << " " << LUTDIMS[2] << std::endl;
    std::cout << "Index dims: " << INDDIMS[0] << " " << INDDIMS[1] << " " << INDDIMS[2] << std::endl;

    // set the dims of the lut
    lutdata.resize(LUTDIMS[0], std::vector<std::vector<int> >(LUTDIMS[1], std::vector<int>(LUTDIMS[2], 0)));

    std::vector<int> localdims = {LUTDIMS[0], LUTDIMS[1], LUTDIMS[2]};

    // lut has the form [y,x,z]
    for (int i = 0; i < LUTDIMS[0]; i++) {
        for (int j = 0; j < LUTDIMS[1]; j++) {
            for (int k = 0; k < LUTDIMS[2]; k++) {
                // calculate the index of the lut
                std::vector<int> subscript = {i, j, k};
                int ind = s2i(subscript, localdims);
                lutdata[i][j][k] = lut[ind];
            }
        }
    }

    // create a vector of cubes to draw
    for (int i = 0; i < LUTDIMS[0]; i++) {
        for (int j = 0; j < LUTDIMS[1]; j++) {
            for (int k = 0; k < LUTDIMS[2]; k++) {
                if (lutdata[i][j][k] > 0) {
                    cubes.push_back(QVector3D(i, j, k));
                }
            }
        }
    }

    // Translate the cubes to the correct position
    for (int i = 0; i < cubes.size(); i++) {
        cubes[i] -= center;
    }

}



