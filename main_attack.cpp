#include "AtackRocket.h"
#include "Vec.cpp"
#include <GLFW/glfw3.h>
#include <GL/glu.h>
#include <random>
#include <ctime>
#include <cmath>
#include <vector>
#include <iostream>
#include <algorithm>

const float WORLD_W=5000.0f;
const float WORLD_H=5000.0f;
const int N_ATK=10;
const int TRAIL_LEN=120;

struct Cam{
    float ax=40.f,ay=25.f;
    float dist=11000.f;
    float cx=WORLD_W/2,cy=WORLD_H/2,cz=1000.f;
    double mx=0,my=0;
    bool down=false;
};

struct Col3{float r,g,b;};
struct Pt3{float x,y,z;};
struct Boom{float x,y,z,life,maxLife;Col3 col;};

struct RNG{
    std::mt19937 g;
    RNG():g(static_cast<unsigned int>(time(nullptr))){}
    float f(float a,float b){return std::uniform_real_distribution<float>(a,b)(g);}
    int   i(int a,int b)    {return std::uniform_int_distribution<int>(a,b)(g);}
};

GLFWwindow* win=nullptr;
int winW=1280,winH=720;
Cam cam;

std::vector<std::vector<Pt3>> atkTrail;
std::vector<Boom> booms;
bool done=false;

struct Target{float x,y;};

void onBtn(GLFWwindow*,int btn,int act,int){
    if(btn==GLFW_MOUSE_BUTTON_LEFT)cam.down=(act==GLFW_PRESS);
}
void onMove(GLFWwindow*,double x,double y){
    if(cam.down){
        cam.ax+=(float)(x-cam.mx)*0.4f;
        cam.ay-=(float)(y-cam.my)*0.4f;
        cam.ay=std::max(-89.f,std::min(89.f,cam.ay));
    }
    cam.mx=x;cam.my=y;
}
void onScroll(GLFWwindow*,double,double dy){
    cam.dist-=(float)dy*300.f;
    cam.dist=std::max(500.f,std::min(20000.f,cam.dist));
}
void onKey(GLFWwindow* w,int k,int,int act,int){
    if(k==GLFW_KEY_ESCAPE&&act==GLFW_PRESS)glfwSetWindowShouldClose(w,1);
}

void setCamera(){
    glMatrixMode(GL_PROJECTION);glLoadIdentity();
    gluPerspective(45.0,(double)winW/winH,10.0,60000.0);
    glMatrixMode(GL_MODELVIEW);glLoadIdentity();
    float rx=cam.ax*(float)M_PI/180.f,ry=cam.ay*(float)M_PI/180.f;
    float ex=cam.cx+cam.dist*cosf(ry)*cosf(rx);
    float ey=cam.cy+cam.dist*cosf(ry)*sinf(rx);
    float ez=cam.cz+cam.dist*sinf(ry);
    gluLookAt(ex,ey,ez,cam.cx,cam.cy,cam.cz,0,0,1);
}

void drawGround(){
    glColor4f(0.08f,0.12f,0.08f,1.f);
    glBegin(GL_QUADS);
    glVertex3f(0,0,0);glVertex3f(WORLD_W,0,0);
    glVertex3f(WORLD_W,WORLD_H,0);glVertex3f(0,WORLD_H,0);
    glEnd();
    glLineWidth(1.f);glColor4f(0.2f,0.35f,0.2f,0.6f);
    glBegin(GL_LINES);
    for(int i=0;i<=20;++i){
        float px=i*(WORLD_W/20),py=i*(WORLD_H/20);
        glVertex3f(px,0,1);glVertex3f(px,WORLD_H,1);
        glVertex3f(0,py,1);glVertex3f(WORLD_W,py,1);
    }
    glEnd();
}

void drawTarget(float x,float y){
    const float SZ=80.f;
    glLineWidth(2.f);glColor3f(1.f,0.2f,0.2f);
    glBegin(GL_LINES);
    glVertex3f(x-SZ,y,3.f);glVertex3f(x+SZ,y,3.f);
    glVertex3f(x,y-SZ,3.f);glVertex3f(x,y+SZ,3.f);
    glEnd();
    glBegin(GL_LINE_LOOP);
    for(int i=0;i<32;++i){
        float a=(float)i/32*2*(float)M_PI;
        glVertex3f(x+SZ*cosf(a),y+SZ*sinf(a),3.f);
    }
    glEnd();
}

void drawRocket(float x,float y,float z,Col3 c,float sc=1.f){
    glPushMatrix();glTranslatef(x,y,z);
    glPointSize(10.f*sc);glColor3f(c.r*1.5f,c.g*1.5f,c.b*1.5f);
    glBegin(GL_POINTS);glVertex3f(0,0,0);glEnd();
    float r0=8.f*sc,h0=25.f*sc;
    glColor3f(c.r,c.g,c.b);
    glBegin(GL_TRIANGLE_FAN);glVertex3f(0,0,h0);
    for(int i=0;i<=8;++i){float a=i*(2.f*(float)M_PI/8);glVertex3f(r0*cosf(a),r0*sinf(a),0);}
    glEnd();
    glColor3f(c.r*0.6f,c.g*0.6f,c.b*0.6f);
    glBegin(GL_TRIANGLE_FAN);glVertex3f(0,0,0);
    for(int i=0;i<=8;++i){float a=i*(2.f*(float)M_PI/8);glVertex3f(r0*cosf(a),r0*sinf(a),0);}
    glEnd();
    glPopMatrix();
}

void drawTrail(const std::vector<Pt3>& t,Col3 c){
    if(t.size()<2)return;
    glLineWidth(2.f);
    glBegin(GL_LINE_STRIP);
    for(int i=0;i<(int)t.size();++i){
        glColor4f(c.r,c.g,c.b,(float)i/t.size()*0.7f);
        glVertex3f(t[i].x,t[i].y,t[i].z);
    }
    glEnd();
}

void drawBoom(const Boom& b){
    float t=1.f-b.life/b.maxLife,r=200.f*t,a=b.life/b.maxLife;
    glPointSize(4.f);
    glBegin(GL_POINTS);
    for(int i=0;i<40;++i){
        float th=(float)i/40*2*(float)M_PI,ph=(float)i/40*(float)M_PI;
        glColor4f(b.col.r,b.col.g,b.col.b,a);
        glVertex3f(b.x+r*sinf(ph)*cosf(th),b.y+r*sinf(ph)*sinf(th),b.z+r*cosf(ph));
    }
    glEnd();
    glLineWidth(2.f);
    glBegin(GL_LINE_LOOP);
    for(int i=0;i<32;++i){
        float ang=(float)i/32*2*(float)M_PI;
        glColor4f(1.f,0.8f,0.2f,a*0.8f);
        glVertex3f(b.x+r*cosf(ang),b.y+r*sinf(ang),b.z);
    }
    glEnd();
}

void drawHUD(int total,bool simDone){
    glMatrixMode(GL_PROJECTION);glPushMatrix();glLoadIdentity();
    glOrtho(0,winW,0,winH,-1,1);
    glMatrixMode(GL_MODELVIEW);glPushMatrix();glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
    glColor4f(0,0.05f,0,0.75f);
    glBegin(GL_QUADS);
    glVertex2f(10,winH-10);glVertex2f(280,winH-10);
    glVertex2f(280,winH-60);glVertex2f(10,winH-60);
    glEnd();
    glColor4f(0.9f,0.3f,0.1f,0.9f);glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(10,winH-10);glVertex2f(280,winH-10);
    glVertex2f(280,winH-60);glVertex2f(10,winH-60);
    glEnd();
    float sq=18.f,gap=4.f,sy=winH-40.f;
    for(int i=0;i<total;++i){
        float x0=20.f+i*(sq+gap);
        glColor4f(0.9f,0.25f,0.1f,1.f);
        glBegin(GL_QUADS);
        glVertex2f(x0,sy);glVertex2f(x0+sq,sy);
        glVertex2f(x0+sq,sy+sq);glVertex2f(x0,sy+sq);
        glEnd();
    }
    if(simDone){
        glColor4f(0.9f,0.4f,0.1f,0.95f);
        glBegin(GL_QUADS);
        glVertex2f(winW/2-200,winH/2-30);glVertex2f(winW/2+200,winH/2-30);
        glVertex2f(winW/2+200,winH/2+30);glVertex2f(winW/2-200,winH/2+30);
        glEnd();
    }
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);glPopMatrix();
    glMatrixMode(GL_MODELVIEW);glPopMatrix();
}

int main(){
    if(!glfwInit()){std::cerr<<"GLFW fail\n";return -1;}
    glfwWindowHint(GLFW_SAMPLES,4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,1);
    win=glfwCreateWindow(winW,winH,"Attacker Only",nullptr,nullptr);
    if(!win){glfwTerminate();return -1;}
    glfwMakeContextCurrent(win);glfwSwapInterval(1);
    glfwSetMouseButtonCallback(win,onBtn);
    glfwSetCursorPosCallback(win,onMove);
    glfwSetScrollCallback(win,onScroll);
    glfwSetKeyCallback(win,onKey);
    glEnable(GL_DEPTH_TEST);glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_POINT_SMOOTH);glEnable(GL_LINE_SMOOTH);
    glClearColor(0.02f,0.03f,0.05f,1.f);

    RNG rng;

    std::vector<Target> targets(N_ATK);
    for(auto& t:targets){
        t.x=rng.f(300.f,WORLD_W-300.f);
        t.y=rng.f(300.f,WORLD_H-300.f);
    }

    std::vector<AtackRocket*> atk;
    for(int i=0;i<N_ATK;++i){
        float sx,sy;
        do{sx=rng.f(0,WORLD_W);sy=rng.f(0,WORLD_H);}
        while(fabsf(sx-targets[i].x)<200.f&&fabsf(sy-targets[i].y)<200.f);
        atk.push_back(new AtackRocket(100,sx,sy,0.f,targets[i].x,targets[i].y,0.f,0.f,500,3,25000.f));
    }

    for(auto* a:atk)a->launch();
    atkTrail.resize(N_ATK);

    const float DT=0.016f,MAX_T=300.f;
    float simT=0.f;
    Col3 cAtk={1.f,0.25f,0.1f},cExp={1.f,0.7f,0.1f};

    while(!glfwWindowShouldClose(win)){
        glfwPollEvents();
        glfwGetFramebufferSize(win,&winW,&winH);
        glViewport(0,0,winW,winH);

        if(!done&&simT<MAX_T){
            bool alive=false;

            for(int i=0;i<N_ATK;++i){
                AtackRocket* a=atk[i];
                if(!a->isActive())continue;
                alive=true;
                auto& tr=atkTrail[i];
                tr.push_back({a->getX(),a->getY(),a->getZ()});
                if((int)tr.size()>TRAIL_LEN)tr.erase(tr.begin());
                bool wasActive=a->isActive();
                a->update(DT);
                if(wasActive&&!a->isActive()){
                    booms.push_back({a->getX(),a->getY(),a->getZ(),1.5f,1.5f,cExp});
                }
            }

            for(auto& b:booms)b.life-=DT;
            booms.erase(std::remove_if(booms.begin(),booms.end(),[](const Boom& b){return b.life<=0.f;}),booms.end());

            simT+=DT;
            if(!alive)done=true;
        }

        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        setCamera();
        drawGround();
        for(auto& t:targets)drawTarget(t.x,t.y);
        for(int i=0;i<N_ATK;++i){
            drawTrail(atkTrail[i],cAtk);
            if(atk[i]->isActive())drawRocket(atk[i]->getX(),atk[i]->getY(),atk[i]->getZ(),cAtk,1.4f);
        }
        for(auto& b:booms)drawBoom(b);
        drawHUD(N_ATK,done);
        glfwSwapBuffers(win);
    }

    for(auto* a:atk)delete a;
    glfwDestroyWindow(win);glfwTerminate();
    return 0;
}