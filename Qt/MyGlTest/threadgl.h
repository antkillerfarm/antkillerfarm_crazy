#ifndef THREADGL_H
#define THREADGL_H

#include <QThread>

class GLWidget;

class ThreadGl : public QThread
{
    Q_OBJECT
public:
    explicit ThreadGl(QObject *parent = 0);
    void glInit(int pthId, GLWidget* glWidget, uchar* pImage, int Formal);

protected:
    void run();

signals:
    void SigCopy();

public slots:
    void GlExit();
    void Copy();

private:
    int m_pthId;
    GLWidget* m_glWidgets;
    bool m_bEndFlag;
    uchar* m_pImage;
    int m_Formal;
    int m_Type;
};

#endif // THREADGL_H
