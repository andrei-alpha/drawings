/*
 * Utility functions
 * Andrei Antonescu - 2014
*/

using namespace std;

#include <cstdio>
#include <vector>
#include "utils.h"

int W, H; 
typedef pair<int, int> coord;

const int xx[] = {-1, -1, -1, 0, 1, 1, 1, 0};
const int yy[] = {-1, 0, 1, 1, 1, 0, -1, -1};

void utils_init(int width, int height) {
  W = width;
  H = height;
}

int color_diff(int color1, int color2) {
  vector<int> rgb1 = color_decode(color1);
  vector<int> rgb2 = color_decode(color1);
  int dr = rgb1[0] - rgb2[0];
  int dg = rgb1[1] - rgb2[1];
  int db = rgb1[2] - rgb2[2];
  return dr * dr + dg * dg + db * db;
}

inline bool in_bounds(int x, int y) {
	return !(x < 0 || x >= W || y < 0 || y >= H);
}

vector<coord> get_neighbours(coord xy) {
  vector<coord> neighbours;
  
  for (int k = 0; k < 8; ++k)
  	if (in_bounds(xy.first + xx[k], xy.second + yy[k]))
  		neighbours.push_back( make_pair(xy.first + xx[k], xy.second + yy[k]) );
  return neighbours;
}

int color_match(int pixels[][1024], int color, coord xy) {
  vector<coord> neighbours = get_neighbours(xy);

  int score = 0;
  for (int i = 0; i < neighbours.size(); ++i)
  	score += color_diff(color, pixels[neighbours[i].first][neighbours[i].second]);
  return score;
}

int color_encode(int r, int g, int b) {
	r &= 255;
	g &= 255;
	b &= 255;
	return (r << 16) + (g << 8) + b;
}

vector<int> color_decode(int color) {
	vector<int> rgb = {color >> 16, (color >> 8) & 255, color & 255};
	return rgb;
}

int coord_encode(coord xy) {
	return W * xy.first + xy.second;
}

coord coord_decode(int cxy) {
	return make_pair(cxy / W, cxy % W);
}