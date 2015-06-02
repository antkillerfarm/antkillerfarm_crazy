#include "threadtime.h"
#include "threadgl.h"
#include "threaddraw.h"
#include "glwidget.h"

ThreadTime::ThreadTime(QObject *parent) :
    QThread(parent)
{
}

void ThreadTime::StartTime(int time, ThreadDraw* pD, GLWidget *pG, ThreadGl *pC[], int size)
{
    if(pD == NULL || pC == NULL || pG == NULL)
    {
        qDebug("Thread Time Init Error\n");
        exit(0);
    }

    m_nTime = time;
    m_pD = pD;
    m_nSize = size;
    m_glWidgets = pG;

    for(int i = 0; i < m_nSize; i++)
    {
        m_pC[i] = pC[i];
    }
}

void ThreadTime::run()
{
    connect(this, SIGNAL(SigExit()), m_pD, SLOT(DrawExit()));

    for(int i = 0; i < m_nSize; i++)
    {
        connect(this, SIGNAL(SigExit()), m_pC[i], SLOT(GlExit()));

    }

    sleep(m_nTime);
    emit SigExit();

    qDebug("Copy is %d, Draw is %d\n", m_glWidgets -> m_nLoopC, m_glWidgets -> m_nLoopD);
    qDebug("End process\n");
}
