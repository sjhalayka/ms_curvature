// Code by: Shawn Halayka -- sjhalayka@gmail.com
// Code is in the public domain


#include "main.h"


int main(int argc, char **argv)
{
	// Image objects
	tga tga_texture;
	float_grayscale luma;

	// Read a 24-bit uncompressed/non-RLE Targa file, and then convert it to a 
	// floating point grayscale image
	cout << "Reading figure1.tga" << endl;
	cout << endl;

	// Make absolutely sure that the make_black_border parameter is set to true
	// This ensures that the line segment mesh(es) are closed, and so 
	// there are exactly two line segment neighbours per line segment
	if (false == convert_tga_to_float_grayscale("figure1.tga", tga_texture, luma, true, true, true))
	{
		cout << "Error reading figure1.tga" << endl;
		return 1;
	}

	// Too small
	if(luma.px < 3 || luma.py < 3)
	{
		cout << "Template must be at least 3x3 pixels in size." << endl;
		return 2;
	}

	// Not square
	if (luma.px != luma.py)
	{
		cout << "Template must be square." << endl;
		return 3;
	}


	// Marching Squares parameters
	template_width = 1.0;
	step_size = template_width/static_cast<double>(luma.px - 1);
	template_height = step_size*(luma.py - 1); // Assumes square pixels.
	isovalue = 0.5;
	grid_x_min = -template_width / 2.0;
	grid_y_max = template_height / 2.0;


	// Print basic data
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


	double grid_x_pos = grid_x_min; // Start at minimum x
	double grid_y_pos = grid_y_max; // Start at maximum y

	size_t box_count = 0;

	// Begin march over the plane
	for(size_t y = 0; y < luma.py - 1; y++, grid_y_pos -= step_size, grid_x_pos = grid_x_min)
	{
		for(size_t x = 0; x < luma.px - 1; x++, grid_x_pos += step_size)
		{
			// Corner vertex order: 03
			//                      12

			grid_square g;

			g.vertex[0] = vertex_2(grid_x_pos, grid_y_pos);
			g.vertex[1] = vertex_2(grid_x_pos, grid_y_pos - step_size);
			g.vertex[2] = vertex_2(grid_x_pos + step_size, grid_y_pos - step_size);
			g.vertex[3] = vertex_2(grid_x_pos + step_size, grid_y_pos);

			g.value[0] = luma.pixel_data[y*luma.px + x];
			g.value[1] = luma.pixel_data[(y + 1)*luma.px + x];
			g.value[2] = luma.pixel_data[(y + 1)*luma.px + (x + 1)];
			g.value[3] = luma.pixel_data[y*luma.px + (x + 1)];

			// Add line segment primitives to line segment vector
			//
			// Box-counting dimension is very simple to calculate
			// when using Marching Squares -- if primitives were added,
			// then the boundary is covered by this particular 
			// grid_square (box)
			if (0 < g.generate_primitives(lsd.line_segments, isovalue))
				box_count++;
		}
	}


	// Ultimately, this enumerates the line segment neighbour data,
	// and uses that to calculate the face normal data
	lsd.process_line_segments();


	// Calculate curvature-based dimension now that we have the face normals
	vector<double> k;

	for (size_t i = 0; i < lsd.line_segments.size(); i++)
	{
		if (lsd.line_segment_neighbours[i].size() != 2)
		{
			cout << "Error" << endl;
			return 4;
		}

		size_t neighbour_0_index = lsd.line_segment_neighbours[i][0];
		size_t neighbour_1_index = lsd.line_segment_neighbours[i][1];

		vertex_2 this_normal = lsd.face_normals[i];
		vertex_2 neighbour_0_normal = lsd.face_normals[neighbour_0_index];
		vertex_2 neighbour_1_normal = lsd.face_normals[neighbour_1_index];

		// Get the average dot product
		double d_i = this_normal.dot(neighbour_0_normal) + this_normal.dot(neighbour_1_normal);
		d_i /= 2.0;

		// Normalize the average dot product to get the curvature
		double k_i = (1.0 - d_i) / 2.0;

		k.push_back(k_i);
	}

	double K = 0;

	for (size_t i = 0; i < k.size(); i++)
		K += k[i];

	// Get the average normalized curvature
	K /= static_cast<double>(k.size());

	cout << "Curvature:                 " << K << " +/- " << standard_deviation(k) << endl;
	cout << "Curvature-based dimension: " << 1.0 + K << endl;
	cout << "Box-counting dimension:    " << log(static_cast<double>(box_count)) / log(1.0 / step_size) << endl;


#ifdef USE_OPENGL
	render_image(argc, argv);
#endif


	return 0;
}

