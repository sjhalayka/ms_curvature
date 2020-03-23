#ifndef PRIMITIVES_H
#define PRIMITIVES_H


#include <cmath>


class vertex_3
{
public:
	inline vertex_3(void) : x(0.0f), y(0.0f), z(0.0f), index(0) { /*default constructor*/ }
	inline vertex_3(const double src_x, const double src_y, const double src_z, const size_t src_index) : x(src_x), y(src_y), z(src_z), index(src_index) { /* custom constructor */ }
	inline vertex_3(const double src_x, const double src_y, const double src_z) : x(src_x), y(src_y), z(src_z), index(0) { /* custom constructor */ }

	inline bool operator==(const vertex_3& right) const
	{
		if (right.x == x && right.y == y && right.z == z)
			return true;
		else
			return false;
	}

	inline bool operator<(const vertex_3& right) const
	{
		if (right.x > x)
			return true;
		else if (right.x < x)
			return false;

		if (right.y > y)
			return true;
		else if (right.y < y)
			return false;

		if (right.z > z)
			return true;
		else if (right.z < z)
			return false;

		return false;
	}

	inline const vertex_3& operator-(const vertex_3& right) const
	{
		static vertex_3 temp;

		temp.x = this->x - right.x;
		temp.y = this->y - right.y;
		temp.z = this->z - right.z;

		return temp;
	}

	inline const vertex_3& operator+(const vertex_3& right) const
	{
		static vertex_3 temp;

		temp.x = this->x + right.x;
		temp.y = this->y + right.y;
		temp.z = this->z + right.z;

		return temp;
	}

	inline const vertex_3& operator*(const float& right) const
	{
		static vertex_3 temp;

		temp.x = this->x * right;
		temp.y = this->y * right;
		temp.z = this->z * right;

		return temp;
	}

	inline const vertex_3& cross(const vertex_3& right) const
	{
		static vertex_3 temp;

		temp.x = y * right.z - z * right.y;
		temp.y = z * right.x - x * right.z;
		temp.z = x * right.y - y * right.x;

		return temp;
	}

	inline double dot(const vertex_3& right) const
	{
		return x * right.x + y * right.y + z * right.z;
	}

	inline const double self_dot(void)
	{
		return x * x + y * y + z * z;
	}

	inline const double length(void)
	{
		return std::sqrt(self_dot());
	}

	inline const void normalize(void)
	{
		double len = length();

		if (0.0 != len)
		{
			x /= len;
			y /= len;
			z /= len;
		}
	}

	inline void zero(void)
	{
		x = y = z = 0;
	}

	inline void rotate_x(const double& radians)
	{
		double t_y = y;

		y = t_y * cos(radians) + z * sin(radians);
		z = t_y * -sin(radians) + z * cos(radians);
	}

	inline void rotate_y(const double& radians)
	{
		double t_x = x;

		x = t_x * cos(radians) + z * -sin(radians);
		z = t_x * sin(radians) + z * cos(radians);
	}


	double x, y, z;
	size_t index;
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

class triangle
{
public:
	vertex_2 vertex[3];

	inline double area(void)
	{
		if(vertex[0] == vertex[1] || vertex[0] == vertex[2] || vertex[1] == vertex[2])
			return 0;

		double a = (vertex[1].x - vertex[0].x)*(vertex[2].y - vertex[0].y) - (vertex[2].x - vertex[0].x)*(vertex[1].y - vertex[0].y);

		return 0.5*a;
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


class indexed_line_segment
{
public:
	size_t vertex_indices[2];

	double length(const vector<vertex_2> &vertices)
	{
		vertex_2 v0 = vertices[vertex_indices[0]];
		vertex_2 v1 = vertices[vertex_indices[1]];

		return sqrt(pow(v0.x - v1.x, 2.0) + pow(v0.y - v1.y, 2.0));
	}
};


#endif
