#include "main.h"


// Cat image from: http://www.iacuc.arizona.edu/training/cats/index.html
int main(int argc, char **argv)
{
	// Read a 24-bit uncompressed/non-RLE Targa file, and then convert it to a floating point grayscale image.
	cout << "Reading figure1.tga" << endl;
	cout << endl;

	if(false == convert_tga_to_float_grayscale("figure1.tga", tga_texture, luma, true, true, true))
		return 0;

	// Too small.
	if(luma.px < 3 || luma.py < 3)
	{
		cout << "Template must be at least 3x3 pixels in size." << endl;
		return 0;
	}

	// If rendering problems occur, try using images of equal width and height (e.g. px = py).
	// Also try sizes that are powers of two (e.g. px = py = 2^x, x = 0, 1, 2, 3, ...).

	template_width = 1.0;

	inverse_width = 1.0/template_width;
	step_size = template_width/static_cast<double>(luma.px - 1);
	template_height = step_size*(luma.py - 1); // Assumes square pixels.

	isovalue = 0.5;

	// Print basic information.
	cout << "Template info: " << endl;
	cout << luma.px << " x " << luma.py << " pixels" << endl;
	cout << template_width << " x " << template_height << " metres" << endl;
	cout << endl;

	grid_x_min = -template_width/2.0;
	grid_y_max = template_height/2.0;
	
	cout << "Grid info: " << endl;
	cout << luma.px - 1 << " x " << luma.py - 1 << " grid squares" << endl;
	cout << "x min (-x max): " << grid_x_min << endl;
	cout << "y min (-y max): " << -grid_y_max << endl;
	cout << "Isovalue: " << isovalue << endl;
	cout << endl;

	cout << "Generating geometric primitives..." << endl;
	cout << endl;

	double grid_x_pos = grid_x_min; // Start at minimum x.
	double grid_y_pos = grid_y_max; // Start at maximum y.

	size_t box_count = 0;

	// Begin march.
	for(size_t y = 0; y < luma.py - 1; y++, grid_y_pos -= step_size, grid_x_pos = grid_x_min)
	{
		for(size_t x = 0; x < luma.px - 1; x++, grid_x_pos += step_size)
		{
			// Corner vertex order: 03
			//                      12
			// e.g.: clockwise, as in OpenGL

			grid_square g;

			g.vertex[0] = vertex_2(grid_x_pos, grid_y_pos);
			g.vertex[1] = vertex_2(grid_x_pos, grid_y_pos - step_size);
			g.vertex[2] = vertex_2(grid_x_pos + step_size, grid_y_pos - step_size);
			g.vertex[3] = vertex_2(grid_x_pos + step_size, grid_y_pos);

			g.value[0] = luma.pixel_data[y*luma.px + x];
			g.value[1] = luma.pixel_data[(y + 1)*luma.px + x];
			g.value[2] = luma.pixel_data[(y + 1)*luma.px + (x + 1)];
			g.value[3] = luma.pixel_data[y*luma.px + (x + 1)];

			size_t curr_ls_size = line_segments.size();

			g.generate_primitives(line_segments, isovalue);

			size_t new_ls_size = line_segments.size();

			if (curr_ls_size != new_ls_size)
				box_count++;
		}
	}


	// Gather and print final information
	double length = 0;

	double x_max = grid_x_min;
	double x_min = -grid_x_min;
	double y_max = -grid_y_max;
	double y_min = grid_y_max;

	for(size_t i = 0; i < line_segments.size(); i++)
	{
		length += line_segments[i].length();

		if(line_segments[i].vertex[0].x > x_max)
			x_max = line_segments[i].vertex[0].x; 

		if(line_segments[i].vertex[1].x > x_max)
			x_max = line_segments[i].vertex[1].x;

		if(line_segments[i].vertex[0].x < x_min)
			x_min = line_segments[i].vertex[0].x;

		if(line_segments[i].vertex[1].x < x_min)
			x_min = line_segments[i].vertex[1].x;

		if(line_segments[i].vertex[0].y > y_max)
			y_max = line_segments[i].vertex[0].y;

		if(line_segments[i].vertex[1].y > y_max)
			y_max = line_segments[i].vertex[1].y;

		if(line_segments[i].vertex[0].y < y_min)
			y_min = line_segments[i].vertex[0].y;

		if(line_segments[i].vertex[1].y < y_min)
			y_min = line_segments[i].vertex[1].y;
	}

	cout << "Geometric primitive info: " << endl;
	cout << "Vertex x min, max: " << x_min << ", " << x_max << endl;
	cout << "Vertex y min, max: " << y_min << ", " << y_max << endl;
	cout << "Line segments:     " << line_segments.size() << endl;
	cout << "Length:            " << length << endl;


	get_vertices_from_vertex_2();

	cout << fn.size() << endl;


	double K = 0;

	for (size_t i = 0; i < line_segments.size(); i++)
	{
		if (ls_neighbours[i].size() != 2)
		{
			cout << "Error" << endl;
			return 1;
		}

		size_t neighbour_0_index = ls_neighbours[i][0];
		size_t neighbour_1_index = ls_neighbours[i][1];

		vertex_2 this_normal = fn[i];
		vertex_2 neighbour_0_normal = fn[neighbour_0_index];
		vertex_2 neighbour_1_normal = fn[neighbour_1_index];

		double d_i = this_normal.dot(neighbour_0_normal) + this_normal.dot(neighbour_1_normal);
		d_i /= 2.0;

		double k_i = (1.0 - d_i) / 2.0;

		K += k_i;
	}

	K /= static_cast<double>(line_segments.size());

	cout << "Dot product dimension: " << 1.0 + K << endl;

	cout << "Box-counting dimension: " << log(static_cast<double>(box_count)) / log(1.0 / step_size) << endl;

	render_image(argc, argv);


	return 0;
}

