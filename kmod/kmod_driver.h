#ifndef _KMOD_DRIVER_H_
#define _KMOD_DRIVER_H_

int kd_open();
int kd_close();
int kd_get_phys_addr(void* virt_addr, void** phys_addr);
int kd_get_kimage_base(void** kimage_base);
int kd_get_spectre_addr(void** spectre_addr);
int kd_get_physmap_base(void** physmap_base);
int kd_get_sock_kaddr(int s, void** sock_kaddr);
int kd_get_bytes(void* addr, char* arr, unsigned long sz);
int kd_probe_page(void* addr);

#endif
