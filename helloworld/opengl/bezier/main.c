
#include <GL/glut.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

//窗口尺寸
#define SIZE_X 500
#define SIZE_Y 500

const double PI  = 3.14159265;
const double PI2 = 3.14159265 * 2;

#define LINES    20
#define PARTS    50
#define CP_PARTS 40
#define CP_STEP  1.0/CP_PARTS

// LINES 积累的线的数量，而非细分数量
// PARTS 单条曲线细分的数量
// CP_PARTS 控制点移动轨迹的细分量

typedef struct
{
    float x;
    float y;
}
point;

typedef struct 
{
    point cp[4];
    float index;
}
ControlPoint;

typedef struct
{
    float r;
    float g;
    float b;
    float a;
}
color;

point MultiLine[LINES][PARTS];
color LineColor[LINES];

int winID;

// PointOnCubicBeizer Function
point PointOnCubicBeizer(point cp[4], float t) 
{
    float cx, bx, ax, cy, by, ay;
    float tSquared, tCubed;
    point result;

    cx = 3.0 * (cp[1].x - cp[0].x);
    bx = 3.0 * (cp[2].x - cp[1].x) - cx;
    ax = cp[3].x - cp[0].x - cx - bx;

    cy = 3.0 * (cp[1].y - cp[0].y);
    by = 3.0 * (cp[2].y - cp[1].y) - cy;
    ay = cp[3].y - cp[0].y - cy - by;

    tSquared = t * t;
    tCubed = tSquared * t;

    result.x = ((ax * tCubed) + (bx * tSquared) + (cx * t) + cp[0].x);
    result.y = ((ay * tCubed) + (by * tSquared) + (cy * t) + cp[0].y);
    return result;
}

void display(void)
{   
    glBegin(GL_POINTS);
    
    for (int i = 0; i < LINES; i++)
    {
        for (int j = 0; j < PARTS; j++)
        {
            glColor4f( 1.0 , (float)j/(float)PARTS, 0.6, 0.15);
            //glColor4f( fabs((float)i - LINES/2)/(float)LINES , (float)j/(float)PARTS, 0.8, 0.4);
            glVertex3f(MultiLine[i][j].x, MultiLine[i][j].y, 0.0);
        }
    }
    glEnd();

    glutSwapBuffers();
}

//闲时回调函数，主要负责计算、延时
void idle(void)
{
    //临时坐标变量
    point coord;
    
    //主控制点，通过PointOnCubicBeizer函数和cp[4]数据获取坐标
    point MainCp[4];

    static int MainIdx = 0;

    //ControlPoint 结构包含 控制点、曲线进度信息
    static ControlPoint cp[4] = 
    {
        {{{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}}, 1.0},
        {{{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}}, 1.0},
        {{{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}}, 1.0},
        {{{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}}, 1.0},
    };

    glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); 
    glPushMatrix();

    //4个控制点的变化和进度
    for (int i = 0; i < 4; i++)
    {
        cp[i].index += CP_STEP;

        //index超过1.0时重新设定控制点
        if (cp[i].index >= 1.0)
        {
            //上一个终点作为新的起点
            cp[i].cp[0].x = cp[i].cp[3].x;   
            cp[i].cp[0].y = cp[i].cp[3].y;

            //使第一控制点与上次曲线对称（减少尖突的情况）
            cp[i].cp[1].x = cp[i].cp[3].x + (cp[i].cp[3].x - cp[i].cp[2].x);  
            cp[i].cp[1].y = cp[i].cp[3].y + (cp[i].cp[3].y - cp[i].cp[2].y);

            //后面两个点随机
            for (int ii = 2; ii < 4; ii++)
            {
                cp[i].cp[ii].x = (float) ( rand() % 500 - 250);
                cp[i].cp[ii].y = (float) ( rand() % 500 - 250);
            }

            //从1单位的CP_STEP开始，而非0.0，避免重合显示
            cp[i].index = CP_STEP;
        }

        coord = PointOnCubicBeizer( cp[i].cp, cp[i].index );
        //主线控制点坐标
        MainCp[i].x = coord.x;
        MainCp[i].y = coord.y;
    }
    
    //当前曲线坐标写入数组
    for (int i = 0; i < PARTS; i++)
    {
        coord = PointOnCubicBeizer(MainCp , (float)i / (float)PARTS );
        MultiLine[MainIdx][i].x = coord.x;
        MultiLine[MainIdx][i].y = coord.y;
    }

    //主线下标序号，达到数组末端时回到 0
    if ( MainIdx >= (LINES - 1) )
    {
        MainIdx = 0;
    }
    else
    {
        MainIdx++;
    }

    usleep(30000);
    glutPostRedisplay();
}

//窗口事件响应函数
void reshape(int Width,int Height)
{
    const float fa = 32.0;
    const float half = 250.0; 

    if (Width == Height) 
    {
        //视口范围
        glViewport(0, 0, Width, Height);
    }
    else if ( Width > Height ) 
    {
        glViewport(0, 0, Height, Height);
    }
    else
    {
        glViewport(0, 0, Width, Width);
    }

    //投影视图矩阵
    glMatrixMode(GL_PROJECTION);              
    glLoadIdentity();
    glOrtho(-half, half, -half, half, 0.0, 100.0);

    // 模型视图矩阵
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0,0.0,fa, 0.0,0.0,0.0, 0.0,1.0,fa);
            // 观察点，   朝向的坐标， 正上方朝向
}

//按键响应
void keypress(unsigned char key, int mousex, int mousey)
{
    switch (key)
    {
        case 'q':
            glutDestroyWindow(winID);
            exit(0);
            break;
        case 'Q':
            glutDestroyWindow(winID);
            exit(0);
            break;
    }
}

void init(void)
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glLineWidth( 2.0 );
    glPointSize( 8.0 );
    //glEnable(GL_DEPTH_TEST);    //开启深度缓冲
    glEnable(GL_BLEND);           //开启颜色混合
    glEnable(GL_POINT_SMOOTH);    //点平滑
    glEnable(GL_LINE_SMOOTH);     //线平滑

    srand(time(NULL));

    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            MultiLine[i][j].x = 0.0;
            MultiLine[i][j].y = 0.0;
        }
    }
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
            //显示模式   双缓冲         RGBA        深度缓冲
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
    glutInitWindowSize(SIZE_X, SIZE_Y);       //窗口大小
    glutInitWindowPosition(200, 200);         //位置
    winID = glutCreateWindow("MultiBezier");  //窗口句柄
    init();
    glutDisplayFunc(display);          //显示
    glutKeyboardFunc(keypress);        //按键事件响应
    glutReshapeFunc(reshape);          //窗口事件响应
    glutIdleFunc(idle);                //闲时回调函数
    glutMainLoop();                    //开始主循环

    return 0;
}
