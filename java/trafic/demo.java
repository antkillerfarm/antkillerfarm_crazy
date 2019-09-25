//auther antkillerfarm
package trafic;

import java.awt.*;
import java.awt.event.*;
import java.applet.*;

public class demo extends Applet {//主类
  public void start(){//启动各个线程
    Graphics g;
    g=getGraphics();
    light light1=new light(g,165,200,true);//左侧信号灯的实例
    light1.start();
    light light2=new light(g,585,200,false);//右侧信号灯的实例
    light2.start();
    map map1=new map();
    carmaker carmaker1=new carmaker(g,light1,light2,map1);
    carmaker1.start();
  }
  public void paint(Graphics g) {
    mapdraw(g);
  }
  public void mapdraw(Graphics g){//画地图
    g.setColor(new Color(255,128,0));
    g.fillRect(0,0,125,140);
    g.fillRect(205,0,340,140);
    g.fillRect(625,0,125,140);
    g.fillRect(0,260,125,140);
    g.fillRect(205,260,340,140);
    g.fillRect(625,260,125,140);
    g.setColor(new Color(0,192,0));
    g.fillRect(45,190,80,20);
    g.fillRect(205,190,340,20);
    g.fillRect(625,190,85,20);
    g.setColor(new Color(0,0,0));
    g.drawRect(0,0,125,140);
    g.drawRect(205,0,340,140);
    g.drawRect(625,0,125,140);
    g.drawRect(0,260,125,140);
    g.drawRect(205,260,340,140);
    g.drawRect(625,260,125,140);
    g.drawRect(45,190,80,20);
    g.drawRect(205,190,340,20);
    g.drawRect(625,190,85,20);
  }
}
class car extends Thread{//有关汽车行为的类
  public map mymap;
  public int x,y;
  public Math math;
  public int predirect;//汽车下一步的行驶方向
  public int direct;//汽车当前的行驶方向，0为向东，1为向南，2为向西，
  //3为向北，4为特殊处理时的保留字
  public Graphics g;
  public light light1,light2;
  public int appearx[]={0,750,150,180,570,600};//汽车出现地点的横坐标
  public int appeary[]={235,165,0,400,0,400};//汽车出现地点的纵坐标
  public int appeard[]={0,2,1,3,1,3};//汽车出现时的行驶方向
  public car(Graphics g,light light1,light light2,map map1) {
    this.g=g;
    this.light1=light1;
    this.light2=light2;
    mymap=map1;
  }
  public void run(){
    g.setColor(Color.red);
    appearcar();
    while(isin()){
      move();
    }
  }
  public boolean isin(){//判断汽车是否在界内
    if(x>=-10&&x<=760&&y>=-10&&y<=410) return true;
    else return false;
  }
  public void move(){//画出下一刻的汽车
    mymap.mapin(x,y);
    if(ispreturn()){
      if(mymap.canturn[mymap.num1][1]==4){
        predirect=mymap.canturn[mymap.num1][0];//只有1种转向
      }
      else{ int i;//有2种转向
        if(math.random()>0.5) i=0;
        else i=1;
        predirect=mymap.canturn[mymap.num1][i];
      }
    }
    if(islight1()&&direct==predirect){//汽车到左侧的信号灯处，且直行时
      if (direct==0||direct==2) {//汽车为东西向行驶时
       while (!light1.isred){
          try{sleep(100);}
          catch(InterruptedException e){};
       }
      }
      else {//汽车为南北向行驶时
       while (light1.isred){
          try{sleep(100);}
          catch(InterruptedException e){};
       }
      }
    }
    if(islight2()&&direct==predirect){//汽车到右侧的信号灯处，且直行时
      if (direct==0||direct==2) {
       while (!light2.isred){
           try{sleep(100);}
           catch(InterruptedException e){};
       }
      }
      else {
       while (light2.isred){
           try{sleep(100);}
           catch(InterruptedException e){};
       }
      }
    }
    if(isturn()){
      direct=predirect;
    }
    while (iswait()){
         try{sleep(100);}
           catch(InterruptedException e){};
     }
      g.clearRect(x-10,y-10,20,20);//清除上一刻的汽车图像
      mymap.mapin(x,y);
      mymap.writeinline(false);
      switch (direct){//根据汽车的行驶方向，确定其下一刻的坐标
        case 0:x=x+5;break;
        case 1:y=y+5;break;
        case 2:x=x-5;break;
        default:y=y-5;
      }
      g.fillRect(x-10,y-10,20,20);//画出下一刻的汽车图像
      mymap.mapin(x,y);
      mymap.writeinline(true);
      try{sleep(100);}
      catch(InterruptedException e){};
   }
 public void appearcar(){//随机产生汽车出现的地点和方向
   int i;
   double d=math.random();
   if (d<0.3) i=0;
   else if(d<0.6) i=1;
        else if(d<0.7) i=2;
             else if(d<0.8) i=3;
                  else if(d<0.9) i=4;
                       else i=5;
   x=appearx[i];y=appeary[i];direct=appeard[i];
   mymap.mapin(x,y);mymap.writeinline(true);
 }
 public boolean iswait(){//判断汽车是否应等待，以避免相撞
   boolean bet=false;
   switch(y) {
    case 165:if(x>30)
              for(int i=1;i<6;i++) {
                if(mymap.line1[x/5-i]) bet=true;
              };break;
    case 235:if(x<720)
              for(int i=1;i<6;i++) {
                if(mymap.line2[x/5+i]) bet=true;
              }break;
    default:;
   }
   switch(x) {
     case 150:if(y<370)
               for(int i=1;i<6;i++) {
                 if(mymap.line3[y/5+i]) bet=true;
              }break;
     case 180:if(y>30)
               for(int i=1;i<6;i++) {
                 if(mymap.line4[y/5-i]) bet=true;
              }break;
     case 570:if(y<370)
               for(int i=1;i<6;i++) {
                 if(mymap.line5[y/5+i]) bet=true;
              }break;
     case 600:if(y>30)
               for(int i=1;i<6;i++) {
                 if(mymap.line6[y/5-i]) bet=true;
              };break;
    default:;
   }
   return(bet);
 }
 public boolean islight1(){//判断汽车是否到了左侧的信号灯处
    for(int i=0;i<4;i++){
      if (x==mymap.light1x[i]&&y==mymap.light1y[i])
        return true;
    }
    return false;
  }
 public boolean islight2(){//判断汽车是否到了右侧的信号灯处
     for(int i=0;i<4;i++){
       if (x==mymap.light2x[i]&&y==mymap.light2y[i])
         return true;
     }
       return false;
  }
  public boolean isturn(){//判断汽车是否到了转向点
     for(int i=0;i<6;i++){
       if (x==mymap.turnx[i]&&(y==165||y==235))
         return true;
     }
       return false;
  }
  public boolean ispreturn(){//判断汽车是否到了决定转向的点
  //因为汽车右转不受信号灯的影响，故转向点并非决定转向的点
     for(int i=0;i<12;i++){
      if (x==mymap.preturnx[i]&&y==mymap.preturny[i])
        {mymap.num1=i;return true;}
     }
       return false;
   }
}
class light extends Thread{//有关灯行为的类
  public boolean isred;//反映信号灯状态的变量
  public Graphics g;
  public int x,y;//信号灯组的坐标
  public light(Graphics g,int x,int y,boolean isred){
   this.g=g;
   this.x=x;
   this.y=y;
   this.isred=isred;
  }
  public void run(){//定时改变灯的状态
    g.setColor(Color.red);
    while (true){
     if (isred) {isred=false;g.setColor(Color.green);}
     else isred=true;
     g.fillArc(x-40,y-75,10,10,0,360);
     g.fillArc(x+30,y+65,10,10,0,360);
     if (isred) {g.setColor(Color.green);}
     else g.setColor(Color.red);
     g.fillArc(x-55,y+50,10,10,0,360);
     g.fillArc(x+45,y-60,10,10,0,360);
     g.setColor(Color.red);
     try{sleep(2000);}
     catch(InterruptedException e){};
    }
  }
}
class map{//与地图有关的类
  public int x,y;
  public int light1x[]={110,180,220,150};//左侧信号灯处的横坐标
  public int light1y[]={235,275,165,125};//左侧信号灯处的纵坐标
  public int light2x[]={530,600,640,570};//右侧信号灯处的横坐标
  public int light2y[]={235,275,165,125};//右侧信号灯处的纵坐标
  public int turnx[]={25,150,180,570,600,725};//转向点的横坐标
  public int preturnx[]={25,25,150,110,220,180,570,530,640,600,725,725};
  //决定转向的点的横坐标
  public int preturny[]={165,235,125,235,165,275,125,235,165,275,165,235};
  //决定转向的点的纵坐标
  public int canturn[][]={{1,2},{0,4},{1,2},{0,1},
                          {2,3},{0,3},{1,2},{0,1},
                          {2,3},{0,3},{2,4},{0,3}};
  //决定转向的点上可供选择的转向方向
  public int num1;
  public boolean line1[]=new boolean[151];//东西向行驶的汽车的横坐标
  public boolean line2[]=new boolean[151];
  public boolean line3[]=new boolean[81];//南北向行驶的汽车的纵坐标
  public boolean line4[]=new boolean[81];
  public boolean line5[]=new boolean[81];
  public boolean line6[]=new boolean[81];
  public map(){
    for(int i=0;i<151;i++){
      line1[i]=false;
      line2[i]=false;
    }
    for(int i=0;i<81;i++){
      line3[i]=false;
      line4[i]=false;
      line5[i]=false;
      line6[i]=false;
    }
  }
  public void mapin(int x,int y){
   this.x=x;
   this.y=y;
  }
  public void writeinline(boolean id){//将id值写入保存汽车坐标的数组
    switch(y) {
      case 165:if(x>30&&x<720) line1[x/5]=id;break;
      case 235:if(x>30&&x<720) line2[x/5]=id;break;
      default:;
    }
    switch(x) {
      case 150:if(y>30&&y<370) line3[y/5]=id;break;
      case 180:if(y>30&&y<370) line4[y/5]=id;break;
      case 570:if(y>30&&y<370) line5[y/5]=id;break;
      case 600:if(y>30&&y<370) line6[y/5]=id;break;
      default:;
    }
   }
}
class carmaker extends Thread{//定时产生汽车实例的类
  public light light1,light2;
  public Graphics g;
  public map map1;
  public carmaker(Graphics g,light light1,light light2,map map1){
    this.light1=light1;
    this.light2=light2;
    this.g=g;
    this.map1=map1;
  }
  public void run(){
     while (true){
       car car1=new car(g,light1,light2,map1);
       car1.start();
       try{carmaker.sleep(1000);}
       catch(InterruptedException e){};
    }
  }
}
