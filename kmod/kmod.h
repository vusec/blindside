#ifndef KMOD_H
#define KMOD_H

#include <linux/ioctl.h>

#define MAJOR_NUM 144

#define IOCTL_KM_VIRT_TO_PHYS       _IOWR(MAJOR_NUM, 0, struct kmod_data *)
#define IOCTL_KM_GET_KERNEL_BASE    _IOWR(MAJOR_NUM, 1, struct kmod_data *)
#define IOCTL_KM_GET_SPECTRE_ADDR   _IOWR(MAJOR_NUM, 2, struct kmod_data *)
#define IOCTL_KM_GET_PHYSMAP_BASE   _IOWR(MAJOR_NUM, 3, struct kmod_data *)
#define IOCTL_KM_GET_SOCK_ADDR      _IOWR(MAJOR_NUM, 4, struct kmod_data *)
#define IOCTL_KM_PROBE_PAGE         _IOWR(MAJOR_NUM, 5, struct kmod_data *)
#define IOCTL_KM_8BYTES_AT_ADDR     _IOWR(MAJOR_NUM, 6, struct kmod_data *)

#define DEVICE_FILE_NAME "/dev/kmod_dev"

struct kmod_data {
  void *addr_in;
  void *addr_out;
  int addr_is_huge_page;
  unsigned long value;
};

#define INIT_KMOD_DATA(N) struct kmod_data N = {.addr_in = NULL, .addr_out = NULL, .addr_is_huge_page = 0, .value = 0}

#endif /* KMOD_H */
