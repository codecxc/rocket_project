#include "AtackRocket.h"
#include "DefenderRocket.h"
#include "map.h"
#include "Parametrs.h"
#include "Vec.cpp"
#include <GLFW/glfw3.h>
#include <GL/glu.h>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <fstream>

const float W=15000.f;
const float H=15000.f;
const int NC=10;
const float DT=0.02f;
const float GAP=800.f;
const float DET=3000.f;
const float DM=100.f;
const float DF=300.f;
const float DT2=80000.f;

float rnd(float a,float b){return a+(b-a)*(float)rand()/RAND_MAX;}
int rndI(int a,int b){return a+rand()%(b-a+1);}

GLFWwindow* win;
int winW=1280,winH=720;
float camA=35.f;
float camD=22000.f;
double mpx=0,mpy=0;
bool mdn=false;

void onBtn(GLFWwindow*,int b,int a,int){if(b==0)mdn=(a==1);}
void onMove(GLFWwindow*,double x,double y){
    if(mdn){camA+=(float)(x-mpx)*0.5f;}
    mpx=x;mpy=y;
}
void onScroll(GLFWwindow*,double,double d) {
    camD-=d*600.f;
    if(camD<2000)camD=2000;
    if(camD>50000)camD=50000;
}
// esc
void onKey(GLFWwindow* w,int k,int,int a,int){
    if(k==GLFW_KEY_ESCAPE&&a==1)glfwSetWindowShouldClose(w,1);
}

void setCamera(){
    glMatrixMode(GL_PROJECTION);glLoadIdentity();
    gluPerspective(50.0,winW/winH,10.0,200000.0);
    glMatrixMode(GL_MODELVIEW);glLoadIdentity();
    float a=camA*3.14159f/180.f;
    float elev=30.f*3.14159f/180.f;
    float ex=W/2+camD*cosf(elev)*cosf(a);
    float ey=H/2+camD*cosf(elev)*sinf(a);
    float ez=4000.f+camD*sinf(elev);
    gluLookAt(ex,ey,ez,W/2,H/2,1000.f,0,0,1);
}

// след ракеты
struct Pt{float x,y,z;};
std::vector<Pt> atr[10],dtr[10];

// добавить точку в буффер следа
void addPt(std::vector<Pt>& t,float x,float y,float z){
    Pt p;p.x=x;p.y=y;p.z=z;
    t.push_back(p);
    if((int)t.size()>150)t.erase(t.begin());
}
// рисование следа
void drawTrail(std::vector<Pt>& t,float r,float g,float b){
    if(t.size()<2)return;
    glBegin(GL_LINE_STRIP);
    for(int i=0;i<(int)t.size();i++){
        float a=(float)i/(float)t.size();
        glColor4f(r,g,b,a*0.8f);
        glVertex3f(t[i].x,t[i].y,t[i].z);
    }
    glEnd();
}

void drawRocket(float x,float y,float z,float r,float g,float b,float sz){
    glColor3f(r,g,b);
    glPointSize(sz);
    glBegin(GL_POINTS);glVertex3f(x,y,z);glEnd();
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    glVertex3f(x-sz*3,y,z);glVertex3f(x+sz*3,y,z);
    glVertex3f(x,y-sz*3,z);glVertex3f(x,y+sz*3,z);
    glVertex3f(x,y,z-sz*3);glVertex3f(x,y,z+sz*3);
    glEnd();
}

void drawCell(float cx,float cy){
    float h=150.f;
    glColor4f(0.2f,1.f,0.3f,0.6f);
    glLineWidth(2.f);
    glBegin(GL_LINE_LOOP);
    glVertex3f(cx-h,cy-h,3);glVertex3f(cx+h,cy-h,3);
    glVertex3f(cx+h,cy+h,3);glVertex3f(cx-h,cy+h,3);
    glEnd();
    glColor3f(0.4f,1.f,0.4f);
    glPointSize(6.f);
    glBegin(GL_POINTS);glVertex3f(cx,cy,3);glEnd();
}

void drawGround(){
    glColor3f(0.08f,0.16f,0.08f);
    glBegin(GL_QUADS);
    glVertex3f(0,0,0);glVertex3f(W,0,0);
    glVertex3f(W,H,0);glVertex3f(0,H,0);
    glEnd();
}

bool inCell(float x,float y,Cell* c){
    auto v=c->getCorners();
    return x>=v[0]->getX()&&x<=v[2]->getX()&&y>=v[0]->getY()&&y<=v[2]->getY();
}

Map* makeMap(){
    std::vector<Cell*> cells;
    float cx[10],cy[10];int n=0;
    for(int tr=0;n<NC&&tr<50000;tr++){
        float x=rnd(200,W-200),y=rnd(200,H-200);
        bool bad=false;
        for(int i=0;i<n;i++){
            float dx=x-cx[i],dy=y-cy[i];
            if(sqrtf(dx*dx+dy*dy)<GAP){bad=true;break;}
        }
        if(bad)continue;
        cx[n]=x;cy[n]=y;n++;
        float h=150.f;
        Corner* c1=new Corner(x-h,y-h,0);Corner* c2=new Corner(x+h,y-h,0);
        Corner* c3=new Corner(x+h,y+h,0);Corner* c4=new Corner(x-h,y+h,0);
        Parametrs* p=new Parametrs(0,0,0,0,0,0,x,y,0);
        DefenderRocket* def=new DefenderRocket(DM,DF,nullptr,p,DT2);
        std::vector<float> pv=p->getParametrs();
        pv[6]=x;pv[7]=y;pv[8]=0;p->setParametrs(pv);
        cells.push_back(new Cell(c1,c2,c3,c4,def));
    }
    return new Map(
        new Corner(0,0,0),new Corner(W,0,0),
        new Corner(W,H,0),new Corner(0,H,0),cells);
}

int main(){
    srand((unsigned int)time(nullptr));
    if(!glfwInit())return 1;
    glfwWindowHint(GLFW_SAMPLES,4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,1);
    win=glfwCreateWindow(winW,winH,"Rockets",nullptr,nullptr);
    glfwMakeContextCurrent(win);glfwSwapInterval(1);
    glfwSetMouseButtonCallback(win,onBtn);
    glfwSetCursorPosCallback(win,onMove);
    glfwSetScrollCallback(win,onScroll);
    glfwSetKeyCallback(win,onKey);
    glEnable(GL_DEPTH_TEST);glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_POINT_SMOOTH);glEnable(GL_LINE_SMOOTH);
    glClearColor(0.02f,0.04f,0.02f,1.f);

    Map* gmap=makeMap();
    int nc=0;
    while(nc<NC&&gmap->getCellNumber(nc))nc++;
	// центры ячеек
    float cx[10],cy[10],cz[10];
    for(int i=0;i<nc;i++){
        Corner* c=gmap->getCellNumber(i)->getCenter();
        cx[i]=c->getX();cy[i]=c->getY();cz[i]=c->getZ();delete c;
    }
	// создаем атакующие
    AtackRocket* atk[10];
    for(int i=0;i<NC;i++){
        int t=rndI(0,nc-1);
        float x,y;bool inside=true;
        for(int tr=0;inside&&tr<10000;tr++){
            x=rnd(0,W);y=rnd(0,H);inside=false;
            for(int j=0;j<nc;j++)if(inCell(x,y,gmap->getCellNumber(j))){inside=true;break;}
        }
        atk[i]=new AtackRocket(100,x,y,0,cx[t],cy[t],0,0,0,0,0);
    }
	// привязка дефендеров
    AtackRocket* tgt[10];
    for(int i=0;i<nc;i++){
        float best=1e18f;int bi=0;
        for(int j=0;j<NC;j++){
            float dx=atk[j]->getX()-cx[i],dy=atk[j]->getY()-cy[i];
            float d=dx*dx+dy*dy;
            if(d<best){best=d;bi=j;}
        }
        tgt[i]=atk[bi];
    }

    for(int i=0;i<NC;i++)atk[i]->launch();
    bool defGo[10]={};
    int nHit=0;bool done=false;float simT=0;

    std::vector<std::string> logLines;
    logLines.push_back("t,x,y,z,id,type");

    while(!glfwWindowShouldClose(win)){
        glfwPollEvents();
        glfwGetFramebufferSize(win,&winW,&winH);
        glViewport(0,0,winW,winH);

        if(!done&&simT<600){
            bool alive=false;
		// обновление атакера
            for(int i=0;i<NC;i++){
                if(!atk[i]->isActive())continue;
                alive=true;
                addPt(atr[i],atk[i]->getX(),atk[i]->getY(),atk[i]->getZ());
                if((int)(simT/DT)%10==0){
                    char buf[128];
                    snprintf(buf,128,"%.2f,%.1f,%.1f,%.1f,%d,atk",
                        simT,atk[i]->getX(),atk[i]->getY(),atk[i]->getZ(),i);
                    logLines.push_back(buf);
                }
                atk[i]->update(DT);
            }
		// надо ли пускать дефендера
            for(int i=0;i<nc;i++){
                if(defGo[i])continue;
                if(!tgt[i]->isActive()){defGo[i]=true;continue;}
                float dx=tgt[i]->getX()-cx[i],dy=tgt[i]->getY()-cy[i];
                if(sqrtf(dx*dx+dy*dy)<=DET){
                    gmap->getCellNumber(i)->getDefenderRocket()->launch(tgt[i]);
                    defGo[i]=true;
                }
            }
		// запуск дефендера
            for(int i=0;i<nc;i++){
                DefenderRocket* d=gmap->getCellNumber(i)->getDefenderRocket();
                if(!d->isActive())continue;
                alive=true;
                addPt(dtr[i],d->getX(),d->getY(),d->getZ());
                bool was=tgt[i]->isDestroyed();
                d->update(DT,tgt[i]);
                if(!was&&tgt[i]->isDestroyed())nHit++;
                if((int)(simT/DT)%10==0){
                    char buf[128];
                    snprintf(buf,128,"%.2f,%.1f,%.1f,%.1f,%d,def",
                        simT,d->getX(),d->getY(),d->getZ(),i);
                    logLines.push_back(buf);
                }
            }

            simT+=DT;
            if(!alive){
                done=true;
                std::ofstream f("trajectories.csv");
                for(auto& s:logLines)f<<s<<"\n";
                std::cout<<"Сбито "<<nHit<<"/"<<NC<<"\n";
            }
        }

        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        setCamera();
        drawGround();
        for(int i=0;i<nc;i++)drawCell(cx[i],cy[i]);
        for(int i=0;i<NC;i++){
            drawTrail(atr[i],1.f,0.3f,0.1f);
            if(atk[i]->isActive())drawRocket(atk[i]->getX(),atk[i]->getY(),atk[i]->getZ(),1.f,0.3f,0.1f,10.f);
        }
        for(int i=0;i<nc;i++){
            drawTrail(dtr[i],0.2f,0.5f,1.f);
            DefenderRocket* d=gmap->getCellNumber(i)->getDefenderRocket();
            if(d->isActive())drawRocket(d->getX(),d->getY(),d->getZ(),0.2f,0.5f,1.f,7.f);
        }
        glfwSwapBuffers(win);
    }

    for(int i=0;i<NC;i++)delete atk[i];
    delete gmap;
    glfwDestroyWindow(win);glfwTerminate();
    return 0;
}
