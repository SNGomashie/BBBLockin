#include <stdio.h>
#include <stdint.h>
#include "sine.h"

void main(void) {
  printf("First value of LUT: %X\n", sine_LUT[0]);
  printf("Size of LUT: %d\n", sizeof(sine_LUT)/2);

  FILE *f = fopen("data.txt", "w");
  if (f == NULL)
  {
    printf("Error opening file!\n");
    exit(1);
  }

  for(uint16_t i = 0; i < 1023; i++){
      fprintf(f, "%x\n", sine_LUT[i]);
  }




}

void fnSYNC(uint32_t period){
  uint16_t phase_incrementor;
  uint16_t phase_accumilator;

  phase_incrementor =

  phase_accumilator += phase_incrementor

  sine_LUT[phase_accumilator >> 8]
}
