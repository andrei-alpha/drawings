/* Utility functions 
 * Andrei Antonescu - 2014
*/
typedef pair<int, int> coord;

extern void utils_init(int width, int height);

extern int color_diff(int color1, int color2);

extern vector<coord> get_neighbours(coord xy);

extern int color_match(int pixels[][2048], int color, coord xy);

extern int color_encode(int r, int g, int b);

extern vector<int> color_decode(int color);

extern int coord_encode(coord xy);

extern coord coord_decode(int cxy);
