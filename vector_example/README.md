# Vector example

D1 contains a T-head C906 cores, which supports RISC-V Vector Spec 0.7.1.
Currently, the native toolchain doesn't support RISC-V Vector extension yet.
This demo shows how to cross compiling vector codes.

## Install T-head cross compiling toolchain

From [T-head website](https://occ.t-head.cn/community/download?id=3902100504121253888), download riscv64-linux-x86_64-20210329.tar.gz or other version.

Optional, you can download [qemu provided by T-head](https://occ.t-head.cn/community/download?id=636946310057951232). Please download csky-qemu-x86_64-Ubuntu-16.04-20210202-1445.tar.

Install the toolchian and set the `PATH`.

## Compile the example

Please read `Makefile` for `-march`, `-mabi`.

`make`

Qemu run the example on x86
`make qemu_run` 

## Run on the boards

Cp the binary to the boards. Run it.
