using namespace std;

#include <algorithm>
#include <unordered_map>
#include <cstdio>
#include <cstring>
#include "utils.h"

#define W 256
#define H 128
#define oo (1<<29)
#define NoColors 32

int pixels[1024][1024];
int colors[W * H + 1];
unordered_map<int, bool> available;

void draw() {
  for (int r = 0; r < 256; r += (256 / NoColors))
    for (int g = 0; g < 256; g += (256 / NoColors))
      for (int b = 0; b < 256; b += (256 / NoColors))
        colors[++colors[0]] = color_encode(r, g, b);
  //random_shuffle(colors+1, colors+W*H+1);

  for (int i = 0; i <= W * H; ++i) {
    int color = colors[i];
    coord best;

    if (!available.size()) {
      best = make_pair(W / 2, H / 2);
    }
    else {
      int score = oo;
      for (unordered_map<int, bool>::iterator it = available.begin(); 
          it != available.end(); ++it) {
        
        coord xy = coord_decode((*it).first);
        int _score = color_match(pixels, color, xy);
        if (_score < score) {
          score = _score;
          best = xy;
        }
      }
    }

    //vector<int> rgb = color_decode(color);
    //printf ("[%d,%d,%d] in %d %d at %d\n", rgb[0], rgb[1], rgb[2], best.first, best.second, i);
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
}

void output() {
  FILE *fout = fopen("image.ppm", "wb");

  char header[64]={0};
  int mag = 10;
  sprintf(header, "P6 %d %d 255 ", W * mag, H * mag);
  fwrite(header, sizeof(char), strlen(header), fout);

  for (int i = 0; i < W * mag; ++i) {
    for (int j = 0; j < H * mag; ++j) {
      vector<int> rgb = color_decode(pixels[i / mag][j / mag]);
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
  output();
  return 0;
}
