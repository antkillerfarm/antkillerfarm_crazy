#include <unistd.h>
#include "threadgl.h"
#include "glwidget.h"


ThreadGl::ThreadGl(QObject *parent) :
    QThread(parent)
{
}

void ThreadGl::glInit(int pthId, GLWidget *glWidget, uchar *pImage, int Formal)
{
    if(glWidget == NULL || pImage == NULL)
    {
        qDebug("Copy Error\n");
        exit(3);
    }

    m_pthId = pthId;
    m_glWidgets = glWidget;
    m_pImage = pImage;
    m_Formal = Formal;
    m_bEndFlag = true;

}

void ThreadGl::GlExit()
{
    m_bEndFlag = false;
}

void ThreadGl::run()
{
    connect(this, SIGNAL(SigCopy()), this, SLOT(Copy()), Qt::AutoConnection);
    printf("ID 0:%x\n", currentThreadId());
    while(1)
    {
        if(!m_bEndFlag)
        {
            break;
        }

        usleep(16000);
        printf("ID 1:%x\n", currentThreadId());
        emit SigCopy();
        //Copy();
    }

    qDebug("Copy End\n");
}

void ThreadGl::Copy()
{
    printf("ID 2:%x\n", currentThreadId());
    m_glWidgets -> OnTimeOut(m_pImage, m_Formal, m_pthId);
}
