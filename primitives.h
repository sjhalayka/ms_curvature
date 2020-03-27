#ifndef PRIMITIVES_H
#define PRIMITIVES_H


#include <cmath>


#include <vector>
using std::vector;

#include <map>
using std::map;


class tri_index
{
public:

	size_t prev_index, curr_index, next_index;

	tri_index(void)
	{
		prev_index = curr_index = next_index = 0;
	}

	inline bool operator<(const tri_index& right) const
	{
		if (right.prev_index > prev_index)
			return true;
		else if (right.prev_index < prev_index)
			return false;

		if (right.curr_index > curr_index)
			return true;
		else if (right.curr_index < curr_index)
			return false;

		if (right.next_index > next_index)
			return true;
		else if (right.next_index < next_index)
			return false;

		return false;
	}
};



class vertex_2
{
public:
	double x;
	double y;
	size_t index;

	inline const void normalize(void)
	{
		double len = length();

		if (0.0 != len)
		{
			x /= len;
			y /= len;
		}
	}

	vertex_2(const double src_x = 0, const double src_y = 0)
	{
		x = src_x;
		y = src_y;
		index = 0;
	}

	inline bool operator==(const vertex_2 &right) const
	{
		if(right.x == x && right.y == y)
			return true;
		else
			return false;
	}

	inline const vertex_2& operator*(const float& right) const
	{
		static vertex_2 temp;

		temp.x = this->x * right;
		temp.y = this->y * right;

		return temp;
	}

	inline const vertex_2& operator/(const float& right) const
	{
		static vertex_2 temp;

		temp.x = this->x / right;
		temp.y = this->y / right;

		return temp;
	}

	inline bool operator<(const vertex_2 &right) const
	{
		if(right.x > x)
			return true;
		else if(right.x < x)
			return false;
			
		if(right.y > y)
			return true;
		else if(right.y < y)
			return false;

		return false;
	}

	inline const vertex_2& operator+(const vertex_2& right) const
	{
		static vertex_2 temp;

		temp.x = this->x + right.x;
		temp.y = this->y + right.y;

		return temp;
	}

	inline const vertex_2& operator-(const vertex_2 &right) const
	{
		static vertex_2 temp;

		temp.x = this->x - right.x;
		temp.y = this->y - right.y;

		return temp;
	}

	inline double dot(const vertex_2 &right) const
	{
		return x*right.x + y*right.y;
	}

	inline const double self_dot(void)
	{
		return x*x + y*y;
	}

	inline const double length(void)
	{
		return sqrt(self_dot());
	}
};

class line_segment
{
public:

	vertex_2 vertex[2];


	double length(void)
	{
		return sqrt( pow(vertex[0].x - vertex[1].x, 2.0) + pow(vertex[0].y - vertex[1].y, 2.0) );
	}
};



// Marching squares-related geometric primitives.
class line_segment_data
{
public:
	vector<line_segment> line_segments;
	map<size_t, vector<size_t> > line_segment_neighbours;
	vector<vertex_2> face_normals;
	vector<vertex_2> vertices;
};


#endif
