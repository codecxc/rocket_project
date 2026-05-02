#include "AtackRocket.h"
#include "DefenderRocket.h"
#include "map.h"
#include "Parametrs.h"
#include "Vec.cpp"
#include <GLFW/glfw3.h>
#include <GL/glu.h>
#include <random>
#include <ctime>
#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>

const float WORLD_W    =15000.f;
const float WORLD_H    =15000.f;
const int   N_CELLS    =10;
const float CELL_SZ    =300.f;
const int   N_ATK      =N_CELLS;
const float MIN_DIST   =800.f;
const float DETECT_DIST=3000.f;
const int   TRAIL_LEN  =200;
const float DEF_MASS =200.f;
const float DEF_FUEL =400.f;
const float DEF_F    =30000.f;
const float DT       =0.008f;

struct Cam{
    float ax=35.f,ay=28.f;
    float dist=22000.f;
    float cx=WORLD_W/2,cy=WORLD_H/2,cz=2000.f;
    double mx=0,my=0;
    bool down=false;
};

struct Col3{float r,g,b;};
struct Pt3 {float x,y,z;};
struct Boom{float x,y,z,life,maxLife;Col3 col;};

GLFWwindow* win=nullptr;
int winW=1280,winH=720;
Cam cam;

std::vector<std::vector<Pt3>> atkTrail,defTrail;
std::vector<Boom> booms;
int nHit=0;
bool done=false;

struct LogPt{float t,x,y,z;int id;bool isAtk;};
std::vector<LogPt> log_data;

struct RNG{
    std::mt19937 g;
    RNG():g(static_cast<unsigned int>(time(nullptr))){}
    float f(float a,float b){return std::uniform_real_distribution<float>(a,b)(g);}
    int   i(int a,int b)    {return std::uniform_int_distribution<int>(a,b)(g);}
};

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
    cam.dist-=(float)dy*500.f;
    cam.dist=std::max(1000.f,std::min(50000.f,cam.dist));
}
void onKey(GLFWwindow* w,int k,int,int act,int){
    if(k==GLFW_KEY_ESCAPE&&act==GLFW_PRESS)glfwSetWindowShouldClose(w,1);
}

void setCamera(){
    glMatrixMode(GL_PROJECTION);glLoadIdentity();
    gluPerspective(45.0,(double)winW/winH,10.0,150000.0);
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
    glLineWidth(1.f);glColor4f(0.18f,0.3f,0.18f,0.5f);
    glBegin(GL_LINES);
    for(int i=0;i<=20;++i){
        float px=i*(WORLD_W/20),py=i*(WORLD_H/20);
        glVertex3f(px,0,1);glVertex3f(px,WORLD_H,1);
        glVertex3f(0,py,1);glVertex3f(WORLD_W,py,1);
    }
    glEnd();
}

void drawCell(float cx,float cy,float sz){
    float h=sz/2,z=2.f;
    glColor4f(0.1f,0.6f,0.2f,0.45f);
    glBegin(GL_QUADS);
    glVertex3f(cx-h,cy-h,z);glVertex3f(cx+h,cy-h,z);
    glVertex3f(cx+h,cy+h,z);glVertex3f(cx-h,cy+h,z);
    glEnd();
    glLineWidth(2.f);glColor3f(0.2f,1.f,0.3f);
    glBegin(GL_LINE_LOOP);
    glVertex3f(cx-h,cy-h,z);glVertex3f(cx+h,cy-h,z);
    glVertex3f(cx+h,cy+h,z);glVertex3f(cx-h,cy+h,z);
    glEnd();
    glPointSize(8.f);glColor3f(0.5f,1.f,0.5f);
    glBegin(GL_POINTS);glVertex3f(cx,cy,z+1);glEnd();
}

void drawRocket(float x,float y,float z,Col3 c,float sc=1.f){
    glPushMatrix();glTranslatef(x,y,z);
    glPointSize(12.f*sc);glColor3f(c.r*1.5f,c.g*1.5f,c.b*1.5f);
    glBegin(GL_POINTS);glVertex3f(0,0,0);glEnd();
    float r0=15.f*sc,h0=50.f*sc;
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
    float t=1.f-b.life/b.maxLife,r=500.f*t,a=b.life/b.maxLife;
    glPointSize(5.f);
    glBegin(GL_POINTS);
    for(int i=0;i<50;++i){
        float th=(float)i/50*2*(float)M_PI,ph=(float)i/50*(float)M_PI;
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

void drawHUD(int hit,int total,bool simDone){
    glMatrixMode(GL_PROJECTION);glPushMatrix();glLoadIdentity();
    glOrtho(0,winW,0,winH,-1,1);
    glMatrixMode(GL_MODELVIEW);glPushMatrix();glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
    glColor4f(0,0.05f,0,0.75f);
    glBegin(GL_QUADS);
    glVertex2f(10,winH-10);glVertex2f(280,winH-10);
    glVertex2f(280,winH-80);glVertex2f(10,winH-80);
    glEnd();
    glColor4f(0.2f,0.9f,0.3f,0.9f);glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(10,winH-10);glVertex2f(280,winH-10);
    glVertex2f(280,winH-80);glVertex2f(10,winH-80);
    glEnd();
    float fill=(total>0)?(float)hit/total:0.f;
    glColor4f(0.1f,0.1f,0.1f,0.8f);
    glBegin(GL_QUADS);
    glVertex2f(20,winH-55);glVertex2f(270,winH-55);
    glVertex2f(270,winH-70);glVertex2f(20,winH-70);
    glEnd();
    glColor4f(0.1f,0.9f,0.2f,1.f);
    glBegin(GL_QUADS);
    glVertex2f(20,winH-55);glVertex2f(20+250*fill,winH-55);
    glVertex2f(20+250*fill,winH-70);glVertex2f(20,winH-70);
    glEnd();
    float sq=18.f,gap=4.f,sy=winH-40.f;
    for(int i=0;i<total;++i){
        float x0=20.f+i*(sq+gap);
        glColor4f(i<hit?0.1f:0.9f,i<hit?0.9f:0.2f,0.1f,1.f);
        glBegin(GL_QUADS);
        glVertex2f(x0,sy);glVertex2f(x0+sq,sy);
        glVertex2f(x0+sq,sy+sq);glVertex2f(x0,sy+sq);
        glEnd();
    }
    if(simDone){
        glColor4f(0.2f,1.f,0.3f,0.95f);
        glBegin(GL_QUADS);
        glVertex2f(winW/2-200,winH/2-30);glVertex2f(winW/2+200,winH/2-30);
        glVertex2f(winW/2+200,winH/2+30);glVertex2f(winW/2-200,winH/2+30);
        glEnd();
    }
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);glPopMatrix();
    glMatrixMode(GL_MODELVIEW);glPopMatrix();
}

bool inCell(float x,float y,Cell* cell){
    auto c=cell->getCorners();
    return x>=c[0]->getX()&&x<=c[2]->getX()&&y>=c[0]->getY()&&y<=c[2]->getY();
}

Map* makeMap(RNG& rng,float z0){
    Corner* cm1=new Corner(0,0,z0),*cm2=new Corner(WORLD_W,0,z0);
    Corner* cm3=new Corner(WORLD_W,WORLD_H,z0),*cm4=new Corner(0,WORLD_H,z0);
    std::vector<Cell*> cells;
    std::vector<std::pair<float,float>> pos;
    for(int tr=0;(int)cells.size()<N_CELLS&&tr<50000;++tr){
        float cx=rng.f(CELL_SZ/2,WORLD_W-CELL_SZ/2);
        float cy=rng.f(CELL_SZ/2,WORLD_H-CELL_SZ/2);
        bool bad=false;
        for(auto& p:pos){float dx=cx-p.first,dy=cy-p.second;if(sqrtf(dx*dx+dy*dy)<MIN_DIST){bad=true;break;}}
        if(bad)continue;
        pos.push_back({cx,cy});
        float h=CELL_SZ/2;
        Corner* c1=new Corner(cx-h,cy-h,z0),*c2=new Corner(cx+h,cy-h,z0);
        Corner* c3=new Corner(cx+h,cy+h,z0),*c4=new Corner(cx-h,cy+h,z0);
        Parametrs* p=new Parametrs(0,0,0,0,0,0,cx,cy,z0);
        DefenderRocket* def=new DefenderRocket(DEF_MASS,DEF_FUEL,nullptr,p,DEF_F);
        std::vector<float> pv=p->getParametrs();
        pv[6]=cx;pv[7]=cy;pv[8]=z0;
        p->setParametrs(pv);
        cells.push_back(new Cell(c1,c2,c3,c4,def));
    }
    return new Map(cm1,cm2,cm3,cm4,cells);
}

AtackRocket* makeAtk(RNG& rng,Map* map,int nc,float tx,float ty){
    float x,y;bool inside=true;
    for(int t=0;t<10000&&inside;++t){
        x=rng.f(0,WORLD_W);y=rng.f(0,WORLD_H);inside=false;
        for(int i=0;i<nc;++i)if(inCell(x,y,map->getCellNumber(i))){inside=true;break;}
    }
    return new AtackRocket(100,x,y,0.f,tx,ty,0.f,0.f,0,0,0);
}

int main(){
    if(!glfwInit()){std::cerr<<"GLFW fail\n";return -1;}
    glfwWindowHint(GLFW_SAMPLES,4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,1);
    win=glfwCreateWindow(winW,winH,"Rocket Defense — 15x15km",nullptr,nullptr);
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
    Map* gmap=makeMap(rng,0.f);
    int nCells=0;
    while(nCells<N_CELLS&&gmap->getCellNumber(nCells))++nCells;

    std::vector<Pt3> cellC(nCells);
    for(int i=0;i<nCells;++i){
        Corner* c=gmap->getCellNumber(i)->getCenter();
        cellC[i]={c->getX(),c->getY(),c->getZ()};delete c;
    }

    std::vector<AtackRocket*> atk;
    for(int i=0;i<N_ATK;++i){
        int t=rng.i(0,nCells-1);
        atk.push_back(makeAtk(rng,gmap,nCells,cellC[t].x,cellC[t].y));
    }

    std::vector<AtackRocket*> defTgt(nCells);
    for(int i=0;i<nCells;++i){
        float best=1e18f;int bi=0;
        for(int j=0;j<(int)atk.size();++j){
            float dx=atk[j]->getX()-cellC[i].x,dy=atk[j]->getY()-cellC[i].y;
            float d=dx*dx+dy*dy;
            if(d<best){best=d;bi=j;}
        }
        defTgt[i]=atk[bi];
    }

    for(auto* a:atk)a->launch();
    std::vector<bool> defReady(nCells,false);

    atkTrail.resize(N_ATK);
    defTrail.resize(nCells);

    const float MAX_T=600.f;
    float simT=0.f;
    const int LOG_STEP=5;
    int stepN=0;

    Col3 cAtk={1.f,0.25f,0.1f},cDef={0.1f,0.4f,1.f},cExp={1.f,0.7f,0.1f};

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
                if(stepN%LOG_STEP==0)
                    log_data.push_back({simT,a->getX(),a->getY(),a->getZ(),i,true});
                a->update(DT);
            }

            for(int i=0;i<nCells;++i){
                if(defReady[i])continue;
                AtackRocket* tgt=defTgt[i];
                if(!tgt->isActive()){defReady[i]=true;continue;}
                float dx=tgt->getX()-cellC[i].x,dy=tgt->getY()-cellC[i].y;
                if(sqrtf(dx*dx+dy*dy)<=DETECT_DIST){
                    gmap->getCellNumber(i)->getDefenderRocket()->launch(tgt);
                    defReady[i]=true;
                }
            }

            for(int i=0;i<nCells;++i){
                DefenderRocket* d=gmap->getCellNumber(i)->getDefenderRocket();
                if(!d->isActive())continue;
                alive=true;
                bool wasDead=defTgt[i]->isDestroyed();
                auto& tr=defTrail[i];
                tr.push_back({d->getX(),d->getY(),d->getZ()});
                if((int)tr.size()>TRAIL_LEN)tr.erase(tr.begin());
                if(stepN%LOG_STEP==0)
                    log_data.push_back({simT,d->getX(),d->getY(),d->getZ(),i,false});
                d->update(DT,defTgt[i]);
                if(!wasDead&&defTgt[i]->isDestroyed()){
                    booms.push_back({defTgt[i]->getX(),defTgt[i]->getY(),defTgt[i]->getZ(),1.5f,1.5f,cExp});
                    ++nHit;
                }
            }

            for(auto& b:booms)b.life-=DT;
            booms.erase(std::remove_if(booms.begin(),booms.end(),[](const Boom& b){return b.life<=0.f;}),booms.end());

            simT+=DT;
            ++stepN;
            if(!alive){
                done=true;
                std::ofstream f("trajectories.csv");
                f<<"t,x,y,z,id,type\n";
                for(auto& p:log_data)
                    f<<p.t<<","<<p.x<<","<<p.y<<","<<p.z<<","
                     <<p.id<<","<<(p.isAtk?"atk":"def")<<"\n";
                std::cout<<"Saved trajectories.csv ("<<log_data.size()<<" points)\n";
            }
        }

        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        setCamera();
        drawGround();
        for(int i=0;i<nCells;++i)drawCell(cellC[i].x,cellC[i].y,CELL_SZ);
        for(int i=0;i<N_ATK;++i){
            drawTrail(atkTrail[i],cAtk);
            if(atk[i]->isActive())drawRocket(atk[i]->getX(),atk[i]->getY(),atk[i]->getZ(),cAtk,1.8f);
        }
        for(int i=0;i<nCells;++i){
            drawTrail(defTrail[i],cDef);
            DefenderRocket* d=gmap->getCellNumber(i)->getDefenderRocket();
            if(d->isActive())drawRocket(d->getX(),d->getY(),d->getZ(),cDef,1.f);
        }
        for(auto& b:booms)drawBoom(b);
        drawHUD(nHit,N_ATK,done);
        glfwSwapBuffers(win);
    }

    for(auto* a:atk)delete a;
    delete gmap;
    glfwDestroyWindow(win);glfwTerminate();
    return 0;
}