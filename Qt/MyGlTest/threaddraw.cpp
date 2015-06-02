#include <unistd.h>

#include "threaddraw.h"
#include "glwidget.h"

ThreadDraw::ThreadDraw(QObject *parent) :
    QThread(parent)
{
}

void ThreadDraw::DrawInit(GLWidget *glWidget)
{
    if(glWidget == NULL)
    {
        qDebug("Thread Draw Error\n");
        exit(1);
    }

    m_glWidget = glWidget;
    m_bEndFlag = true;

}

void ThreadDraw::DrawExit()
{
    m_bEndFlag = false;
}

void ThreadDraw::run()
{
    connect(this, SIGNAL(SigDraw()), m_glWidget, SLOT(Draw()));
    while(m_bEndFlag)
    {
        usleep(16000);
        emit SigDraw();
    }

    qDebug("End Draw\n");
}
