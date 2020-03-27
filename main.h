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

#include <algorithm>
using std::sort;













void get_sorted_points_from_line_segment(const line_segment_data &lsd, size_t ls_index, vector<size_t>& points)
{
    points.resize(2);

    vector<size_t> sorted_vertex_indices;

    // do end point 0 and 1
    sorted_vertex_indices.push_back(lsd.line_segments[ls_index].vertex[0].index);
    sorted_vertex_indices.push_back(lsd.line_segments[ls_index].vertex[1].index);
    sort(sorted_vertex_indices.begin(), sorted_vertex_indices.end());
    points[0] = sorted_vertex_indices[0];
    points[1] = sorted_vertex_indices[1];
}



void get_all_line_segment_neighbours(line_segment_data &lsd)
{
    lsd.line_segment_neighbours.clear();

    vector<size_t> default_lookup;

    for (size_t i = 0; i < lsd.line_segments.size(); i++)
        lsd.line_segment_neighbours[i] = default_lookup;

    cout << "Enumerating shared faces" << endl;

    map<size_t, vector<size_t> > neighbours;

    for (size_t i = 0; i < lsd.line_segments.size(); i++)
    {
        if (i % 100 == 0)
            cout << i + 1 << " of " << lsd.line_segments.size() << endl;

        vector<size_t> points;
        get_sorted_points_from_line_segment(lsd, i, points);

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

        lsd.line_segment_neighbours[ci->second[0]].push_back(ci->second[1]);
        lsd.line_segment_neighbours[ci->second[1]].push_back(ci->second[0]);
    }
}



void process_line_segments(line_segment_data &lsd)
{
    lsd.face_normals.clear();
    lsd.vertices.clear();

    if (0 == lsd.line_segments.size())
        return;

    cout << "Welding vertices" << endl;

    // Insert unique vertices into set.
    set<vertex_2> vertex_set;

    for (vector<line_segment>::const_iterator i = lsd.line_segments.begin(); i != lsd.line_segments.end(); i++)
    {
        vertex_set.insert(i->vertex[0]);
        vertex_set.insert(i->vertex[1]);
    }

    cout << "Vertices: " << vertex_set.size() << endl;

    cout << "Generating vertex indices" << endl;

    // Add indices to the vertices.
    for (set<vertex_2>::const_iterator i = vertex_set.begin(); i != vertex_set.end(); i++)
    {
        size_t index = lsd.vertices.size();
        lsd.vertices.push_back(*i);
        lsd.vertices[index].index = index;
    }

    vertex_set.clear();

    // Re-insert modifies vertices into set.
    for (vector<vertex_2>::const_iterator i = lsd.vertices.begin(); i != lsd.vertices.end(); i++)
        vertex_set.insert(*i);

    cout << "Assigning vertex indices to line segments" << endl;

    // Find the two vertices for each line segment, by index.
    set<vertex_2>::iterator find_iter;

    for (vector<line_segment>::iterator i = lsd.line_segments.begin(); i != lsd.line_segments.end(); i++)
    {
        find_iter = vertex_set.find(i->vertex[0]);
        i->vertex[0].index = find_iter->index;

        find_iter = vertex_set.find(i->vertex[1]);
        i->vertex[1].index = find_iter->index;
    }

    vertex_set.clear();

    get_all_line_segment_neighbours(lsd);

    cout << "Calculating normals" << endl;
    lsd.face_normals.resize(lsd.line_segments.size());

    size_t num_objects = 0;
    size_t last_unprocessed_index = 0;

    set<size_t> processed_indices;
    vector<tri_index> tri_indices;

    // Get the neighbours for each line segment
    do
    {
        const size_t first_index = last_unprocessed_index;

        size_t prev_index = first_index;
        size_t curr_index = lsd.line_segment_neighbours[first_index][0];
        size_t next_index = lsd.line_segment_neighbours[curr_index][0];

        tri_index t;
        t.prev_index = prev_index;
        t.curr_index = curr_index;
        t.next_index = next_index;
        tri_indices.push_back(t);
        processed_indices.insert(curr_index);

        // For each disconnected object in the image
        do
        {
            size_t index0 = lsd.line_segment_neighbours[curr_index][0];
            size_t index1 = lsd.line_segment_neighbours[curr_index][1];

            if (index0 == prev_index)
            {
                prev_index = curr_index;
                curr_index = index1;
                next_index = lsd.line_segment_neighbours[curr_index][1];

                if (prev_index == next_index)
                    next_index = lsd.line_segment_neighbours[curr_index][0];
            }
            else if (index1 == prev_index)
            {
                prev_index = curr_index;
                curr_index = index0;
                next_index = lsd.line_segment_neighbours[curr_index][0];

                if (prev_index == next_index)
                    next_index = lsd.line_segment_neighbours[curr_index][1];
            }
            else
            {
                // This should never happen.
            }

            tri_index t;
            t.prev_index = prev_index;
            t.curr_index = curr_index;
            t.next_index = next_index;
            tri_indices.push_back(t);
            processed_indices.insert(curr_index);

        } while (curr_index != first_index);

        for (size_t i = 0; i < lsd.face_normals.size(); i++)
        {
            if (processed_indices.end() == processed_indices.find(i))
            {
                last_unprocessed_index = i;
                break;
            }
        }
        
        // Use the neighbour data to calculate the line segment normals
        for (size_t i = 0; i < tri_indices.size(); i++)
        {
            size_t prev_index = tri_indices[i].prev_index;
            size_t curr_index = tri_indices[i].curr_index;
            size_t next_index = tri_indices[i].next_index;

            size_t first_vertex_index = 0;
            size_t last_vertex_index = 0;

            if (lsd.line_segment_neighbours[prev_index][0] == curr_index)
                first_vertex_index = 0;
            else
                first_vertex_index = 1;

            if (lsd.line_segment_neighbours[next_index][0] == curr_index)
                last_vertex_index = 0;
            else
                last_vertex_index = 1;

            vertex_2 edge = lsd.line_segments[prev_index].vertex[first_vertex_index] - lsd.line_segments[next_index].vertex[last_vertex_index];
            lsd.face_normals[curr_index] = vertex_2(-edge.y, edge.x);
            lsd.face_normals[curr_index].normalize();
        }
 
        if(num_objects % 100 == 0)
            cout << "Found object " << num_objects + 1 << endl;
        
        num_objects++;

    } while (processed_indices.size() != lsd.face_normals.size());

    cout << "Found " << num_objects << " object(s)." << endl;
}




#endif
