#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QCloseEvent>

class GLWidget;
class ThreadDraw;
class ThreadTime;
class ThreadGl;

class Window : public QWidget
{
    Q_OBJECT

public:
    Window(char* strFormal, char* strTime, char *strNum);

    void TimeCopy();
    void TimeDraw();
    void ReadImageOne();
    void ReadPthBegin();
    void TotalCount();

private:
    static const int PTH_NUM = 20;
    static const int RGBA = 2;
    static const int YUY2 = 1;
    static const int WRONG = 0;

    void DrawRgb();
    void DrawYuv();

    ThreadTime* m_pTime;
    ThreadDraw* m_pDraw;
    ThreadGl*  m_pcopyGl[PTH_NUM];

    GLWidget *m_glWidgets;
    uchar* m_pImage;
    int m_nTime;
    int m_nFlag;
    int m_nPth;
    bool m_bEndFlag;
    int m_type;
    int m_pthSucced[PTH_NUM];
    int m_nLoop;
};

#endif
