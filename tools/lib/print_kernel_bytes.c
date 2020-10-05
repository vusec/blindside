#include "../../kmod/kmod.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>

int file_desc;

int print_kernel_bytes(void* addr, unsigned long len, int print_raw_bytes)
{
  int ret_val;
  int print_bytes = 0;
  int i;
  INIT_KMOD_DATA(data);

  while(len > 0){
    data.addr_in = addr;
    ret_val = ioctl(file_desc, IOCTL_KM_8BYTES_AT_ADDR, &data);

    if (ret_val < 0) {
      printf("IOCTL_KM_8BYTES_AT_ADDR failed:%d\n", ret_val);
      return -1;
    }
    
    print_bytes = 8;
    if(len < print_bytes) print_bytes = len;
    
    unsigned long values = (unsigned long) data.addr_out;
    for(i = 0; i < print_bytes; i++){
      if(print_raw_bytes) {
        printf("%c", (int) (values % 0x100));
      } else {
        printf("%02lX ", values % 0x100);
      }
      values = values >> 8;
    }

    len -= print_bytes;
    addr += print_bytes;
  }

  if(!print_raw_bytes) {
    printf("\n");
  }

  return 0;
}

int kmod_open(){
  file_desc = open(DEVICE_FILE_NAME, 0);
  if(file_desc < 0){
    printf("Can't open device file: %s\n", DEVICE_FILE_NAME);
    return -1;
  }
  return 0;
}

int kmod_close(){
  return close(file_desc);
}

/* 
 * Main - Call the ioctl functions 
 */
int main(int argc, char **argv)
{
  void* virt_addr;

  if(argc < 3 || argc > 4){
    fprintf(stderr, "Provide args: <kernel-virtual-address> <#bytes> (<raw>)\n");
    exit(1);
  }

  int print_raw_bytes = 0;
  if(argc == 4) {
    if(strcmp(argv[3], "raw") == 0) {
      print_raw_bytes = 1;
    }
  }

  if (kmod_open()) {
    fprintf(stderr,"kmod_open() failed\n");
    exit(1);
  }

  int offset = 8;
  if (argv[1][0] == '0' && argv[1][1] == 'x'){
    offset = 10;
  }

  unsigned long rhs = strtol(argv[1] + offset, NULL, 16);
  argv[1][offset] = '\0';
  unsigned long lhs = strtol(argv[1], NULL, 16);

  unsigned long len = strtol(argv[2], NULL, 10);

  virt_addr = (void*) ((lhs << 32) + rhs);

  if (print_kernel_bytes(virt_addr, len, print_raw_bytes)) {
    fprintf(stderr,"print_kernel_bytes() failed\n");
    exit(1);
  }

  kmod_close();

  return 0;
}
