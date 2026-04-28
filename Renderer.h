#ifndef RENDERER_H
#define RENDERER_H

#include <GLFW/glfw3.h>
#include <vector>
#include <deque>

class AtackRocket;
class DefenderRocket;
class Map;

struct TrajectoryPoint {
    float x, y, z;
    float r, g, b;
};

class Renderer {
private:
    GLFWwindow* window;
    int width, height;
    
    // Камера
    float cameraAngleX, cameraAngleY;
    float cameraDistance;
    float cameraTargetX, cameraTargetY, cameraTargetZ;
    
    // Траектории
    std::deque<TrajectoryPoint> attackerTrajectory;
    std::vector<std::deque<TrajectoryPoint>> defenderTrajectories;
    int maxTrajectoryPoints;
    
    // Управление мышью
    double lastMouseX, lastMouseY;
    bool mousePressed;
    
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    
public:
    Renderer(int width = 1280, int height = 720);
    ~Renderer();
    
    bool init();
    bool shouldClose();
    void clear();
    void swapBuffers();
    void pollEvents();
    
    void setupCamera();
    void drawGrid(float size, int divisions);
    void drawMap(Map* map, int numCells);
    void drawCell(float x1, float y1, float x2, float y2, float z, bool hasDefender);
    void drawRocket(float x, float y, float z, float r, float g, float b, float scale = 1.0f);
    void drawAttacker(AtackRocket* attacker);
    void drawDefender(DefenderRocket* defender, int id);
    void drawTrajectories();
    void drawAxis();
    void drawText(float x, float y, const char* text);
    
    void addAttackerPoint(float x, float y, float z);
    void addDefenderPoint(int id, float x, float y, float z);
    void clearTrajectories();
    
    void updateCamera(float dx, float dy, float dz);
    void setCameraTarget(float x, float y, float z);
};

#endif
