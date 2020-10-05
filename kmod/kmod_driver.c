#include "kmod_driver.h"
#include "kmod.h"

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

int kd_fd = -1;

void kd_close_tmp() {
  close(kd_fd);
  kd_fd = -1;
}

int kd_open() {
  if (kd_fd >= 0) return 0;

  int res = open(DEVICE_FILE_NAME, 0);
  if(res < 0){
    printf("Can't open device file: %s. res: %d\n", DEVICE_FILE_NAME, res);
    return res;
  }

  kd_fd = res;

  atexit(kd_close_tmp);

  return 0;
}

int kd_close() {
  return 0;
}

int kd_get_phys_addr(void* virt_addr, void** phys_addr) {
  INIT_KMOD_DATA(data);
  int res;

  if (kd_fd < 0) {
    res = kd_open();
    if (res) return res;
  }

  data.addr_in = virt_addr;

  res = ioctl(kd_fd, IOCTL_KM_VIRT_TO_PHYS, &data);
  if(res) {
    printf("IOCTL_KM_VIRT_TO_PHYS failed. res: %d\n", res);
    return res;
  }

  //printf("%s: %p\n", __FUNCTION__, data.addr_out);

  *phys_addr = data.addr_out;

  return 0;
}

int kd_get_kimage_base(void** kimage_base) {
  INIT_KMOD_DATA(data);
  int res;

  if (kd_fd < 0) {
    res = kd_open();
    if (res) return res;
  }

  res = ioctl(kd_fd, IOCTL_KM_GET_KERNEL_BASE, &data);
  if(res) {
    printf("IOCTL_KM_GET_KERNEL_BASE failed. res: %d\n", res);
    return res;
  }

  //printf("%s: %p\n", __FUNCTION__, data.addr_out);

  *kimage_base = data.addr_out;

  return 0;
}

int kd_get_spectre_addr(void** spectre_addr) {
  INIT_KMOD_DATA(data);
  int res;

  if (kd_fd < 0) {
    res = kd_open();
    if (res) return res;
  }

  res = ioctl(kd_fd, IOCTL_KM_GET_SPECTRE_ADDR, &data);
  if(res) {
    printf("IOCTL_KM_GET_SPECTRE_ADDR failed. res: %d\n", res);
    return res;
  }

  //printf("%s: %p\n", __FUNCTION__, data.addr_out);

  *spectre_addr = data.addr_out;

  return 0;
}

int kd_get_physmap_base(void** physmap_base) {
  INIT_KMOD_DATA(data);
  int res;

  if (kd_fd < 0) {
    res = kd_open();
    if (res) return res;
  }

  res = ioctl(kd_fd, IOCTL_KM_GET_PHYSMAP_BASE, &data);
  if(res) {
    printf("IOCTL_KM_GET_PHYSMAP_BASE failed. res: %d\n", res);
    return res;
  }

  //printf("%s: %p\n", __FUNCTION__, data.addr_out);

  *physmap_base = data.addr_out;

  return 0;
}

int kd_get_sock_kaddr(int s, void** sock_kaddr) {
  INIT_KMOD_DATA(data);
  int res;

  if (kd_fd < 0) {
    res = kd_open();
    if (res) return res;
  }

  data.value = (unsigned long) s;

  res = ioctl(kd_fd, IOCTL_KM_GET_SOCK_ADDR, &data);
  if(res) {
    printf("IOCTL_KM_GET_SOCK_ADDR failed. res: %d\n", res);
    return res;
  }

  //printf("%s: %p\n", __FUNCTION__, data.addr_out);

  *sock_kaddr = data.addr_out;

  return 0;
}

int kd_probe_page(void* addr)
{
  INIT_KMOD_DATA(data);
  int res;

  if (kd_fd < 0) {
    res = kd_open();
    if (res) return res;
  }

  data.addr_in = addr;

  res = ioctl(kd_fd, IOCTL_KM_PROBE_PAGE, &data);
  if (res < 0) {
    printf("IOCTL_KM_PROBE_PAGE failed. res: %d\n", res);
    return res;
  }

  return 0;
}

int kd_get_bytes(void* addr, char* arr, unsigned long sz) {
  if (sz % 8 != 0) {
    // For now we only support reading 8 bytes in full so
    // array has to have a size of a multiple of 8.
    return -1;
  }

  INIT_KMOD_DATA(data);
  int res;

  if (kd_fd < 0) {
    res = kd_open();
    if (res) return res;
  }

  unsigned long idx;
  for (idx = 0; idx < sz; idx += 8) {
    data.addr_in = addr + idx;

    res = ioctl(kd_fd, IOCTL_KM_8BYTES_AT_ADDR, &data);
    if (res < 0) {
      printf("IOCTL_KM_PROBE_PAGE failed. res: %d\n", res);
      return res;
    }

    *(unsigned long*) &arr[idx] = (unsigned long) data.addr_out;
  }

  return 0;
}
