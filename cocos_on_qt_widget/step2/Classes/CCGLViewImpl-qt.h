#ifndef __CC_EGLViewIMPL_QT_H__
#define __CC_EGLViewIMPL_QT_H__

#include "base/CCRef.h"
#include "platform/CCCommon.h"
#include "platform/CCGLView.h"
#include "glwidget.h"

NS_CC_BEGIN

class CC_DLL QtGLViewImpl : public GLView
{
public:
    static QtGLViewImpl* create(GLWidget* qt_window);

    /*
     *frameZoomFactor for frame. This method is for debugging big resolution (e.g.new ipad) app on desktop.
     */

    //void resize(int width, int height);

    float getFrameZoomFactor() const;
    //void centerWindow();

    virtual void setViewPortInPoints(float x , float y , float w , float h);
    virtual void setScissorInPoints(float x , float y , float w , float h);

    bool windowShouldClose();
    void pollEvents();

    /* override functions */
    virtual bool isOpenGLReady() override;
    virtual void end() override;
    virtual void swapBuffers() override;
    virtual void setFrameSize(float width, float height) override;
    virtual void setIMEKeyboardState(bool bOpen) override;

    /*
     * Set zoom factor for frame. This method is for debugging big resolution (e.g.new ipad) app on desktop.
     */
    void setFrameZoomFactor(float zoomFactor);

protected:
    QtGLViewImpl();
    virtual ~QtGLViewImpl();

    bool initWithRect(GLWidget* qt_window, Rect rect, float frameZoomFactor);

    void updateFrameSize();
    bool initGlew();

    bool _captured;
    bool _supportTouch;

    float _frameZoomFactor;

    float _mouseX;
    float _mouseY;
    GLWidget* m_window;

private:
    CC_DISALLOW_COPY_AND_ASSIGN(QtGLViewImpl);
};

NS_CC_END   // end of namespace   cocos2d

#endif  // end of __CC_EGLViewImpl_QT_H__
