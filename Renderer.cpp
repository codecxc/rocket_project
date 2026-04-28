#include "Renderer.h"
#include "AtackRocket.h"
#include "DefenderRocket.h"
#include "map.h"
#include <iostream>
#include <cmath>

#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

Renderer::Renderer(int width, int height) 
    : window(nullptr), width(width), height(height),
      cameraAngleX(45.0f), cameraAngleY(30.0f), cameraDistance(8000.0f),
      cameraTargetX(2500.0f), cameraTargetY(2500.0f), cameraTargetZ(500.0f),
      maxTrajectoryPoints(1000), mousePressed(false),
      lastMouseX(0.0), lastMouseY(0.0) {
    defenderTrajectories.resize(20);
}

Renderer::~Renderer() {
    if (window) {
        glfwDestroyWindow(window);
        glfwTerminate();
    }
}

bool Renderer::init() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }
    
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    
    window = glfwCreateWindow(width, height, "Rocket Defense Simulation", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    
    glfwMakeContextCurrent(window);
    glfwSetWindowUserPointer(window, this);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSwapInterval(1);
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    
    return true;
}

bool Renderer::shouldClose() {
    return glfwWindowShouldClose(window);
}

void Renderer::clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::swapBuffers() {
    glfwSwapBuffers(window);
}

void Renderer::pollEvents() {
    glfwPollEvents();
}

void Renderer::setupCamera() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)width / (double)height, 10.0, 50000.0);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    float camX = cameraTargetX + cameraDistance * cos(cameraAngleY * M_PI / 180.0) * cos(cameraAngleX * M_PI / 180.0);
    float camY = cameraTargetY + cameraDistance * cos(cameraAngleY * M_PI / 180.0) * sin(cameraAngleX * M_PI / 180.0);
    float camZ = cameraTargetZ + cameraDistance * sin(cameraAngleY * M_PI / 180.0);
    
    gluLookAt(camX, camY, camZ,
              cameraTargetX, cameraTargetY, cameraTargetZ,
              0.0, 0.0, 1.0);
}

void Renderer::drawGrid(float size, int divisions) {
    glLineWidth(1.0f);
    glColor4f(0.3f, 0.3f, 0.3f, 0.5f);
    
    glBegin(GL_LINES);
    for (int i = 0; i <= divisions; ++i) {
        float pos = (size / divisions) * i;
        
        glVertex3f(pos, 0.0f, 0.0f);
        glVertex3f(pos, size, 0.0f);
        
        glVertex3f(0.0f, pos, 0.0f);
        glVertex3f(size, pos, 0.0f);
    }
    glEnd();
}

void Renderer::drawCell(float x1, float y1, float x2, float y2, float z, bool hasDefender) {
    if (hasDefender) {
        glColor4f(0.2f, 0.6f, 0.2f, 0.7f);
    } else {
        glColor4f(0.5f, 0.5f, 0.5f, 0.3f);
    }
    
    // Нижняя грань
    glBegin(GL_QUADS);
    glVertex3f(x1, y1, z);
    glVertex3f(x2, y1, z);
    glVertex3f(x2, y2, z);
    glVertex3f(x1, y2, z);
    glEnd();
    
    // Рамка
    glLineWidth(2.0f);
    if (hasDefender) {
        glColor4f(0.3f, 0.9f, 0.3f, 1.0f);
    } else {
        glColor4f(0.7f, 0.7f, 0.7f, 0.8f);
    }
    
    glBegin(GL_LINE_LOOP);
    glVertex3f(x1, y1, z);
    glVertex3f(x2, y1, z);
    glVertex3f(x2, y2, z);
    glVertex3f(x1, y2, z);
    glEnd();
}

void Renderer::drawMap(Map* map, int numCells) {
    for (int i = 0; i < numCells; ++i) {
        Cell* cell = map->getCellNumber(i);
        if (cell) {
            std::vector<Corner*> corners = cell->getCorners();
            if (corners.size() >= 4) {
                drawCell(corners[0]->getX(), corners[0]->getY(),
                        corners[2]->getX(), corners[2]->getY(),
                        corners[0]->getZ(),
                        cell->getDefenderRocket() != nullptr);
            }
        }
    }
}

void Renderer::drawRocket(float x, float y, float z, float r, float g, float b, float scale) {
    glPushMatrix();
    glTranslatef(x, y, z);
    
    // Тело ракеты (конус)
    glColor3f(r, g, b);
    GLUquadric* quad = gluNewQuadric();
    glRotatef(-90, 1, 0, 0);
    gluCylinder(quad, 5.0f * scale, 2.0f * scale, 20.0f * scale, 8, 1);
    
    // Наконечник
    glTranslatef(0, 0, 20.0f * scale);
    glColor3f(r * 0.7f, g * 0.7f, b * 0.7f);
    gluCylinder(quad, 2.0f * scale, 0.0f, 10.0f * scale, 8, 1);
    
    gluDeleteQuadric(quad);
    glPopMatrix();
    
    // Светящаяся точка
    glPointSize(8.0f * scale);
    glBegin(GL_POINTS);
    glColor3f(r * 1.5f, g * 1.5f, b * 1.5f);
    glVertex3f(x, y, z);
    glEnd();
}

void Renderer::drawAttacker(AtackRocket* attacker) {
    if (attacker && attacker->isActive()) {
        drawRocket(attacker->getX(), attacker->getY(), attacker->getZ(),
                   1.0f, 0.2f, 0.2f, 1.5f);
    }
}

void Renderer::drawDefender(DefenderRocket* defender, int id) {
    if (defender && defender->isActive()) {
        drawRocket(defender->getX(), defender->getY(), defender->getZ(),
                   0.2f, 0.2f, 1.0f, 1.0f);
    }
}

void Renderer::drawTrajectories() {
    // Траектория атакующей ракеты
    if (attackerTrajectory.size() > 1) {
        glLineWidth(2.0f);
        glBegin(GL_LINE_STRIP);
        for (const auto& point : attackerTrajectory) {
            glColor4f(point.r, point.g, point.b, 0.6f);
            glVertex3f(point.x, point.y, point.z);
        }
        glEnd();
    }
    
    // Траектории защитных ракет
    for (const auto& trajectory : defenderTrajectories) {
        if (trajectory.size() > 1) {
            glLineWidth(2.0f);
            glBegin(GL_LINE_STRIP);
            for (const auto& point : trajectory) {
                glColor4f(point.r, point.g, point.b, 0.6f);
                glVertex3f(point.x, point.y, point.z);
            }
            glEnd();
        }
    }
}

void Renderer::drawAxis() {
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    
    // X - красный
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(500.0f, 0.0f, 0.0f);
    
    // Y - зеленый
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 500.0f, 0.0f);
    
    // Z - синий
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 500.0f);
    
    glEnd();
}

void Renderer::addAttackerPoint(float x, float y, float z) {
    attackerTrajectory.push_back({x, y, z, 1.0f, 0.3f, 0.3f});
    if (attackerTrajectory.size() > maxTrajectoryPoints) {
        attackerTrajectory.pop_front();
    }
}

void Renderer::addDefenderPoint(int id, float x, float y, float z) {
    if (id >= 0 && id < defenderTrajectories.size()) {
        defenderTrajectories[id].push_back({x, y, z, 0.3f, 0.3f, 1.0f});
        if (defenderTrajectories[id].size() > maxTrajectoryPoints) {
            defenderTrajectories[id].pop_front();
        }
    }
}

void Renderer::clearTrajectories() {
    attackerTrajectory.clear();
    for (auto& trajectory : defenderTrajectories) {
        trajectory.clear();
    }
}

void Renderer::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    Renderer* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            renderer->mousePressed = true;
            glfwGetCursorPos(window, &renderer->lastMouseX, &renderer->lastMouseY);
        } else if (action == GLFW_RELEASE) {
            renderer->mousePressed = false;
        }
    }
}

void Renderer::cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    Renderer* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
    if (renderer->mousePressed) {
        double dx = xpos - renderer->lastMouseX;
        double dy = ypos - renderer->lastMouseY;
        
        renderer->cameraAngleX += dx * 0.5f;
        renderer->cameraAngleY -= dy * 0.5f;
        
        if (renderer->cameraAngleY > 89.0f) renderer->cameraAngleY = 89.0f;
        if (renderer->cameraAngleY < -89.0f) renderer->cameraAngleY = -89.0f;
        
        renderer->lastMouseX = xpos;
        renderer->lastMouseY = ypos;
    }
}

void Renderer::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    Renderer* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
    renderer->cameraDistance -= yoffset * 200.0f;
    if (renderer->cameraDistance < 500.0f) renderer->cameraDistance = 500.0f;
    if (renderer->cameraDistance > 20000.0f) renderer->cameraDistance = 20000.0f;
}

void Renderer::setCameraTarget(float x, float y, float z) {
    cameraTargetX = x;
    cameraTargetY = y;
    cameraTargetZ = z;
}
