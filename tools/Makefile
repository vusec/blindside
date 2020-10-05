APP_DIR := $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
KMOD_DIR := $(APP_DIR)/../kmod/

all:
	(cd lib && make)
	gcc -O2 -g generate_kernel_offset_index_mapping.c -o generate_kernel_offset_index_mapping \
     -Wunused-function -Wunused-variable \
    -I$(KMOD_DIR) -L$(KMOD_DIR) -Wl,-rpath=$(KMOD_DIR) \
    -lkmod_driver

clean:
	(cd lib && make clean)
	rm -f generate_kernel_offset_index_mapping
