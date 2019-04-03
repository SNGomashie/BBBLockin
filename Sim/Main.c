#include <stdio.h>
#include <stdint.h>

#define SHIFT_BITS1 4
#define SHIFT_DIVISOR1 (1 << SHIFT_BITS1)
#define SHIFT_MASK1 (SHIFT_DIVISOR1 - 1)
#define SHIFT_BITS2 9
#define SHIFT_DIVISOR2 (1 << SHIFT_BITS2)
#define SHIFT_MASK2 (SHIFT_DIVISOR2 - 1)
#define CYCLE_TIME 5
#define S_NS 1000000000

typedef uint32_t fixed_point_t;

float Fin = 100;

void main(void){
// Calculate amount of clock cycles in a period
uint64_t periodCycle = (( 1 / Fin ) * S_NS) / CYCLE_TIME;
printf("Cycles: %d\n", periodCycle);

// Calculate Q number
fixed_point_t periodCyclefix = (periodCycle << SHIFT_BITS1);
printf("Cycles as fixed point: %u\n", periodCyclefix >> SHIFT_BITS1);

// From cycles to NS
fixed_point_t periodSfix = (periodCyclefix / 1000);
printf("Period in NS: %f\n", (periodSfix >> SHIFT_BITS1) + (double)(periodSfix & SHIFT_MASK1) / SHIFT_DIVISOR1);

// From cycles to S
periodSfix *= 5;
printf("Period in S: %f\n", (periodSfix >> SHIFT_BITS1) + (double)(periodSfix & SHIFT_MASK1) / SHIFT_DIVISOR1);

periodSfix = (periodSfix << 5);
printf("Period in S(shifted): %f\n", (periodSfix >> (SHIFT_BITS2)) + (double)(periodSfix & SHIFT_MASK2) / SHIFT_DIVISOR2);

periodSfix = periodSfix / 1000000;
printf("Period in S: %f\n", (periodSfix >> (SHIFT_BITS2)) + (double)(periodSfix & SHIFT_MASK2) / SHIFT_DIVISOR2);

fixed_point_t s1 = (1 << SHIFT_BITS2);
fixed_point_t freq = s1 * (1 << SHIFT_BITS2) /  periodSfix;
printf("Frequency in Hz: %f\n", (freq >> (SHIFT_BITS2)) + (double)(freq & SHIFT_MASK2) / SHIFT_DIVISOR2);

// const fixed_point_t x_original = 503;
// fixed_point_t x_fix = x_original << SHIFT_BITS;
// x_fix += 10 << SHIFT_BITS;
// x_fix *= 3;
// x_fix /= 7;
//
// printf("FULL PRICE: %f\n", ((double)x_original + 10) * 3 / 7);
// printf("INTEGER OF PRICE: %u\n", x_fix >> SHIFT_BITS);
// printf("FRACTION OF PRICE: %f\n", (double)(x_fix & SHIFT_MASK) / SHIFT_DIVISOR);
}
