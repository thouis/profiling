#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <math.h>

#include <vector>

#include "Ball.hpp"

// Global variable to work around glut's display callback not taking arguments
std::vector<Ball> *balls;
int max_steps = -1;


struct Timer {
  CS207::Clock clock;
  std::string msg;

  Timer(const std::string& _msg)
      : msg(_msg) {
    clock.start();
  }

  ~Timer() {
    double elapsed_ms = clock.seconds() * 1000;
    std::cout << msg << ": " << elapsed_ms << " ms" << std::endl;
  }
};



void init()
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
}

void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, 0, 20);
    //glFrustum(-0.1, 0.1, -float(h)/(10.0*float(w)), float(h)/(10.0*float(w)), 0.5, 1000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void draw_circle(const Point &center,
                 float radius,
                 double *color)
{
    glBegin(GL_TRIANGLE_FAN);

    glColor3f(color[0], color[1], color[2]);
    for (int steps = 0; steps < 10; steps++) {
        glVertex3f(center[0] + radius * cos(steps * 2 * M_PI / 10),
                   center[1] + radius * sin(steps * 2 * M_PI / 10),
                   center[2]);
    }
    glEnd();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    {
        Timer timer("step");
        for (auto b1 = balls->begin(); b1 < balls->end(); ++b1) {
            for (auto b2 = b1; b2 < balls->end(); ++b2) {
                if ((b1 != b2) && (b1->near(*b2)))
                    b1->collide(*b2);
            }
        }

        for (auto b1 = balls->begin(); b1 < balls->end(); ++b1) {
            b1->collide_walls();
            b1->step(0.01);
        }

        for (auto b = balls->begin(); b < balls->end(); ++b) {
            draw_circle(b->center(), b->radius(), &(b->color()));
        }
    }

    max_steps -= 1;
    if (max_steps == 0) {
        exit(0);
    }

    glutSwapBuffers();
}

int main(int argc, char **argv)
{

    int count = 100;

    if (argc > 1) count = atoi(argv[1]);

    max_steps = -1;
    if (argc > 2) max_steps = atoi(argv[2]);


    balls = new std::vector<Ball>(count);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);
    glutInitWindowSize(512, 512);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("ball simulation");
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutIdleFunc(display);

    init();
    glutMainLoop();
    return 0;
}
