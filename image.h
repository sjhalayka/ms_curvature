#ifndef IMAGE_H
#define IMAGE_H


#include <vector>
using std::vector;

#include <fstream>
using std::ifstream;

#include <ios>
using std::ios;

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

#include <cstring>


// http://local.wasp.uwa.edu.au/~pbourke/dataformats/tga/
class tga
{
public:

    tga(void)
    {
        idlength = 0;
        colourmaptype = 0;
        datatypecode = 0;
        colourmaporigin = 0;
        colourmaporigin = 0;
        colourmaplength = 0;
        colourmapdepth = 0;
        x_origin = 0;
        y_origin = 0;
        px = 0;
        py = 0;
        bitsperpixel = 0;
        imagedescriptor = 0;
    }

   unsigned char  idlength;
   unsigned char  colourmaptype;
   unsigned char  datatypecode;
   unsigned short int colourmaporigin;
   unsigned short int colourmaplength;
   unsigned char  colourmapdepth;
   unsigned short int x_origin;
   unsigned short int y_origin;
   unsigned short int px;
   unsigned short int py;
   unsigned char  bitsperpixel;
   unsigned char  imagedescriptor;

   vector<char> idstring;

   vector<unsigned char> pixel_data;
};

class float_grayscale
{
public:

    float_grayscale(void)
    {
        px = py = 0;
    }

	unsigned short int px;
	unsigned short int py;
	vector<float> pixel_data;
};

float int_rgb_to_float_grayscale(const unsigned char r, const unsigned char g, const unsigned char b);
bool convert_tga_to_float_grayscale(const char *const filename, tga &t, float_grayscale &l, const bool make_black_border = false, const bool reverse_rows = true, const bool reverse_pixel_byte_order = true);

#endif
