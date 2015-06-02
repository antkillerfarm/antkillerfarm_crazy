#include <QtWidgets>
#include <QtOpenGL>
#include "glwidget.h"
//#include <GL/glut.h>
#include <pthread.h>
#include <unistd.h>

GLWidget::GLWidget(int pth, QWidget *parent, QGLWidget *shareWidget)
    : QGLWidget(parent, shareWidget)
{
   m_pImage = NULL;
   m_nLoopC = 0;
   m_nLoopD = 0;
   m_nPth = pth;
   m_nFocusTex = 0;
}

GLWidget::~GLWidget()
{
    free(m_pImage);
    glDeleteTextures(m_nPth, textures);
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize GLWidget::sizeHint() const
{
    return QSize(1000, 1000);
}

void GLWidget::initializeGL()
{
    makeObject();
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -10.0f);

    DrawOne();
}

void GLWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-0.5*2, +0.5*2, +0.5*2, -0.5*2, 4.0*2, 15.0*2);
    glMatrixMode(GL_MODELVIEW);
}

bool GLWidget::OnTimeOut(uchar *pTex1, int nFormal, int texId)
{
     makeCurrent();

     m_pImage = pTex1;

     if(m_pImage != NULL)
     {
         if(nFormal == YUY2)
         {
             glBindTexture(GL_TEXTURE_2D, textures[texId]);
             glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1920, 1080, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, m_pImage);
         }

         if(nFormal == RGBA)
         {
             glBindTexture(GL_TEXTURE_2D, textures[texId]);
             glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1920, 1080, GL_RGBA, GL_UNSIGNED_BYTE, m_pImage);
         }

            mutex.lock();
            m_nLoopC++;
            mutex.unlock();

      }
      else
      {
         mutex.lock();
         m_nLoopC++;
         mutex.unlock();
      }

     return true;
}

void GLWidget::Draw()
{
    updateGL();
}

void GLWidget::makeObject()
{
   makeCurrent();  //focus on

   glGenTextures(m_nPth, textures);

   for(int i = 0; i < m_nPth; i++)
   {
        glBindTexture(GL_TEXTURE_2D, textures[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1920, 1080, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, m_pImage);
   }

    texCoords.append (QVector2D(1.0, 0.0));
    texCoords.append (QVector2D(0.0, 0.0));
    texCoords.append (QVector2D(0.0, 1.0));
    texCoords.append (QVector2D(1.0, 1.0));

    verticesOne.append(QVector3D(1, -1, 0));
    verticesOne.append(QVector3D(-1, -1, 0));
    verticesOne.append(QVector3D(-1, 1, 0));
    verticesOne.append(QVector3D(1, 1, 0));

}

void GLWidget::DrawOne()
{
    for(int j = 0; j < m_nPth; j++)
    {
        glVertexPointer(3, GL_FLOAT, 0, verticesOne.constData());
        glTexCoordPointer(2, GL_FLOAT, 0, texCoords.constData());
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glBindTexture(GL_TEXTURE_2D, textures[j]);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        m_nLoopD++;
    }
}
