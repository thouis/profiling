#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <math.h>

#include <vector>

#include "Ball.hpp"

// Global variable to work around glut's display callback not taking arguments
std::vector<Ball> *balls;
Grid<Ball *> *grid;
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
            auto it = grid->near_begin(b1->center()[0], b1->center()[1],
                                       2 * b1->radius(), 2 * b1->radius());
            auto end = grid->near_end(b1->center()[0], b1->center()[1],
                                      2 * b1->radius(), 2 * b1->radius());
            while (it != end) {
                auto& _b1 = (*b1);
                auto& _b2 = *(*it);
                if ((&_b1 != &_b2) && (b1->near(_b2))) {
                    _b1.collide(_b2);
                }
                ++it;
            }
        }

        for (auto b1 = balls->begin(); b1 < balls->end(); ++b1) {
            b1->collide_walls();
        }

        for (auto b1 = balls->begin(); b1 < balls->end(); ++b1) {
            grid->remove(b1->center()[0],
                         b1->center()[1],
                         &(*b1));
            b1->step(0.01);
            grid->add(b1->center()[0],
                      b1->center()[1],
                      &(*b1));
        }


        for (auto b = balls->begin(); b < balls->end(); ++b) {
            draw_circle(b->center(), b->radius(), &(b->color()));
        }
    }

    std::cout << "steps" << max_steps << std::endl;

    max_steps -= 1;
    if (max_steps == 0) {
        exit(0);
    }

    glutSwapBuffers();
}

int main(int argc, char **argv)
{
    int count = atoi(argv[1]);

    max_steps = atoi(argv[2]);
    balls = new std::vector<Ball>(count);
    /* make a grid that can hold O(1) balls per bin */
    grid = new Grid<Ball *>(-1, -1, 1, 1,
                            2 * (*balls)[0].radius(), 2 * (*balls)[0].radius());
    /* add the balls into the grid */
    for (std::size_t i = 0; i < balls->size(); i++)
        grid->add((*balls)[i].center()[0],
                  (*balls)[i].center()[1],
                  &(*balls)[i]);

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
