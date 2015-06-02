#ifndef THREADTIME_H
#define THREADTIME_H

#include <QThread>

class ThreadDraw;
class ThreadGl;
class GLWidget;

class ThreadTime : public QThread
{
    Q_OBJECT
public:
    explicit ThreadTime(QObject *parent = 0);
    void StartTime(int time, ThreadDraw* pD, GLWidget* pG, ThreadGl* pC[], int size);

protected:
    void run();

signals:
    void SigExit();

public slots:

private:
    int m_nTime;
    int m_nSize;
    GLWidget* m_glWidgets;
    ThreadDraw* m_pD;
    ThreadGl* m_pC[20];
};

#endif // THREADTIME_H
