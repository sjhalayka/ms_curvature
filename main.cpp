#include "main.h"


int main(void)
{
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

	// Not square.
	if (luma.px != luma.py)
	{
		cout << "Template must be square." << endl;
		return 0;
	}

	template_width = 1.0;
	inverse_width = 1.0/template_width;
	step_size = template_width/static_cast<double>(luma.px - 1);
	template_height = step_size*(luma.py - 1); // Assumes square pixels.
	isovalue = 0.5;
	grid_x_min = -template_width / 2.0;
	grid_y_max = template_height / 2.0;

	// Print basic information.
	cout << "Template info: " << endl;
	cout << luma.px << " x " << luma.py << " pixels" << endl;
	cout << template_width << " x " << template_height << " metres" << endl;
	cout << endl;	
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


	process_line_segments();

	// Calculate curvature-based dimension
	double K = 0;

	for (size_t i = 0; i < line_segments.size(); i++)
	{
		if (line_segment_neighbours[i].size() != 2)
		{
			cout << "Error" << endl;
			return 1;
		}

		size_t neighbour_0_index = line_segment_neighbours[i][0];
		size_t neighbour_1_index = line_segment_neighbours[i][1];

		vertex_2 this_normal = face_normals[i];
		vertex_2 neighbour_0_normal = face_normals[neighbour_0_index];
		vertex_2 neighbour_1_normal = face_normals[neighbour_1_index];

		double d_i = this_normal.dot(neighbour_0_normal) + this_normal.dot(neighbour_1_normal);
		d_i /= 2.0;

		double k_i = (1.0 - d_i) / 2.0;

		K += k_i;
	}

	K /= static_cast<double>(line_segments.size());

	cout << "Curvature-based dimension: " << 1.0 + K << endl;
	cout << "Box-counting dimension:    " << log(static_cast<double>(box_count)) / log(1.0 / step_size) << endl;

	return 0;
}

