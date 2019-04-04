#include <stdio.h>
#include <stdint.h>

const uint16_t entries = 512;
uint32_t freq = 10;
uint32_t samp_freq = 100;

uint32_t accumulator = 0;
uint32_t phase = 0;


void main(void){
  uint32_t period_cycle = 200000000 / freq;
  uint32_t samp_period_cycle = 200000000 / samp_freq;
  uint32_t norm_period = period_cycle / samp_period_cycle;
  float interval = (1 / (float)samp_freq) * 1000;
  printf("sample period in ms:%f\n", interval);
  printf("period: %d\n", period_cycle);
  printf("sample period: %d\n", samp_period_cycle);

  phase = (uint64_t)(samp_period_cycle * 4294967296) / period_cycle;
  printf("%d\n", phase);
}
