/*
 * kernel module tutorial at: http://www.tldp.org/LDP/lkmpg/2.6/html/lkmpg.html#AEN121
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <asm/io.h>
#include <linux/net.h>
#include <linux/file.h>
#include <linux/kallsyms.h>

#include <linux/hugetlb.h>
#include <asm/pgtable.h>

#include "kmod.h"
#define DEVICE_NAME "kmod"
#define SUCCESS 0

MODULE_LICENSE("GPL");

static int device_open(struct inode *inode, struct file *file)
{
  return SUCCESS;
}

static int device_release(struct inode *inode, struct file *file)
{
  return SUCCESS;
}

int pmd_huge(pmd_t pmd)
{
  return !pmd_none(pmd) &&
    (pmd_val(pmd) & (_PAGE_PRESENT|_PAGE_PSE)) != _PAGE_PRESENT;
}

#define P4D_PRESENT 0
static int virt_to_phys_through_page_table_walk(void* virt_addr, void **phys_addr, int *is_huge_page){
  pgd_t *pgd;
#if P4D_PRESENT
  p4d_t *p4d;
#endif
  pud_t *pud;
  pmd_t *pmd;
  pte_t *ptep, pte;

  struct page *page = NULL;

  pgd = pgd_offset(current->mm, (unsigned long) virt_addr);
  if (pgd_none(*pgd) || unlikely(pgd_bad(*pgd)))
    return -1;

#if P4D_PRESENT
  p4d = p4d_offset(pgd, (unsigned long) virt_addr);
  if (p4d_none(*p4d) || unlikely(p4d_bad(*p4d)))
    return -2;

  pud = pud_offset(p4d, (unsigned long) virt_addr);
#else
  pud = pud_offset(pgd, (unsigned long) virt_addr);
#endif
  if (pud_none(*pud))
    return -3;
  if (unlikely(pud_bad(*pud)))
    return -31;

  pmd = pmd_offset(pud, (unsigned long) virt_addr);
  VM_BUG_ON(pmd_trans_huge(*pmd));

  if(pmd_huge(*pmd)){
    *is_huge_page = 1;
    page = pmd_page(*pmd);
    *phys_addr = (void*) page_to_phys(page);
    return 0;
  }

  ptep = pte_offset_map(pmd, (unsigned long) virt_addr);
  if (!ptep)
    return -4;

  if(!pte_present(*ptep)){
    //printk(KERN_INFO " pte is not present. vaddr: 0x%lx\n", (unsigned long) virt_addr);
    return -5;
  }

  pte = *ptep;

  page = pte_page(pte);
  *phys_addr = (void*) page_to_phys(page);

  pte_unmap(ptep);

  return 0;
}

static long
device_ioctl(
     struct file *file,
     unsigned int ioctl_num,
     unsigned long ioctl_param)
{
  INIT_KMOD_DATA(data);
  int i, err;
  struct socket *sock;
  unsigned long probed_values = 0;

  switch (ioctl_num) {
  case IOCTL_KM_VIRT_TO_PHYS:
    if(copy_from_user((void*)&data, (void*)ioctl_param, sizeof(struct kmod_data))){
      return -ENOMEM;
    }

    //data.addr_out = (void*) virt_to_phys(data.addr_in);
    err = virt_to_phys_through_page_table_walk(data.addr_in, &data.addr_out, &data.addr_is_huge_page);
    if (data.addr_is_huge_page) {
      data.addr_out += ((unsigned long) data.addr_in) & (0x200000ul-1);
    } else {
      data.addr_out += ((unsigned long) data.addr_in) & (0x1000ul-1);
    }
    if (err) {
      printk("virt_to_phys_through_page_table_walk failed: %d\n", err);
      return err;
    }

    if(copy_to_user((void*)ioctl_param, (void*)&data, sizeof(struct kmod_data))){
      return -ENOMEM;
    }

    break;

  case IOCTL_KM_GET_KERNEL_BASE:
    if(copy_from_user((void*)&data, (void*)ioctl_param, sizeof(struct kmod_data))){
      return -ENOMEM;
    }

    data.addr_out = (void*) kallsyms_lookup_name("startup_64");

    if(copy_to_user((void*)ioctl_param, (void*)&data, sizeof(struct kmod_data))){
      return -ENOMEM;
    }

    break;

  case IOCTL_KM_GET_SPECTRE_ADDR:
    if(copy_from_user((void*)&data, (void*)ioctl_param, sizeof(struct kmod_data))){
      return -ENOMEM;
    }

    data.addr_out = (void*) kallsyms_lookup_name("vp_del_vqs");

    if(copy_to_user((void*)ioctl_param, (void*)&data, sizeof(struct kmod_data))){
      return -ENOMEM;
    }

    break;

  case IOCTL_KM_GET_PHYSMAP_BASE:
    if(copy_from_user((void*)&data, (void*)ioctl_param, sizeof(struct kmod_data))){
      return -ENOMEM;
    }

    data.addr_out = (void*) PAGE_OFFSET;

    if(copy_to_user((void*)ioctl_param, (void*)&data, sizeof(struct kmod_data))){
      return -ENOMEM;
    }

    break;

  case IOCTL_KM_GET_SOCK_ADDR:
    if(copy_from_user((void*)&data, (void*)ioctl_param, sizeof(struct kmod_data))){
      return -ENOMEM;
    }

    sock = sockfd_lookup((int) data.value, &err); // XXX handle err
    data.addr_out = sock->sk;
    sockfd_put(sock);

    if(copy_to_user((void*)ioctl_param, (void*)&data, sizeof(struct kmod_data))){
      return -ENOMEM;
    }

    break;

  case IOCTL_KM_PROBE_PAGE:
    if(copy_from_user((void*)&data, (void*)ioctl_param, sizeof(struct kmod_data))){
      return -ENOMEM;
    }

    probed_values = 1;
    for(i = 0; i < 0x1000; i += 8){
      probed_values += *(unsigned long*)(data.addr_in + i);
    }

    data.addr_out = (void*) probed_values;
    if(copy_to_user((void*)ioctl_param, (void*)&data, sizeof(struct kmod_data))){
      return -ENOMEM;
    }

    break;

  case IOCTL_KM_8BYTES_AT_ADDR:
    if(copy_from_user((void*)&data, (void*)ioctl_param, sizeof(struct kmod_data))){
      return -ENOMEM;
    }

    data.addr_out = (void*) *(unsigned long*)(data.addr_in);
    if(copy_to_user((void*)ioctl_param, (void*)&data, sizeof(struct kmod_data))){
      return -ENOMEM;
    }

    break;

  default:
    printk("kmod ioctl: no such command %d\n", ioctl_num);
    return -EINVAL;
  }

  return SUCCESS;
}

struct file_operations Fops = {
  .unlocked_ioctl = device_ioctl,
  .open = device_open,
  .release = device_release,
};

int init_module()
{
  int ret_val;

  ret_val = register_chrdev(MAJOR_NUM, DEVICE_NAME, &Fops);
  if (ret_val < 0) {
    printk(KERN_ALERT "%s failed with %d\n",
           "Sorry, registering the character device ", ret_val);
    return ret_val;
  }

  printk(KERN_INFO "%s The major device number is %d.\n",
         "Registeration is a success", MAJOR_NUM);
  printk(KERN_INFO "If you want to talk to the device driver,\n");
  printk(KERN_INFO "you'll have to create a device file. \n");
  printk(KERN_INFO "We suggest you use:\n");
  printk(KERN_INFO "mknod %s c %d 0\n", DEVICE_FILE_NAME, MAJOR_NUM);
  printk(KERN_INFO "The device file name is important, because\n");
  printk(KERN_INFO "the ioctl program assumes that's the\n");
  printk(KERN_INFO "file you'll use.\n");

  return 0;
}

void cleanup_module()
{
  unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
}
