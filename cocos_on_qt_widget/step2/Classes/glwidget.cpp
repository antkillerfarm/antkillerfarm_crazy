#include "AppDelegate.h"
#include "glwidget.h"

USING_NS_CC;

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::DoubleBuffer), parent)
    , mouseMoveFunc(NULL)
    , mousePressFunc(NULL)
    , mouseReleaseFunc(NULL)
    , keyEventFunc(NULL)
{
  //resize(width, height);
}

GLWidget::~GLWidget()
{
}

void GLWidget::initWidget(AppDelegate* app)
{
  app->initWidget(this);
  //this->show();
  //cocos2d::Application::getInstance()->run();
  app->initGLContextAttrs();
  app->applicationDidFinishLaunching();
}

void GLWidget::paintGL()
{
#if 1
    auto director = Director::getInstance();
    director->mainLoop();
    printf("GLWidget::paintGL\r\n");
#else
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -10.0);
    glRotatef(0 / 16.0, 1.0, 0.0, 0.0);
    glRotatef(0 / 16.0, 0.0, 1.0, 0.0);
    glRotatef(0 / 16.0, 0.0, 0.0, 1.0);
    qglColor(Qt::red);
    glBegin(GL_QUADS);
        glNormal3f(0,0,-1);
        glVertex3f(-1,-1,0);
        glVertex3f(-1,1,0);
        glVertex3f(1,1,0);
        glVertex3f(1,-1,0);

    glEnd();
#endif
}

void GLWidget::resizeGL(int width, int height)
{
#if 1
    int side = qMin(width, height);
    //glViewport((width - side) / 2, (height - side) / 2, side, side);
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
#ifdef QT_OPENGL_ES_1
    glOrthof(-2, +2, -2, +2, 1.0, 15.0);
#else
    glOrtho(-2, +2, -2, +2, 1.0, 15.0);
#endif
    glMatrixMode(GL_MODELVIEW);
#endif
}


void GLWidget::setMouseMoveFunc(PTRFUN func)
{
    mouseMoveFunc = func;
}

void GLWidget::setMousePressFunc(PTRFUN func)
{
    mousePressFunc = func;
}

void GLWidget::setMouseReleaseFunc(PTRFUN func)
{
    mouseReleaseFunc = func;
}

void GLWidget::setKeyEventFunc(ACCEL_PTRFUN func)
{
    keyEventFunc = func;
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (mouseMoveFunc)
        mouseMoveFunc(event);

    QGLWidget::mouseMoveEvent(event);
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    if (mousePressFunc)
        mousePressFunc(event);

    QGLWidget::mousePressEvent(event);
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (mouseReleaseFunc)
        mouseReleaseFunc(event);

    QGLWidget::mouseReleaseEvent(event);
}

void GLWidget::keyPressEvent(QKeyEvent *e)
{
    if (keyEventFunc)
        keyEventFunc(e);

    QGLWidget::keyPressEvent(e);
}

void GLWidget::keyReleaseEvent(QKeyEvent *e)
{
    if (keyEventFunc)
        keyEventFunc(e);

    QGLWidget::keyReleaseEvent(e);
}
