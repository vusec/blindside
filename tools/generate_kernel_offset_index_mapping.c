#include <kmod_driver.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define NUM_CACHELINES 64
#define MIN_CACHELINES 32

int main(int argc, char** argv) {
  if (argc < 3) {
    printf("Usage: %s <.rodata offset from .text> <.rodata size>\n", argv[0]);
    return 1;
  }

  unsigned long kernel_base;
  unsigned long rodata_addr;
  unsigned long rodata_size;

  kd_get_kimage_base((void**) &kernel_base);
  rodata_addr = kernel_base + strtoull(argv[1], NULL, 16);
  rodata_size = strtoull(argv[2], NULL, 16);

  printf("kernel_base = 0x%lx\n", kernel_base);
  printf("rodata_addr = 0x%lx\n", rodata_addr);
  printf("rodata_size = 0x%lx\n", rodata_size);

  unsigned long kernel_offsets[NUM_CACHELINES]; // key = cl_idx
  memset(kernel_offsets, 0, sizeof(kernel_offsets));
  int found_cachelines = 0;

  unsigned long curr_addr = rodata_addr;
  while (found_cachelines < MIN_CACHELINES && curr_addr+8 < rodata_addr + rodata_size) {
    unsigned long val_at_addr;
    kd_get_bytes((void*)curr_addr, (char*) &val_at_addr, 8);

    unsigned long cl_idx = val_at_addr / 8;

    if (cl_idx < NUM_CACHELINES && kernel_offsets[cl_idx] == 0) {
      kernel_offsets[cl_idx] = curr_addr - kernel_base;
      found_cachelines += 1;
    }

    curr_addr += 8;
  }

  FILE *f = fopen("kernel_offset_index_mapping.h", "wb");
  fprintf(f, "#ifndef _KERNEL_OFFSET_INDEX_MAPPING_H_\n");
  fprintf(f, "#define _KERNEL_OFFSET_INDEX_MAPPING_H_\n\n");
  fprintf(f, "#define NUM_KERNEL_OFFSET_MAPPINGS %d\n", NUM_CACHELINES);
  fprintf(f, "unsigned int kernel_offset_index_mapping[NUM_KERNEL_OFFSET_MAPPINGS] =\n");
  fprintf(f, "{\n");

  int i;
  for (i = 0; i < NUM_CACHELINES; i++) {
    fprintf(f, "  [%2d] = 0x%lx,\n", i, kernel_offsets[i]);
  }

  fprintf(f, "};\n\n");
  fprintf(f, "#endif\n");
  fclose(f);

  return 0;
}
