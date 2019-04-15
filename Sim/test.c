#include <stdio.h>
#include <stdint.h>

#define MASK (1 << 31)

void main(void){

int32_t a = -10;
uint32_t b = 5;
int32_t c = 0;

if(a & MASK){
  a = ~a + 1;
  c = a / b;
  c = ~c + 1;
  printf("%d\n", c);
} else {
  c = a / b;
  printf("%d\n", c);
}

}
