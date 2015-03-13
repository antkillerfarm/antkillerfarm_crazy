#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "Box2D/Box2D.h"

class HelloWorld : public cocos2d::Layer
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);

    b2Body* createGround(int sx,int sy,int width,int height);
    void createSprite(cocos2d::Point location,char image[]);
    void update(float dt);
    //virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* unused_event);

    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);

private:
    b2World*world;
};

#endif // __HELLOWORLD_SCENE_H__
