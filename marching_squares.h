// Code by: Shawn Halayka -- sjhalayka@gmail.com
// Code is in the public domain


#ifndef MARCHING_SQUARES_H
#define MARCHING_SQUARES_H

#include <vector>
using std::vector;

#include "primitives.h"

class grid_square
{
public:
	vertex_2 vertex[4];
	double value[4];

	grid_square(void)
	{
		value[0] = value[1] = value[2] = value[3] = 0;
	}

	inline vertex_2 vertex_interp(vertex_2 p1, vertex_2 p2, double v1, double v2, const double isovalue)
	{
		// Sort the vertices to avoid cracks in the mesh
		if (p2 < p1)
		{
			vertex_2 tv = p1;
			p1 = p2;
			p2 = tv;

			double td = v1;
			v1 = v2;
			v2 = td;
		}

		static vertex_2 temp;
		static double mu;

		// http://paulbourke.net/geometry/polygonise/
		mu = (isovalue - v1)/(v2 - v1);
		temp.x = p1.x + mu*(p2.x - p1.x);
		temp.y = p1.y + mu*(p2.y - p1.y);

		return temp;
	}

	inline void generate_primitives(vector<line_segment> &line_segments, const double isovalue)
	{
		// Identify which of the 4 corners of the square are within the isosurface
		//
		// Max 16 cases. Only 14 cases create line segments

		// Corner vertex order: 03
		//                      12

		unsigned short int mask = 0;

		if(value[0] >= isovalue) 
			mask |= 1;

		if(value[1] >= isovalue)
			mask |= 2;

		if(value[2] >= isovalue)
			mask |= 4;

		if(value[3] >= isovalue)
			mask |= 8;

		// Max 6 vertices per grid cube
		static vertex_2 a, b, c, d, e, f;
		
		// Max two line segments per grid cube
		static line_segment ls;

		// Handle the 16 cases manually
		switch(mask)
		{
			case 1:
			{
				//1:
				//10
				//00

				b = vertex_interp(vertex[0], vertex[1], value[0], value[1], isovalue);
				c = vertex_interp(vertex[0], vertex[3], value[0], value[3], isovalue);

				ls.vertex[0] = b;
				ls.vertex[1] = c;

				line_segments.push_back(ls);

				break;
			}
			case 2:
			{
				//2:
				//00
				//10

				a = vertex_interp(vertex[1], vertex[0], value[1], value[0], isovalue);
				c = vertex_interp(vertex[1], vertex[2], value[1], value[2], isovalue);

				ls.vertex[0] = a;
				ls.vertex[1] = c;
				line_segments.push_back(ls);

				break;
			}
			case 4:
			{
				//4:
				//00
				//01

				a = vertex_interp(vertex[2], vertex[1], value[2], value[1], isovalue);
				c = vertex_interp(vertex[2], vertex[3], value[2], value[3], isovalue);

				ls.vertex[0] = a;
				ls.vertex[1] = c;
				line_segments.push_back(ls);

				break;
			}
			case 8:
			{
				//8:
				//01
				//00

				a = vertex_interp(vertex[3], vertex[0], value[3], value[0], isovalue);
				b = vertex_interp(vertex[3], vertex[2], value[3], value[2], isovalue);

				ls.vertex[0] = a;
				ls.vertex[1] = b;
				line_segments.push_back(ls);

				break;
			}


			case 3:
			{
				//3:
				//10
				//10

				c = vertex_interp(vertex[1], vertex[2], value[1], value[2], isovalue);
				d = vertex_interp(vertex[0], vertex[3], value[0], value[3], isovalue);

				ls.vertex[0] = d;
				ls.vertex[1] = c;
				line_segments.push_back(ls);

				break;
			}
			case 6:
			{
				//6:
				//00
				//11

				a = vertex_interp(vertex[1], vertex[0], value[1], value[0], isovalue);
				d = vertex_interp(vertex[2], vertex[3], value[2], value[3], isovalue);

				ls.vertex[0] = a;
				ls.vertex[1] = d;
				line_segments.push_back(ls);

				break;
			}
			case 9:
			{
				//9:
				//11
				//00

				b = vertex_interp(vertex[0], vertex[1], value[0], value[1], isovalue);
				c = vertex_interp(vertex[3], vertex[2], value[3], value[2], isovalue);

				ls.vertex[0] = b;
				ls.vertex[1] = c;
				line_segments.push_back(ls);

				break;
			}
			case 12:
			{
				//12:
				//01
				//01

				a = vertex_interp(vertex[3], vertex[0], value[3], value[0], isovalue);
				b = vertex_interp(vertex[2], vertex[1], value[2], value[1], isovalue);

				ls.vertex[0] = a;
				ls.vertex[1] = b;
				line_segments.push_back(ls);

				break;
			}


			case 5:
			{
				//5:
				//10
				//01

				b = vertex_interp(vertex[0], vertex[1], value[0], value[1], isovalue);
				c = vertex_interp(vertex[0], vertex[3], value[0], value[3], isovalue);
				d = vertex_interp(vertex[2], vertex[1], value[2], value[1], isovalue);
				f = vertex_interp(vertex[2], vertex[3], value[2], value[3], isovalue);

				ls.vertex[0] = b;
				ls.vertex[1] = c;
				line_segments.push_back(ls);

				ls.vertex[0] = d;
				ls.vertex[1] = f;
				line_segments.push_back(ls);

				break;
			}
			case 10:
			{
				//10:
				//01
				//10

				a = vertex_interp(vertex[1], vertex[0], value[1], value[0], isovalue);
				c = vertex_interp(vertex[1], vertex[2], value[1], value[2], isovalue);
				d = vertex_interp(vertex[3], vertex[0], value[3], value[0], isovalue);
				e = vertex_interp(vertex[3], vertex[2], value[3], value[2], isovalue);

				ls.vertex[0] = a;
				ls.vertex[1] = c;
				line_segments.push_back(ls);

				ls.vertex[0] = d;
				ls.vertex[1] = e;
				line_segments.push_back(ls);

				break;
			}


			case 7:
			{
				//7:
				//10
				//11

				d = vertex_interp(vertex[2], vertex[3], value[2], value[3], isovalue);
				e = vertex_interp(vertex[0], vertex[3], value[0], value[3], isovalue);

				ls.vertex[0] = e;
				ls.vertex[1] = d;
				line_segments.push_back(ls);

				break;
			}
			case 11:
			{
				//11:
				//11
				//10

				c = vertex_interp(vertex[1], vertex[2], value[1], value[2], isovalue);
				d = vertex_interp(vertex[3], vertex[2], value[3], value[2], isovalue);

				ls.vertex[0] = c;
				ls.vertex[1] = d;
				line_segments.push_back(ls);

				break;
			}
			case 13:
			{
				//13:
				//11
				//01

				b = vertex_interp(vertex[0], vertex[1], value[0], value[1], isovalue);
				c = vertex_interp(vertex[2], vertex[1], value[2], value[1], isovalue);

				ls.vertex[0] = b;
				ls.vertex[1] = c;
				line_segments.push_back(ls);

				break;
			}
			case 14:
			{
				//14:
				//01
				//11

				a = vertex_interp(vertex[1], vertex[0], value[1], value[0], isovalue);
				e = vertex_interp(vertex[3], vertex[0], value[3], value[0], isovalue);

				ls.vertex[0] = a;
				ls.vertex[1] = e;
				line_segments.push_back(ls);

				break;
			}
			case 15:
			{
				// Case 15 (all inside of image area) produces no outlines

				//15:
				//11
				//11

				break;
			}

			default:
			{
				// Case 0 (all outside of image area) produces no outlines

				//0:
				//00
				//00

				break;
			}
		}
	}
};

#endif
