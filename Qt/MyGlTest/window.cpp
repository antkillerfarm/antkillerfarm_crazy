#include <QtWidgets>
#include <stdlib.h>
#include <stdio.h>

#include "threadtime.h"
#include "threadgl.h"
#include "threaddraw.h"
#include "glwidget.h"
#include "window.h"

Window::Window(char *strFormal, char *strTime, char *strNum)
{
    m_nLoop = 0;
    m_bEndFlag = true;

    m_nFlag = RGBA;
    m_nTime = 10;
    m_nPth = 5;
    m_type = 1;
    /*if(strFormal != NULL && strTime != NULL && strNum != NULL)
    {
        if(strcmp(strFormal, "RGBA") == 0)
        {
            m_nFlag = RGBA;
        }
        if(strcmp(strFormal, "YUY2") == 0)
        {
            m_nFlag = YUY2;
        }
        m_nTime = atoi(strTime);
        m_nPth = atoi(strNum);
    }
    else
    {
        m_nFlag = WRONG;
        m_nTime = WRONG;
        m_nPth = WRONG;
    }*/

    ReadImageOne();

    QGridLayout *mainLayout = new QGridLayout;
    m_glWidgets = new GLWidget(m_nPth, 0, 0);
    mainLayout -> addWidget(m_glWidgets, 0, 0); //first call initresizeGL

    setLayout(mainLayout);
    setWindowTitle(tr("test"));

    m_pTime = new ThreadTime(0);
    m_pDraw = new ThreadDraw(0);

    for(int i = 0; i < m_nPth; i++)
    {
        m_pcopyGl[i] = new ThreadGl(0);
        m_pcopyGl[i] -> glInit(i, m_glWidgets, m_pImage, m_nFlag);
        m_pcopyGl[i] -> start();
    }

    m_pTime -> StartTime(m_nTime, m_pDraw, m_glWidgets, m_pcopyGl, m_nPth);
    m_pDraw -> DrawInit(m_glWidgets);

    m_pTime -> start();
    m_pDraw -> start();
}

void Window::ReadImageOne()
{
    if(m_nFlag == RGBA)
    {
        DrawRgb();
    }
    else if(m_nFlag == YUY2)
    {
        DrawYuv();
    }
    else
    {
        printf("order woring\n");
        exit(1);
    }
}

void Window::DrawRgb()
{
    m_pImage = (uchar*)malloc(1920 * 1080 * 4);
    if(m_pImage == NULL)
    {
        return;
    }
    memset(m_pImage, 0, 1920 * 1080 *4);

    int i, j;
    uchar cRed, cGreen, cBlue;
    for(i = 0; i < 1080; i++)
    {
        for(j = 0; j < 1920; j++)
        {
            if(0 <j && j < 320)
            {
               cRed = 220;
               cGreen = 20;
               cBlue = 60;
            }
            if(320 < j && j < 640)
            {
                cRed = 139;
                cGreen = 0;
                cBlue = 139;
            }
            if(640 < j && j < 960)
            {
                cRed = 65;
                cGreen = 105;
                cBlue = 225;
            }
            if(960 < j && j < 1280)
            {
                cRed = 0;
                cGreen = 255;
                cBlue = 127;
            }
            if(1280 < j && j < 1600)
            {
                cRed = 255;
                cGreen = 215;
                cBlue = 0;
            }
            if(1600 < j && j < 1920)
            {
                cRed = 210;
                cGreen = 105;
                cBlue = 30;
            }

            *(m_pImage + i * 1920 * 4 + j * 4 + 0) = cRed;
            *(m_pImage + i * 1920 * 4 + j * 4 + 1) = cGreen;
            *(m_pImage + i * 1920 * 4 + j * 4 + 2) = cBlue;
            *(m_pImage + i * 1920 * 4 + j * 4 + 3) = 255;
        }
    }
}

void Window::DrawYuv()
{
    m_pImage = (uchar*)malloc(1920 * 1080 * 2);
    if(m_pImage == NULL)
    {
        return;
    }
    memset(m_pImage, 0, 1920 * 1080 * 2);

/*    int i, j;
    uchar cA;
    for(i = 0; i < 1080; i++)
    {
        for(j = 0; j < 1920; j++)
        {
            if(0 <j && j < 320)
            {
               cA = 128;

            }
            if(320 < j && j < 640)
            {
               cA = 0;
            }
            if(640 < j && j < 960)
            {
               cA = 128;
            }
            if(960 < j && j < 1280)
            {
               cA = 0;
            }
            if(1280 < j && j < 1600)
            {
               cA = 128;
            }
            if(1600 < j && j < 1920)
            {
               cA = 0;
            }

            *(m_pImage + i * 1920 * 2 + j * 2 + 0) = cA;
            *(m_pImage + i * 1920 * 2 + j * 2 + 1) = 255;

        }
    }
    */
    FILE* pFile = fopen("/home/zkb/d7/build-testv3-Desktop_Qt_5_0_2_GCC_64bit-Debug/test-mmap.yuv", "rb");
    if(pFile == NULL)
    {
        qDebug("file open fail!\n");
        return;
    }

    int readSucced =  fread(m_pImage, sizeof(uchar), 1920 * 1080 * 2, pFile);
    if(readSucced == 0)
    {
       printf("file end!\n");

    }
    qDebug("readSucced%d\n", readSucced);

    fclose(pFile);
}


