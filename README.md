# Disclaimer

USE AT YOUR OWN RISK! AUTHORS OF THIS CODE ARE NOT RESPONSIBLE FOR ANY HARM DONE
TO YOUR MACHINE, YOUR BELONGINGS OR YOURSELF.

Note that code in this repository exploits kernel code. In case you try the code
out on a remote machine, make sure you can reboot it remotely in case of a
crash.

# About

This repository contains the exploits showcasing the BlindSide attack described
in our [paper](https://download.vusec.net/papers/blindside_ccs20.pdf)
published at CCS'20. We branched off the exploits from google project zero's
[exploit](https://googleprojectzero.blogspot.com/2017/05/exploiting-linux-kernel-via-packet.html)
of CVE-2017-7308. For the demo videos of the exploits, check out the
[project page](https://www.vusec.net/projects/blindside/) of BlindSide.

Our proof-of-concept exploits are configured to work with the linux kernel
version 4.8.0-39-generic and are tested on a machine with Intel CPU E3-1270 v6
and 16GB of RAM.
Running it on a different kernel version (but vulnerable to CVE-2017-7308) will
require few changes to the exploits like setting the function pointer offset,
adjusting the gadget offsets or instructions, etc. Changes to the exploits are
also necessary when running on a machine with different RAM size or when running
on a CPU with a different cache size or a different cache-associativity.


# Config

Exploits can be configured to skip certain parts with macros in local\_conig.h files.

# Preparation

Execute the following commands to enable huge page tables, to set cpu governor
to performance and to locally install the evsets library:

```
$ echo 1024 | sudo tee /proc/sys/vm/nr_hugepages
$ for gov in /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor; do sudo sh -c "echo performance > $gov"; done
$ ./make_evsets_lib.sh
```
