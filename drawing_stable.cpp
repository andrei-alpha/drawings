using namespace std;

#include <algorithm>
#include <unordered_map>
#include <cstdio>
#include <cstring>
#include <cmath>
#include "microseconds.h"
#include "utils.h"

#define W 2048
#define H 1024
#define Magnify 1
#define oo (1<<29)
#define NoColors 128 

int pixels[2048][2048];
int colors[NoColors * NoColors * NoColors + 1];
unordered_map<int, bool> available;

bool comp(int c1, int c2) {
  vector<int> rgb1 = color_decode(c1);
  vector<int> rgb2 = color_decode(c2);
  double hue1 = atan2(sqrt(3) * (rgb1[1] - rgb1[2]), 2 * rgb1[0] - rgb1[1] - rgb1[2]);
  double hue2 = atan2(sqrt(3) * (rgb2[1] - rgb2[2]), 2 * rgb2[0] - rgb2[1] - rgb2[2]);
  return hue1 < hue2;
}

void draw() {
  long s1, s2, acum = 0;
  printf(" Generating colors... ");
  s1 = stamp();

  srand((int)time(0));
  for (int r = 0; r < 256; r += (256 / NoColors))
    for (int g = 0; g < 256; g += (256 / NoColors))
      for (int b = 0; b < 256; b += (256 / NoColors))
        colors[++colors[0]] = color_encode(r, g, b);
  //random_shuffle(colors+1, colors+W*H+1);
  for (int i = 0; i < colors[0] * 4; ++i)
    swap(colors[rand() % colors[0] + 1], colors[rand() % colors[0] + 1]);
  //sort(colors+1, colors+W*H+1, comp);  

  s2 = stamp();
  printf("%g s\n", (s2 - s1)/1e9);
  s1 = stamp();

  vector< pair<int, int> > starts;
  for (int i = 0; i < 10; ++i)
    starts.push_back( make_pair(rand() % W, rand() % H) );

  for (int i = 0; i < W * H; ++i) {
    int color = colors[i];
    int score = oo;
    coord best;

    if (i < starts.size()) {
      best = starts[i];
    }
    else {
      long ss1 = stamp();
      for (unordered_map<int, bool>::iterator it = available.begin(); 
          it != available.end(); ++it) {
        
        coord xy = coord_decode((*it).first);
        int _score = color_match(pixels, color, xy);
        if (_score < score) {
          score = _score;
          best = xy;
        }
      }
      long ss2 = stamp();
      acum += ss2 - ss1;
    }

    if (!(i & 31))
      printf (" Rendering image... %d%c\r", i / (W * H / 100), '%');

    //vector<int> rgb = color_decode(color);
    //printf ("[%d,%d,%d] in %d %d\n", rgb[0], rgb[1], rgb[2], best.first, best.second);
    
    pixels[best.first][best.second] = color;
    available.erase(coord_encode(best));
    vector<coord> neighbours = get_neighbours(best);

    for (vector<coord>::iterator it = neighbours.begin(); 
        it != neighbours.end(); ++it) {
      
      coord xy = *it;
      if (!pixels[xy.first][xy.second])
        available[coord_encode(xy)] = true;
    }
  }

  s2 = stamp();
  printf(" Rendering image... %g s\n", (s2 - s1)/1e9);
  printf(" Sorting time... %g s\n", acum/1e9);
}

void output() {
  char name[40]={0};
  sprintf(name, "image-%d.ppm", rand());
  FILE *fout = fopen(name, "wb");

  char header[64]={0};
  sprintf(header, "P6 %d %d 255 ", W * Magnify, H * Magnify);
  fwrite(header, sizeof(char), strlen(header), fout);

  for (int j = 0; j < H * Magnify; ++j) {
    for (int i = 0; i < W * Magnify; ++i) {
      vector<int> rgb = color_decode(pixels[i / Magnify][j / Magnify]);
      //printf ("[%d,%d,%d] in %d %d\n", rgb[0], rgb[1], rgb[2], i, j);
      fwrite(&rgb[0], 1, 1, fout);
      fwrite(&rgb[1], 1, 1, fout);
      fwrite(&rgb[2], 1, 1, fout);
    }
  }

  fclose(fout);
}

int main(int argc, char* args[]) {
  utils_init(W, H);
  draw();

  long s1, s2;
  s1 = stamp();
  printf(" Writing image file... ");
  output();
  s2 = stamp();
  printf("%g s\n", (s2 - s1)/1e9);

  return 0;
}
