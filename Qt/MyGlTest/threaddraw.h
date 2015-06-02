#ifndef THREADDRAW_H
#define THREADDRAW_H

#include <QThread>

class GLWidget;

class ThreadDraw : public QThread
{
    Q_OBJECT
public:
    explicit ThreadDraw(QObject *parent = 0);
    void DrawInit(GLWidget* glWidget);

protected:
    void run();

signals:
    void SigDraw();

public slots:
   void DrawExit();

private:
   GLWidget* m_glWidget;
   bool m_bEndFlag;
};

#endif // THREADDRAW_H
