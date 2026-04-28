// ==============================================================
//  Rocket Defense Simulation — OpenGL визуализация
//  Компиляция:
//    g++ -std=c++17 main_opengl.cpp AtackRocket.cpp DefenderRocket.cpp
//        map.cpp Parametrs.cpp -o sim_gl -lGL -lGLU -lglfw -lm
// ==============================================================
#include "AtackRocket.h"
#include "DefenderRocket.h"
#include "map.h"
#include "Parametrs.h"
#include "Vec.cpp"

#include <GLFW/glfw3.h>
#ifdef __APPLE__
  #include <OpenGL/glu.h>
#else
  #include <GL/glu.h>
#endif

#include <random>
#include <ctime>
#include <cmath>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

// ==============================================================
//  Параметры симуляции
// ==============================================================
static const float MAP_WIDTH    = 5000.0f;
static const float MAP_HEIGHT   = 5000.0f;
static const int   NUM_CELLS    = 10;
static const float CELL_SIZE    = 150.0f;
static const int   NUM_ATTACKERS= NUM_CELLS;
static const float START_Z      = 0.0f;
static const float MIN_CELL_DIST= 300.0f;

// ==============================================================
//  RandomGenerator
// ==============================================================
class RandomGenerator {
    std::mt19937 gen;
public:
    RandomGenerator() : gen(static_cast<unsigned int>(time(nullptr))) {}
    float getFloat(float a, float b) {
        return std::uniform_real_distribution<float>(a,b)(gen);
    }
    int getInt(int a, int b) {
        return std::uniform_int_distribution<int>(a,b)(gen);
    }
};

// ==============================================================
//  Вспомогательные структуры
// ==============================================================
struct Color3 { float r,g,b; };

struct Explosion {
    float x,y,z;
    float life;      // 0..1, убывает
    float maxLife;
    Color3 col;
};

struct TrailPoint { float x,y,z; };

// ==============================================================
//  Глобальное состояние сцены
// ==============================================================
static GLFWwindow* g_win = nullptr;
static int  g_winW = 1280, g_winH = 720;

// Камера — орбита вокруг центра карты
static float g_camAngleX  = 40.0f;   // вокруг Z
static float g_camAngleY  = 35.0f;   // подъём
static float g_camDist    = 9000.0f;
static float g_camCX      = MAP_WIDTH/2;
static float g_camCY      = MAP_HEIGHT/2;
static float g_camCZ      = 0.0f;
static double g_mx=0,g_my=0;
static bool   g_mouseDown = false;

// Траектории
static std::vector<std::vector<TrailPoint>> g_atkTrails;
static std::vector<std::vector<TrailPoint>> g_defTrails;
static const int TRAIL_LEN = 120;

// Взрывы
static std::vector<Explosion> g_explosions;

// Статистика
static int g_intercepted = 0;
static int g_total       = NUM_ATTACKERS;
static bool g_simDone    = false;

// ==============================================================
//  Мышь — коллбэки
// ==============================================================
static void cbMouseBtn(GLFWwindow*, int btn, int act, int) {
    if (btn==GLFW_MOUSE_BUTTON_LEFT) g_mouseDown = (act==GLFW_PRESS);
}
static void cbMouseMove(GLFWwindow*, double x, double y) {
    if (g_mouseDown) {
        g_camAngleX += (float)(x-g_mx)*0.4f;
        g_camAngleY -= (float)(y-g_my)*0.4f;
        g_camAngleY  = std::max(-89.f, std::min(89.f, g_camAngleY));
    }
    g_mx=x; g_my=y;
}
static void cbScroll(GLFWwindow*, double, double dy) {
    g_camDist -= (float)dy*300.f;
    g_camDist  = std::max(500.f, std::min(20000.f, g_camDist));
}
static void cbKey(GLFWwindow* w, int k, int, int act, int) {
    if (k==GLFW_KEY_ESCAPE && act==GLFW_PRESS) glfwSetWindowShouldClose(w,1);
}

// ==============================================================
//  Утилиты рисования
// ==============================================================
static void setCamera() {
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    gluPerspective(45.0, (double)g_winW/g_winH, 10.0, 60000.0);
    glMatrixMode(GL_MODELVIEW);  glLoadIdentity();
    float rad_x = g_camAngleX * (float)M_PI/180.f;
    float rad_y = g_camAngleY * (float)M_PI/180.f;
    float cx = g_camCX + g_camDist * cosf(rad_y)*cosf(rad_x);
    float cy = g_camCY + g_camDist * cosf(rad_y)*sinf(rad_x);
    float cz = g_camCZ + g_camDist * sinf(rad_y);
    gluLookAt(cx,cy,cz, g_camCX,g_camCY,g_camCZ, 0,0,1);
}

// Сетка земли
static void drawGround() {
    int steps = 20;
    float step_x = MAP_WIDTH  / steps;
    float step_y = MAP_HEIGHT / steps;

    // заливка
    glColor4f(0.08f,0.12f,0.08f,1.f);
    glBegin(GL_QUADS);
    glVertex3f(0,0,0); glVertex3f(MAP_WIDTH,0,0);
    glVertex3f(MAP_WIDTH,MAP_HEIGHT,0); glVertex3f(0,MAP_HEIGHT,0);
    glEnd();

    // сетка
    glLineWidth(1.f);
    glColor4f(0.2f,0.35f,0.2f,0.6f);
    glBegin(GL_LINES);
    for (int i=0; i<=steps; ++i) {
        glVertex3f(i*step_x, 0, 1);          glVertex3f(i*step_x, MAP_HEIGHT, 1);
        glVertex3f(0, i*step_y, 1);          glVertex3f(MAP_WIDTH, i*step_y, 1);
    }
    glEnd();
}

// Ячейка с дефендером
static void drawCell(float cx, float cy, float sz) {
    float h = sz/2;
    float z = 2.f;

    // заливка — зелёный квадрат
    glColor4f(0.1f,0.6f,0.2f,0.5f);
    glBegin(GL_QUADS);
    glVertex3f(cx-h,cy-h,z); glVertex3f(cx+h,cy-h,z);
    glVertex3f(cx+h,cy+h,z); glVertex3f(cx-h,cy+h,z);
    glEnd();

    // рамка
    glLineWidth(2.f);
    glColor3f(0.2f,1.f,0.3f);
    glBegin(GL_LINE_LOOP);
    glVertex3f(cx-h,cy-h,z); glVertex3f(cx+h,cy-h,z);
    glVertex3f(cx+h,cy+h,z); glVertex3f(cx-h,cy+h,z);
    glEnd();

    // центральная точка
    glPointSize(6.f);
    glColor3f(0.5f,1.f,0.5f);
    glBegin(GL_POINTS); glVertex3f(cx,cy,z+1); glEnd();
}

// Конус-ракета
static void drawRocket(float x, float y, float z, Color3 col, float scale=1.f) {
    glPushMatrix();
    glTranslatef(x,y,z);

    // светящаяся точка
    glPointSize(10.f*scale);
    glColor3f(col.r*1.5f, col.g*1.5f, col.b*1.5f);
    glBegin(GL_POINTS); glVertex3f(0,0,0); glEnd();

    // конус (8 граней)
    float r0=8.f*scale, h0=25.f*scale;
    glColor3f(col.r, col.g, col.b);
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0,0,h0); // вершина
    for (int i=0; i<=8; ++i) {
        float a = i*(2.f*(float)M_PI/8);
        glVertex3f(r0*cosf(a), r0*sinf(a), 0);
    }
    glEnd();
    // основание
    glColor3f(col.r*0.6f, col.g*0.6f, col.b*0.6f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0,0,0);
    for (int i=0; i<=8; ++i) {
        float a = i*(2.f*(float)M_PI/8);
        glVertex3f(r0*cosf(a), r0*sinf(a), 0);
    }
    glEnd();

    glPopMatrix();
}

// Хвост-трейл
static void drawTrail(const std::vector<TrailPoint>& trail, Color3 col) {
    if (trail.size()<2) return;
    glLineWidth(2.f);
    glBegin(GL_LINE_STRIP);
    for (int i=0; i<(int)trail.size(); ++i) {
        float alpha = (float)i / trail.size() * 0.7f;
        glColor4f(col.r, col.g, col.b, alpha);
        glVertex3f(trail[i].x, trail[i].y, trail[i].z);
    }
    glEnd();
}

// Взрыв — сфера из точек
static void drawExplosion(const Explosion& e) {
    float t = 1.f - e.life/e.maxLife; // 0→1
    float radius = 200.f * t;
    float alpha  = e.life/e.maxLife;

    glPointSize(4.f);
    glBegin(GL_POINTS);
    // 40 точек по сфере
    for (int i=0; i<40; ++i) {
        float theta = (float)i / 40 * 2*(float)M_PI;
        float phi   = (float)i / 40 * (float)M_PI;
        float px = e.x + radius*sinf(phi)*cosf(theta);
        float py = e.y + radius*sinf(phi)*sinf(theta);
        float pz = e.z + radius*cosf(phi);
        glColor4f(e.col.r, e.col.g, e.col.b, alpha);
        glVertex3f(px,py,pz);
    }
    glEnd();

    // кольцо
    glLineWidth(2.f);
    glBegin(GL_LINE_LOOP);
    for (int i=0; i<32; ++i) {
        float a = (float)i/32*2*(float)M_PI;
        glColor4f(1.f,0.8f,0.2f, alpha*0.8f);
        glVertex3f(e.x+radius*cosf(a), e.y+radius*sinf(a), e.z);
    }
    glEnd();
}

// Простой HUD-текст через строки точек (растровые символы GLUT-style не нужны)
// Рисуем прямоугольную панель в 2D
static void drawHUD(int intercepted, int total, bool done) {
    glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
    glOrtho(0,g_winW,0,g_winH,-1,1);
    glMatrixMode(GL_MODELVIEW);  glPushMatrix(); glLoadIdentity();
    glDisable(GL_DEPTH_TEST);

    // фон панели
    glColor4f(0.0f,0.05f,0.0f,0.75f);
    glBegin(GL_QUADS);
    glVertex2f(10,g_winH-10); glVertex2f(280,g_winH-10);
    glVertex2f(280,g_winH-80); glVertex2f(10,g_winH-80);
    glEnd();

    // рамка
    glColor4f(0.2f,0.9f,0.3f,0.9f);
    glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(10,g_winH-10); glVertex2f(280,g_winH-10);
    glVertex2f(280,g_winH-80); glVertex2f(10,g_winH-80);
    glEnd();

    // прогресс-бар перехватов
    float fill = (total>0) ? (float)intercepted/total : 0.f;
    glColor4f(0.1f,0.1f,0.1f,0.8f);
    glBegin(GL_QUADS);
    glVertex2f(20,g_winH-55); glVertex2f(270,g_winH-55);
    glVertex2f(270,g_winH-70); glVertex2f(20,g_winH-70);
    glEnd();
    glColor4f(0.1f,0.9f,0.2f,1.f);
    glBegin(GL_QUADS);
    glVertex2f(20,g_winH-55); glVertex2f(20+250*fill,g_winH-55);
    glVertex2f(20+250*fill,g_winH-70); glVertex2f(20,g_winH-70);
    glEnd();

    // точки-индикаторы ракет (по одному квадрату на каждый атакер)
    float sq = 18.f, gap=4.f;
    float startX=20.f, startY=g_winH-40.f;
    for (int i=0;i<total;++i) {
        float x0=startX+i*(sq+gap);
        if (i<intercepted)
            glColor4f(0.1f,0.9f,0.2f,1.f);  // сбит — зелёный
        else
            glColor4f(0.9f,0.2f,0.1f,1.f);  // активен — красный
        glBegin(GL_QUADS);
        glVertex2f(x0,startY); glVertex2f(x0+sq,startY);
        glVertex2f(x0+sq,startY+sq); glVertex2f(x0,startY+sq);
        glEnd();
    }

    if (done) {
        // крупная надпись "SIMULATION COMPLETE"
        glColor4f(0.2f,1.f,0.3f,0.95f);
        glBegin(GL_QUADS);
        glVertex2f(g_winW/2-200, g_winH/2-30);
        glVertex2f(g_winW/2+200, g_winH/2-30);
        glVertex2f(g_winW/2+200, g_winH/2+30);
        glVertex2f(g_winW/2-200, g_winH/2+30);
        glEnd();
        glColor4f(0,0.05f,0,1.f);
        // линии "DONE"
        glLineWidth(3.f);
        // Х
        glBegin(GL_LINES);
        glVertex2f(g_winW/2-180,g_winH/2-15); glVertex2f(g_winW/2-155,g_winH/2+15);
        glVertex2f(g_winW/2-155,g_winH/2-15); glVertex2f(g_winW/2-180,g_winH/2+15);
        glEnd();
    }

    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION); glPopMatrix();
    glMatrixMode(GL_MODELVIEW);  glPopMatrix();
}

// ==============================================================
//  Логика карты
// ==============================================================
static bool isPointInCell(float x, float y, Cell* cell) {
    auto corners = cell->getCorners();
    float minX=corners[0]->getX(), maxX=corners[2]->getX();
    float minY=corners[0]->getY(), maxY=corners[2]->getY();
    return (x>=minX&&x<=maxX&&y>=minY&&y<=maxY);
}

static Map* createRandomMap(RandomGenerator& rng, float baseZ) {
    Corner* cm1=new Corner(0,0,baseZ);
    Corner* cm2=new Corner(MAP_WIDTH,0,baseZ);
    Corner* cm3=new Corner(MAP_WIDTH,MAP_HEIGHT,baseZ);
    Corner* cm4=new Corner(0,MAP_HEIGHT,baseZ);

    std::vector<Cell*> cells;
    std::vector<std::pair<float,float>> placed;
    int attempts=0;
    while ((int)cells.size()<NUM_CELLS && attempts<50000) {
        ++attempts;
        float cx=rng.getFloat(CELL_SIZE/2, MAP_WIDTH -CELL_SIZE/2);
        float cy=rng.getFloat(CELL_SIZE/2, MAP_HEIGHT-CELL_SIZE/2);
        bool bad=false;
        for (auto& p:placed) {
            float dx=cx-p.first, dy=cy-p.second;
            if (sqrtf(dx*dx+dy*dy)<MIN_CELL_DIST){bad=true;break;}
        }
        if (bad) continue;
        placed.push_back({cx,cy});
        float h=CELL_SIZE/2;
        Corner* c1=new Corner(cx-h,cy-h,baseZ);
        Corner* c2=new Corner(cx+h,cy-h,baseZ);
        Corner* c3=new Corner(cx+h,cy+h,baseZ);
        Corner* c4=new Corner(cx-h,cy+h,baseZ);
        Parametrs* p=new Parametrs(0,0,0,0,0,4000,cx,cy,baseZ);
        DefenderRocket* def=new DefenderRocket(10,  1000, 5000, nullptr, p, 800000.0f);
        std::vector<float> pv=p->getParametrs();
        pv[6]=cx; pv[7]=cy; pv[8]=baseZ;
        p->setParametrs(pv);
        cells.push_back(new Cell(c1,c2,c3,c4,def));
    }
    return new Map(cm1,cm2,cm3,cm4,cells);
}

static AtackRocket* createAtacker(RandomGenerator& rng, Map* map, int nc,
                                   float tx,float ty,float tz) {
    float x,y;
    bool inside=true;
    for (int att=0; att<10000&&inside; ++att) {
        x=rng.getFloat(0,MAP_WIDTH);
        y=rng.getFloat(0,MAP_HEIGHT);
        inside=false;
        for (int i=0;i<nc;++i)
            if (isPointInCell(x,y,map->getCellNumber(i))){inside=true;break;}
    }
    return new AtackRocket(100,1500,250, x,y,START_Z, tx,ty,tz,
                           45.f,30,500,2.5,6000,0.05f,0.3f);
}

// ==============================================================
//  main
// ==============================================================
int main() {
    // --- GLFW ---
    if (!glfwInit()) { std::cerr<<"GLFW init failed\n"; return -1; }
    glfwWindowHint(GLFW_SAMPLES,4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,1);
    g_win=glfwCreateWindow(g_winW,g_winH,"Rocket Defense Simulation",nullptr,nullptr);
    if (!g_win){glfwTerminate();return -1;}
    glfwMakeContextCurrent(g_win);
    glfwSwapInterval(1);
    glfwSetMouseButtonCallback(g_win,cbMouseBtn);
    glfwSetCursorPosCallback(g_win,cbMouseMove);
    glfwSetScrollCallback(g_win,cbScroll);
    glfwSetKeyCallback(g_win,cbKey);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    glClearColor(0.02f,0.03f,0.05f,1.f);

    // --- Симуляция ---
    RandomGenerator rng;
    Map* gameMap=createRandomMap(rng,0.f);

    int realCells=0;
    while (realCells<NUM_CELLS && gameMap->getCellNumber(realCells)) ++realCells;

    // Собираем позиции ячеек для рисования
    std::vector<std::pair<float,float>> cellPos(realCells);
    for (int i=0;i<realCells;++i){
        Corner* c=gameMap->getCellNumber(i)->getCenter();
        cellPos[i]={c->getX(),c->getY()};
        delete c;
    }

    // Атакеры
    std::vector<AtackRocket*> attackers;
    for (int i=0;i<NUM_ATTACKERS;++i){
        int tc=rng.getInt(0,realCells-1);
        Corner* c=gameMap->getCellNumber(tc)->getCenter();
        AtackRocket* a=createAtacker(rng,gameMap,realCells,c->getX(),c->getY(),c->getZ());
        delete c;
        attackers.push_back(a);
    }

    // Назначение целей — каждый дефендер берёт ближайшего атакера
    std::vector<AtackRocket*> defTargets(realCells);
    for (int i=0; i<realCells; ++i) {
        Corner* dc = gameMap->getCellNumber(i)->getCenter();
        float dx0 = dc->getX(), dy0 = dc->getY();
        delete dc;
        float bestDist = 1e18f;
        int   bestIdx  = 0;
        for (int j=0; j<(int)attackers.size(); ++j) {
            float ddx = attackers[j]->getX() - dx0;
            float ddy = attackers[j]->getY() - dy0;
            float dist = ddx*ddx + ddy*ddy;
            if (dist < bestDist) { bestDist=dist; bestIdx=j; }
        }
        defTargets[i] = attackers[bestIdx];
    }

    // Запуск
    for (auto* a:attackers) a->launch();
    for (int i=0;i<realCells;++i)
        gameMap->getCellNumber(i)->getDefenderRocket()->launch(defTargets[i]);

    // Трейлы
    g_atkTrails.resize(NUM_ATTACKERS);
    g_defTrails.resize(realCells);

    const float DT=0.016f;   // ~60 fps физики
    const float MAX_SIM=300.f;
    float simTime=0.f;

    Color3 colAtk={1.f,0.25f,0.1f};
    Color3 colDef={0.1f,0.4f,1.f};
    Color3 colExp={1.f,0.7f,0.1f};

    // --- Главный цикл ---
    while (!glfwWindowShouldClose(g_win)) {
        glfwPollEvents();
        glfwGetFramebufferSize(g_win,&g_winW,&g_winH);
        glViewport(0,0,g_winW,g_winH);

        // --- Шаг физики ---
        if (!g_simDone && simTime<MAX_SIM) {
            bool anyActive=false;

            for (int id=0;id<NUM_ATTACKERS;++id){
                AtackRocket* a=attackers[id];
                if (a->isActive()){
                    anyActive=true;
                    // трейл
                    auto& tr=g_atkTrails[id];
                    tr.push_back({a->getX(),a->getY(),a->getZ()});
                    if ((int)tr.size()>TRAIL_LEN) tr.erase(tr.begin());
                    a->update(DT);
                }
            }

            for (int i=0;i<realCells;++i){
                DefenderRocket* d=gameMap->getCellNumber(i)->getDefenderRocket();
                if (d->isActive()){
                    anyActive=true;
                    // взрыв при уничтожении цели — поймаем через isDestroyed
                    bool wasDest=defTargets[i]->isDestroyed();
                    auto& tr=g_defTrails[i];
                    tr.push_back({d->getX(),d->getY(),d->getZ()});
                    if ((int)tr.size()>TRAIL_LEN) tr.erase(tr.begin());
                    d->update(DT,defTargets[i]);
                    // если только что сбили — взрыв
                    if (!wasDest && defTargets[i]->isDestroyed()){
                        g_explosions.push_back({
                            defTargets[i]->getX(),
                            defTargets[i]->getY(),
                            defTargets[i]->getZ(),
                            1.f,1.f, colExp
                        });
                        ++g_intercepted;
                    }
                }
            }

            // обновляем взрывы
            for (auto& e:g_explosions) e.life-=DT;
            g_explosions.erase(
                std::remove_if(g_explosions.begin(),g_explosions.end(),
                    [](const Explosion& e){return e.life<=0.f;}),
                g_explosions.end());

            simTime+=DT;
            if (!anyActive) g_simDone=true;
        }

        // --- Рендер ---
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        setCamera();

        drawGround();

        // Ячейки
        for (int i=0;i<realCells;++i)
            drawCell(cellPos[i].first,cellPos[i].second,CELL_SIZE);

        // Трейлы + ракеты — атакеры
        for (int id=0;id<NUM_ATTACKERS;++id){
            drawTrail(g_atkTrails[id], colAtk);
            AtackRocket* a=attackers[id];
            if (a->isActive())
                drawRocket(a->getX(),a->getY(),a->getZ(), colAtk, 1.4f);
        }

        // Трейлы + ракеты — дефендеры
        for (int i=0;i<realCells;++i){
            drawTrail(g_defTrails[i], colDef);
            DefenderRocket* d=gameMap->getCellNumber(i)->getDefenderRocket();
            if (d->isActive())
                drawRocket(d->getX(),d->getY(),d->getZ(), colDef, 1.f);
        }

        // Взрывы
        for (auto& e:g_explosions) drawExplosion(e);

        // HUD
        drawHUD(g_intercepted, g_total, g_simDone);

        glfwSwapBuffers(g_win);
    }

    for (auto* a:attackers) delete a;
    delete gameMap;
    glfwDestroyWindow(g_win);
    glfwTerminate();
    return 0;
}
