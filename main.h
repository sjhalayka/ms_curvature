#ifndef MAIN_H
#define MAIN_H

#include "image.h"
#include "primitives.h"
#include "marching_squares.h"


#include <iostream>
using std::cout;
using std::endl;





double template_width = 0;
double step_size = 0;
double template_height = 0;
double isovalue = 0;
double grid_x_min = 0;
double grid_y_max = 0;

line_segment_data lsd;


//#define USE_OPENGL

#ifdef USE_OPENGL
#include <GL/glut.h>

GLint win_id = 0;
GLint win_x = 800, win_y = 800;
GLfloat camera_z = 1.25;
float background_colour = 0.33f;

void idle_func(void)
{
    glutPostRedisplay();
}

// Resize window.
void reshape_func(int width, int height)
{
    if (width < 1)
        width = 1;

    if (height < 1)
        height = 1;

    win_x = width;
    win_y = height;

    // Viewport setup.
    glutSetWindow(win_id);
    glutReshapeWindow(win_x, win_y);
    glViewport(0, 0, win_x, win_y);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, static_cast<GLfloat>(win_x) / static_cast<GLfloat>(win_y), 0.1, 10);
    gluLookAt(0, 0, camera_z, 0, 0, 0, 0, 1, 0);
}

// Visualization.
void display_func(void)
{
    glClear(GL_COLOR_BUFFER_BIT);

    // Scale all geometric primitives so that template width == 1.
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glScaled(1.0 / template_width, 1.0 / template_width, 1.0 / template_width);

    // Render a dark background.
    glColor3f(0, 0, 0);
    glBegin(GL_QUADS);
    glVertex2d(-template_width / 2.0, template_height / 2.0);
    glVertex2d(-template_width / 2.0, -template_height / 2.0);
    glVertex2d(template_width / 2.0, -template_height / 2.0);
    glVertex2d(template_width / 2.0, template_height / 2.0);
    glEnd();

    // Render image outline edge length.
    glColor3f(0, 0, 1);
    glLineWidth(4);
    glBegin(GL_LINES);
    for (size_t i = 0; i < lsd.line_segments.size(); i++)
    {
        glVertex2d(lsd.line_segments[i].vertex[0].x, lsd.line_segments[i].vertex[0].y);
        glVertex2d(lsd.line_segments[i].vertex[1].x, lsd.line_segments[i].vertex[1].y);
    }
    glEnd();

    glColor3f(1, 0.5f, 0);
    glPointSize(6);
    glBegin(GL_LINES);

    for (size_t i = 0; i < lsd.line_segments.size(); i++)
    {
        vertex_2 avg_vertex = lsd.line_segments[i].vertex[0];
        avg_vertex = avg_vertex + lsd.line_segments[i].vertex[1];
        avg_vertex = avg_vertex / 2.0;

        glVertex2d(avg_vertex.x, avg_vertex.y);
        glVertex2d(avg_vertex.x + lsd.face_normals[i].x * 0.001, avg_vertex.y + lsd.face_normals[i].y * 0.001);
    }
    glEnd();

    glFlush();
}

void render_image(int& argc, char**& argv)
{
    // Initialize OpenGL.
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(win_x, win_y);
    win_id = glutCreateWindow("Marching Squares");
    glutIdleFunc(idle_func);
    glutReshapeFunc(reshape_func);
    glutDisplayFunc(display_func);

    glShadeModel(GL_FLAT);
    glClearColor(background_colour, background_colour, background_colour, 1);

    // Begin rendering.
    glutMainLoop();

    // Cleanup OpenGL.
    glutDestroyWindow(win_id);
}

#endif // USE_OPENGL

#endif