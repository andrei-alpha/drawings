using namespace std;

#include <algorithm>
#include <bitset>
#include <cstdio>
#include <cstring>
#include <pthread.h>
#include <unordered_map>
#include "microseconds.h"
#include "utils.h"

#define W 128
#define H 64
#define Magnify 1
#define oo (1<<29)
#define NoColors 32
#define CORES 2

int pixels[1024][1024];
int colors[64000]; // W * H + 1];
int available[W * H + 1];
vector< bitset<H + 1> > is_available(W + 1);

typedef struct worker_struct {bool is_work, finish; int start, end;
  int color, score; int best_ind; coord best; } worker_struct;
worker_struct workers[CORES];

void *color_match_thread(void *arg) {
  int ind = *(int *) arg; 
  //fprintf (stderr, "Worker %d has started\n", ind);

  while (!workers[ind].finish || workers[ind].is_work) {
    //fprintf (stderr, "wtf %d %d\n", ind, workers[ind].is_work);    
    if (!workers[ind].is_work) {
      micro_sleep(1);
      continue;
    }

    //fprintf (stderr, "Worker %d started work %d %d\n", ind, workers[ind].start, workers[ind].end);
    workers[ind].score = oo;
    
    for (int i = workers[ind].start; i < workers[ind].end; ++i) {
      if (available[i] == -1)
        continue;

      coord xy = coord_decode(available[i]);
      int _score = color_match(pixels, workers[ind].color, xy);

      if (_score < workers[ind].score) {
        workers[ind].score = _score;
        workers[ind].best = xy;
        workers[ind].best_ind = i;
      }
    }
    //fprintf (stderr, "Worker %d finished work %d %d\n", ind, coord_encode(workers[ind].best), workers[ind].best_ind);

    workers[ind].is_work = false;
  }

  //fprintf (stderr, "Worker %d has finished\n", ind);
}

void spawn_workers() {
  pthread_t threads[CORES];
  for (int i = 0; i < CORES - 1; ++i) {
    int *ind = (int *) malloc(1);
    *ind = i;
    pthread_create(&threads[i], NULL, color_match_thread, (void*) ind);
  }
}

void stop_workers() {
  for (int i = 0; i < CORES; ++i) {
    workers[i].finish = true;
  }
}

void render_image() {
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

  s2 = stamp();
  printf("%g s\n", (s2 - s1)/1e9);
  s1 = stamp();

  for (int iter = 0; iter < W * H; ++iter) {
    int color = colors[iter];
    coord best;
    int best_ind = -1;

    if (!available[0]) {
      best = make_pair(W / 2, H / 2);
    }
    else {
      long ss1 = stamp();
      
      int start = 1;
      int end = available[0] + 1;
      int iter_step = (end - start + 1) / CORES;
      for (int t = 0; t < CORES; ++t) {
        int _start = start + (iter_step * t);
        int _end = (t == CORES - 1 ? end : _start + iter_step - 1);

        workers[t].color = color;
        workers[t].start = _start;
        workers[t].end = _end;
        workers[t].is_work = true;

        // The last worker is the main thread
        if (t == CORES - 1) {
          int *ind = (int *) malloc(1);
          *ind = t;
          workers[t].finish = true;
          color_match_thread((void*) ind);
        }
      }

      int score = oo;
      for (int t = 0; t < CORES; ++t) {
        while (workers[t].is_work)
          micro_sleep(1);

        if (workers[t].score < score) {
          score = workers[t].score;
          best = workers[t].best;
          best_ind = workers[t].best_ind;
        }
      }

      //for (int j = 1; j <= available[0]; ++j)
      //  printf("%d ", available[j]);
      //printf("\n");
      //printf ("%d score = %d %d\n", coord_encode(best), score, best_ind);
      //if (!best_ind)
      //  return;

      long ss2 = stamp();
      acum += ss2 - ss1;
    }

    //if (!(i & 31))
    //  printf (" Rendering image... %d%c\r", i / (W * H / 100), '%');

    //vector<int> rgb = color_decode(color);
    //printf ("[%d,%d,%d] in %d %d\n", rgb[0], rgb[1], rgb[2], best.first, best.second);
    
    pixels[best.first][best.second] = color;
    is_available[best.first][best.second] = false;
    available[best_ind] = -1;
    vector<coord> neighbours = get_neighbours(best);

    for (vector<coord>::iterator it = neighbours.begin(); 
        it != neighbours.end(); ++it) {
      
      coord xy = *it;
      if (!pixels[xy.first][xy.second] && !is_available[xy.first][xy.second]) {
        available[++available[0]] = coord_encode(xy);
        is_available[xy.first][xy.second] = true;
      }
    }
  }

  s2 = stamp();
  printf(" Rendering image... %g s\n", (s2 - s1)/1e9);
  printf(" Sorting time... %g s\n", acum/1e9);
}

void output() {
  FILE *fout = fopen("image.ppm", "wb");

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
  spawn_workers();
  render_image();
  stop_workers();

  long s1, s2;
  s1 = stamp();
  printf(" Writing image file... ");
  output();
  s2 = stamp();
  printf("%g s\n", (s2 - s1)/1e9);

  return 0;
}
