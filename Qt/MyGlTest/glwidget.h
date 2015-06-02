#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QMutex>

class GLWidget : public QGLWidget
{

    Q_OBJECT

public:
    GLWidget(int pth,QWidget *parent = 0, QGLWidget *shareWidget = 0);
    ~GLWidget();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

public:
    bool OnTimeOut(uchar* pTex1, int nFormal, int texId);
    void DrawOne();
    static const int RGBA = 2;
    static const int YUY2 = 1;
    int m_nLoopC;
    int m_nLoopD;

signals:
    void GlSigTex();

public slots:
    void Draw();

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

private:
    static const int PTH_NUM = 20;
    void makeObject();
    GLuint textures[PTH_NUM];
    int m_nFocusTex;
    uchar* m_pImage;
    int m_nPth;
    QMutex mutex;

    QVector<QVector2D> texCoords;
    QVector<QVector3D> verticesOne;

};

#endif
