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



void get_sorted_points_from_line_segment(size_t ls_index, vector<size_t>& points)
{
    points.resize(2);

    vector<size_t> sorted_vertex_indices;

    // do end point 0
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

    cout << "Calculating normals" << endl;
    fn.resize(line_segments.size());

    for (size_t i = 0; i < line_segments.size(); i++)
    {
        vertex_2 edge = line_segments[i].vertex[1] - line_segments[i].vertex[0];

        vertex_3 v0(edge.x, edge.y, 0.0);
        vertex_3 v1(0.0, 0.0, 1.0);
        vertex_3 c = v0.cross(v1);
        fn[i] = vertex_2(c.x, c.y);
        fn[i].normalize();
    }
}




#endif
