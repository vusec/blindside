#include "kmod_driver.h"

#include <stdio.h>

int main() {
  kd_open();

  void * addr = 0x0;

  kd_get_kimage_base(&addr);
  printf("kimage base  = 0x%lx\n", (unsigned long) addr);

  kd_get_physmap_base(&addr);
  printf("physmap base = 0x%lx\n", (unsigned long) addr);

  kd_close();

  return 0;
}
