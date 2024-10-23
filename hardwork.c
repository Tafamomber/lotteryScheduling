#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char *argv[]) {
  int i, k;
  int tickets = atoi(argv[1]);
  settickets(tickets);

  for (i = 0; i < 500000; i++) {
    asm("nop");
    if (i % 10000 == 0)
      k++;
  }

  printf(1, "Process with %d tickets finished\n", tickets);
  exit();
}
