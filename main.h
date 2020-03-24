#ifndef MS_H
#define MS_H

#include "image.h"
#include "primitives.h"
#include "marching_squares.h"

#include <vector>
using std::vector;

#include <set>
using std::set;

#include <map>
using std::map;

#include <iostream>
using std::cout;
using std::endl;

#include <sstream>
using std::istringstream;

#include <algorithm>
using std::sort;

#include <complex>
using std::complex;


#include <GL/glut.h>


GLint win_id = 0;
GLint win_x = 800, win_y = 800;
GLfloat camera_z = 1.25;
float background_colour = 0.33f;



// Image objects and parameters.
tga tga_texture;
float_grayscale luma;

double template_width = 0;
double template_height = 0;
double step_size = 0;
double isovalue = 0;
double inverse_width = 0;
double grid_x_min = 0;
double grid_y_max = 0;

// Marching squares-generated geometric primitives.
vector<line_segment> line_segments;
map<size_t, vector<size_t> > ls_neighbours;
vector<vertex_2> fn;
vector<vertex_2> v;



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
    glScaled(inverse_width, inverse_width, inverse_width);

    // Render a dark background.
    glColor3f(0, 0, 0);
    glBegin(GL_QUADS);
        glVertex2d(-template_width / 2.0, template_height / 2.0);
        glVertex2d(-template_width / 2.0, -template_height / 2.0);
        glVertex2d(template_width / 2.0, -template_height / 2.0);
        glVertex2d(template_width / 2.0, template_height / 2.0);
    glEnd();

    // Render image area.

    // Render image outline edge length.
    glColor3f(0, 0, 1);
    glLineWidth(4);
    glBegin(GL_LINES);
    for (size_t i = 0; i < line_segments.size(); i++)
    {
        glVertex2d(line_segments[i].vertex[0].x, line_segments[i].vertex[0].y);
        glVertex2d(line_segments[i].vertex[1].x, line_segments[i].vertex[1].y);
    }
    glEnd();


    
    glPointSize(6);
    glBegin(GL_LINES);

    for (size_t i = 0; i < line_segments.size(); i++)
    {
        vertex_2 avg_vertex = line_segments[i].vertex[0];
        avg_vertex = avg_vertex + line_segments[i].vertex[1];
        avg_vertex = avg_vertex / 2.0;

        glColor3f(1, 0, 0);
        glVertex2d(avg_vertex.x, avg_vertex.y);
        glVertex2d(avg_vertex.x + fn[i].x*0.1, avg_vertex.y + fn[i].y*0.1);
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








void get_sorted_points_from_line_segment(size_t ls_index, vector<size_t>& points)
{
    points.resize(2);

    vector<size_t> sorted_vertex_indices;

    // do end point 0 and 1
    sorted_vertex_indices.push_back(line_segments[ls_index].vertex[0].index);
    sorted_vertex_indices.push_back(line_segments[ls_index].vertex[1].index);
    sort(sorted_vertex_indices.begin(), sorted_vertex_indices.end());
    points[0] = sorted_vertex_indices[0];
    points[1] = sorted_vertex_indices[1];
}



void get_all_ls_neighbours(void)
{
    ls_neighbours.clear();

    vector<size_t> default_lookup;

    for (size_t i = 0; i < line_segments.size(); i++)
        ls_neighbours[i] = default_lookup;

    cout << "Enumerating shared faces" << endl;

    map<size_t, vector<size_t> > neighbours;

    for (size_t i = 0; i < line_segments.size(); i++)
    {
        if (i % 100 == 0)
            cout << i + 1 << " of " << line_segments.size() << endl;

        vector<size_t> points;
        get_sorted_points_from_line_segment(i, points);

        for (size_t j = 0; j < points.size(); j++)
neighbours[points[j]].push_back(i);
    }

    cout << "Processing shared faces" << endl;

    size_t count = 0;

    for (map<size_t, vector<size_t> >::const_iterator ci = neighbours.begin(); ci != neighbours.end(); ci++)
    {
        if (count % 100 == 0)
            cout << count + 1 << " of " << neighbours.size() << endl;

        count++;

        ls_neighbours[ci->second[0]].push_back(ci->second[1]);
        ls_neighbours[ci->second[1]].push_back(ci->second[0]);
    }
}


void get_vertices_from_vertex_2(void)
{
    fn.clear();
    v.clear();

    if (0 == line_segments.size())
        return;

    cout << "Welding vertices" << endl;

    // Insert unique vertices into set.
    set<vertex_2> vertex_set;

    for (vector<line_segment>::const_iterator i = line_segments.begin(); i != line_segments.end(); i++)
    {
        vertex_set.insert(i->vertex[0]);
        vertex_set.insert(i->vertex[1]);
    }

    cout << "Vertices: " << vertex_set.size() << endl;

    cout << "Generating vertex indices" << endl;

    // Add indices to the vertices.
    for (set<vertex_2>::const_iterator i = vertex_set.begin(); i != vertex_set.end(); i++)
    {
        size_t index = v.size();
        v.push_back(*i);
        v[index].index = index;
    }

    vertex_set.clear();

    // Re-insert modifies vertices into set.
    for (vector<vertex_2>::const_iterator i = v.begin(); i != v.end(); i++)
        vertex_set.insert(*i);

    cout << "Assigning vertex indices to line segments" << endl;

    // Find the three vertices for each line segment, by index.
    set<vertex_2>::iterator find_iter;

    for (vector<line_segment>::iterator i = line_segments.begin(); i != line_segments.end(); i++)
    {
        find_iter = vertex_set.find(i->vertex[0]);
        i->vertex[0].index = find_iter->index;

        find_iter = vertex_set.find(i->vertex[1]);
        i->vertex[1].index = find_iter->index;
    }

    vertex_set.clear();

    get_all_ls_neighbours();

    cout << "Calculating normals" << endl;
    fn.resize(line_segments.size());

    size_t num_objects = 0;
    bool some_unfound = true;
    size_t last_unfound_index = 0;
    size_t prev_index = 0;
    set<size_t> found_indices;
    found_indices.insert(0);

    do
    {
        const size_t first_index = last_unfound_index;

        prev_index = first_index;

        size_t curr_index = ls_neighbours[first_index][0];


        //size_t backward = prev_index;
        //size_t forward = ls_neighbours[curr_index][0];

        //if (backward == forward)
        //    forward = ls_neighbours[curr_index][1];

        //vertex_2 edge = line_segments[backward].vertex[0] - line_segments[forward].vertex[0];
        //edge.normalize();

        //vertex_3 v0(edge.x, edge.y, 0.0);
        //vertex_3 v1(0.0, 0.0, 1.0);

        //vertex_3 c = v0.cross(v1);
        //fn[prev_index] = vertex_2(c.x, c.y);
        //fn[prev_index].normalize();

        do
        {
            found_indices.insert(curr_index);

            size_t index0 = ls_neighbours[curr_index][0];
            size_t index1 = ls_neighbours[curr_index][1];
  
            if (index0 == prev_index)
            {
                prev_index = curr_index;
                curr_index = index1;
            }
            else
            {
                prev_index = curr_index;
                curr_index = index0;
            }
        
            //backward = prev_index;
            //forward = ls_neighbours[curr_index][0];

            //if (backward == forward)
            //    forward = ls_neighbours[curr_index][1];

            //edge = line_segments[backward].vertex[0] - line_segments[forward].vertex[0];
            //edge.normalize();

            //v0 = vertex_3(edge.x, edge.y, 0.0);
            //v1 = vertex_3(0.0, 0.0, 1.0);

            //c = v0.cross(v1);
            //fn[prev_index] = vertex_2(c.y, c.x);
            //fn[prev_index].normalize();

        } while (curr_index != first_index);

        for (size_t i = 0; i < fn.size(); i++)
        {
            if (found_indices.end() == found_indices.find(i))
            {
                last_unfound_index = i;
                found_indices.insert(last_unfound_index);
                break;
            }
        }



        num_objects++;

    } while (found_indices.size() != fn.size());

    cout << "Found " << num_objects << " object(s)." << endl;
}




#endif
