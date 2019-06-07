#include <stdio.h>
#include <stdint.h>
#include <math.h>

void main(void) {
  int16_t sine_LUT = 0;
  FILE *f = fopen("data.txt", "w");

  for(uint16_t i = 0; i < 1024; i++){
      sine_LUT = (int16_t)32767 * sin(2 * M_PI * i / 1024);
      fprintf(f, "%#04x, ", (sine_LUT & 0xFFFF));
  }

}
