## Show kernel image headers

1. Extract vmlinux from `vmlinuz`: ``sudo ./extract-vmlinux /boot/vmlinuz-`uname -r` > vmlinux``
2. Show section headers: `readelf -S vmlinux`

## Generate code\_bytes\_table.h

1. Get code (`.text`) section size from kernel image headers
2. Dump code bytes in at least 2 boots:
  a. Install kmod: `make -C ../kmod install`
  b. Get kernel base: ``echo $(echo 0x`sudo grep ffffffff /proc/kallsyms | head -1` | awk '{print $1}')``
  c. Get kernel bytes: `./print_kernel_bytes.sh <kernel-base> <code-size> > code-bytes-<boot_nr>.out`
  d. Separate bytes on newlines: `sed 's/ /\n/g' code-bytes-<boot_nr>.out > code-bytes-<boot_nr>.nl.out`
3. Filter constant bytes: `./filter_constant_code_bytes.py code-bytes-1.nl.out code-bytes-2.nl.out > constant-code-bytes.out`
4. Generate the header file: `./get_byte_sequence_stats.py constant-code-bytes.out`

## Generate kernel\_offset\_index\_mapping.h

1. Get `.rodata` section offset from the kernel base. You can get this by subtracting `.text` section's address from `.rodata` section's address retrieved from the kernel image section headers.
2. Get `.rodata` section size from the section headers.
3. Generate header file: `./generate_kernel_offset_index_mapping <.rodata offset from the kernel base> <.rodata size>`
