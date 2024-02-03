#ifndef ML_RAND_H
#define ML_RAND_H

int ml_rand_seed = 70421;

void srand(int x) {
  ml_rand_seed = x;
}

short rand() {
  ml_rand_seed *= 1664525;
  ml_rand_seed += 1013904223;
  return (ml_rand_seed >> 16);
}

// oh god...

#endif
