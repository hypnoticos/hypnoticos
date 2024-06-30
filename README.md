# HypnoticOS: 64-bit Hobby Operating System

HypnoticOS is a 64-bit Hobby Operating System. Functionality includes:

* Memory management
* Userspace binary support
* Temporary memory file system
* Support for local/IO APIC
* SMP

![Screenshot](http://hypnoticos.org/screenshot.png)

## Web site

The web site has details about the project's plans and what's currently being worked on. See: [http://www.hypnoticos.org/](http://www.hypnoticos.org/)

The web site is also updated periodically with a generated ISO based upon a recent commit.

## Wiki

[The wiki is a a work in progress, but in the mean time it can be found here](https://github.com/hypnoticos/hypnoticos/wiki).

## Prerequisites

You will need:
* an x86_64 cross-compiler (and other build tools, like `make`)
* grub-mkrescue
* [Findutils](https://www.gnu.org/software/findutils/)

## Installing

To build:
```
make
```

To build and create an ISO:
```
make iso
```

To create an ISO which has debug mode enabled:
```
_DEBUG=1 make iso
```

To build the kernel, libc and the modules with the `-Werror` flag:
```
_WERROR=1 make iso
```

## Debug mode

When HypnoticOS is compiled with deubg mode, you can show `INFO`, `WARNING` and `HALT` output on the console if you run QEMU with the `-debugcon stdio` parameter. For example:
```
qemu-system-x86_64 -debugcon stdio -cdrom hypnoticos.iso
```

## License

This project is licensed under GNU GPLv3 - see the [LICENSE](LICENSE) file for details.
