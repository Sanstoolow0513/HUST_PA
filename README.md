# Introduction

HUST PA来自南京大学ICS2019版本，略有修改但是没有很大的变化

## 相关链接

[NJU-ICS2019](https://nju-projectn.github.io/ics-pa-gitbook/ics2019/) 使用这个是因为在很多agent工具上可以使用web相关工具调用。

[HUST-PA-WIKI](https://course.cunok.cn:52443/projects/pa/wiki) hust校园网环境下可以访问，没什么有用的信息，核心问题下面会解释

校园网外访问：

[HUST-PA-WIKI](https://course1.istratus.cn/projects/pa/wiki)

[HUST-PA-DOC](https://course1.istratus.cn/pa/doc2019/)

wiki中没有及时更新git clone地址，应使用如下地址

```bash
git clone https://course1.istratus.cn/pa/ics2019_1.git
```

tip:
- 这是通过阿里云跳转，速度可能不太稳定。
- 文档中提到的网址，要将域名从course.cunok.cn替换成course1.istratus.cn来访问。
- 如果前期已经在校园网内clone，后期要在校园网外访问，则需要手动修改代码根目录.git/config，以及Makefile，将其中的course.cunok.cn都替换成course1.istratus.cn、scripts都替换成scripts1。反之亦然。


## 仓库相关

实际上没有那么多限制，本质就是git仓库，你看下面的log就能得知其实是一个还挺有意思的设计
```bash
- git remote -v

github  git@github.com:Sanstoolow0513/HUST_PA.git (fetch)
github  git@github.com:Sanstoolow0513/HUST_PA.git (push)
hustpa  https://course.cunok.cn:52443/git/U202215561.git (fetch)
hustpa  https://course.cunok.cn:52443/git/U202215561.git (push)
origin  https://course.cunok.cn:52443/pa/ics2019.git (fetch)
origin  https://course.cunok.cn:52443/pa/ics2019.git (push)

- git push -u github --all

Enumerating objects: 2888, done.
Counting objects: 100% (2888/2888), done.
Delta compression using up to 20 threads
Compressing objects: 100% (2539/2539), done.
Writing objects: 100% (2888/2888), 2.96 MiB | 1.49 MiB/s, done.
Total 2888 (delta 1251), reused 1491 (delta 230), pack-reused 0
remote: Resolving deltas: 100% (1251/1251), done.
To github.com:Sanstoolow0513/HUST_PA.git
* [new branch]      master -> master
* [new branch]      pa0 -> pa0
* [new branch]      pa1 -> pa1
* [new branch]      pa2 -> pa2
* [new branch]      pa3 -> pa3
branch 'master' set up to track 'github/master'.
branch 'pa0' set up to track 'github/pa0'.
branch 'pa1' set up to track 'github/pa1'.
branch 'pa2' set up to track 'github/pa2'.
branch 'pa3' set up to track 'github/pa3'.
```

所以日常推送几个如下

```bash
# 推送到 GitHub
git push github
# 推送到 hustpa 服务器
git push hustpa
# 如果设置了默认追踪 （-u语法可以设置），直接 git push 即可
git push
# 推送到 hustpa pa3强制替换(提交作业可以不用make submit)
git checkout pa3
git push -f hustpa pa3
```
## start

你需要在nemu下makefile.git中配置你的id，

```bash
grep --color=auto -r "STUID" .
./Makefile:     STUID=$(STUID) STUNAME=$(STUNAME) bash -c "$$(curl -s https://course.cunok.cn:52443/pa/scripts/submit.sh)"
./Makefile:     STUID=$(STUID) STUNAME=$(STUNAME) bash -c "$$(curl -s https://course.cunok.cn:52443/pa/scripts/info.sh)"
./Makefile:     STUID=$(STUID) STUNAME=$(STUNAME) bash -c "$$(curl -s https://course.cunok.cn:52443/pa/scripts/setup.sh)"
./Makefile:     STUID=$(STUID) STUNAME=$(STUNAME) bash -c "$$(curl -s https://course.cunok.cn:52443/pa/scripts/password.sh)"
./nemu/Makefile.git:STUID = U202215561
./nemu/Makefile.git:    -@(echo "> $(1)" && echo $(STUID) && id -un && uname -a && uptime && (head -c 20 /dev/urandom | hexdump -v -e '"%02x"') && echo) | git commit -F - 
$(GITFLAGS)                                       
```

文档中有要求 username is your STUID，其实是你第一次make setup配置的时候设置的账号名字需要是学号，密码是自己设置的。

## 有关实现

HUST只要求完成riscv32架构的实现，不要做多了

## 写在最后

经过大量的vibe赤石，可以看到大概率各家模型都喂了PA的语料，大概率都能做对，但是效果怎么样就难说了，我给出一个示例提示词

```bash
这是南京大学PA课设的仓库，你需要完成PA3.1的设计，先修复klib输出不支持完全的问题，在修复当前的问题
以下是URL有关于PA3设定的：
 ` https://nju-projectn.github.io/ics-pa-gitbook/ics2019/PA3.html ` 
- pa3.1:
 ` https://nju-projectn.github.io/ics-pa-gitbook/ics2019/3.1.html ` 
 ` https://nju-projectn.github.io/ics-pa-gitbook/ics2019/3.2.html ` 
- pa3.2:
 ` https://nju-projectn.github.io/ics-pa-gitbook/ics2019/3.3.html ` 
- pa3.3:
 ` https://nju-projectn.github.io/ics-pa-gitbook/ics2019/3.4.html ` 
 ` https://nju-projectn.github.io/ics-pa-gitbook/ics2019/3.5.html ` 
 
 本实验选择riscv32架构，具体任务来说
- PA3.1:触发自陷操作，保存上下文，事件分发
- PA3.2:加载第一个用户程序，系统调用，标准输出，堆区管理
- PA3.3:简易文件系统,一切皆文件：操作系统上的IOE,批处理系统

实现过程要求：

- 你需要了解nemu的实现方法，不要想象或者猜测，可以使用py确定指令字段，反汇编等符合nemu的构建的方法。
- 你只需要给出修改的具体内容。调试等信息你可以添加
- nemu不是qemu，只是模仿但是不是特别特别完善的设计，但是也是完善的设计只是没有那么完整


现有仓库的bash运行结果如下：
make ARCH=riscv32-nemu run
# Building nanos-lite [riscv32-nemu] with AM_HOME {/home/sanstoolow/ics2019/nexus-am}
make -s -C /home/sanstoolow/ics2019/navy-apps/tests/dummy install ISA=riscv32
+ CC src/nanos.c
+ AR /home/sanstoolow/ics2019/navy-apps/libs/libos/build/libos-riscv32.a
+ LD /home/sanstoolow/ics2019/navy-apps/tests/dummy/build/dummy-riscv32
cp /home/sanstoolow/ics2019/navy-apps/fsimg/bin/dummy build/ramdisk.img
touch src/files.h
+ CC src/loader.c
+ CC src/fs.c
+ CC src/mm.c
+ CC src/device.c
+ CC src/proc.c
+ CC src/main.c
+ CC src/irq.c
+ CC src/ramdisk.c
+ CC src/syscall.c
+ AS src/raw_logo.S
+ AS src/initrd.S
# Building lib-am [riscv32-nemu]
# Building lib-klib [riscv32-nemu]
+ CC src/stdio.c
+ CC src/string.c
+ CC src/cpp.c
+ CC src/stdlib.c
+ CC src/io.c
+ CC src/int64.c
+ AR -> build/klib-riscv32-nemu.a
# Creating binary image [riscv32-nemu]
+ LD -> build/nanos-lite-riscv32-nemu.elf
riscv-none-embed-ld: warning: /home/sanstoolow/ics2019/nanos-lite/build/nanos-lite-riscv32-nemu.elf has a LOAD segment with RWX permissions
+ OBJCOPY -> build/nanos-lite-riscv32-nemu.bin
make -C /home/sanstoolow/ics2019/nemu ISA=riscv32 run ARGS="-b  -l /home/sanstoolow/ics2019/nanos-lite/build/nemu-log.txt /home/sanstoolow/ics2019/nano
s-lite/build/nanos-lite-riscv32-nemu.bin"                                                                                                              make[1]: Entering directory '/home/sanstoolow/ics2019/nemu'
Building riscv32-nemu
make -C /home/sanstoolow/ics2019/nemu/tools/qemu-diff
make[2]: Entering directory '/home/sanstoolow/ics2019/nemu/tools/qemu-diff'
+ CC src/protocol.c
+ CC src/gdb-host.c
+ CC src/diff-test.c
+ CC src/isa/riscv32/init.c
+ LD build/riscv32-qemu-so
make[2]: Leaving directory '/home/sanstoolow/ics2019/nemu/tools/qemu-diff'
./build/riscv32-nemu -b  -l /home/sanstoolow/ics2019/nanos-lite/build/nemu-log.txt /home/sanstoolow/ics2019/nanos-lite/build/nanos-lite-riscv32-nemu.bi
n -d /home/sanstoolow/ics2019/nemu/tools/qemu-diff/build/riscv32-qemu-so                                                                               [src/monitor/monitor.c,48,load_img] The image is /home/sanstoolow/ics2019/nanos-lite/build/nanos-lite-riscv32-nemu.bin
[src/memory/memory.c,16,register_pmem] Add 'pmem' at [0x80000000, 0x87ffffff]
[src/device/io/port-io.c,16,add_pio_map] Add port-io map 'serial' at [0x000003f8, 0x000003f8]
[src/device/io/mmio.c,14,add_mmio_map] Add mmio map 'serial' at [0xa10003f8, 0xa10003f8]
[src/device/io/port-io.c,16,add_pio_map] Add port-io map 'rtc' at [0x00000048, 0x0000004b]
[src/device/io/mmio.c,14,add_mmio_map] Add mmio map 'rtc' at [0xa1000048, 0xa100004b]
[src/device/io/port-io.c,16,add_pio_map] Add port-io map 'screen' at [0x00000100, 0x00000107]
[src/device/io/mmio.c,14,add_mmio_map] Add mmio map 'screen' at [0xa1000100, 0xa1000107]
[src/device/io/mmio.c,14,add_mmio_map] Add mmio map 'vmem' at [0xa0000000, 0xa007ffff]
[src/device/io/port-io.c,16,add_pio_map] Add port-io map 'keyboard' at [0x00000060, 0x00000063]
[src/device/io/mmio.c,14,add_mmio_map] Add mmio map 'keyboard' at [0xa1000060, 0xa1000063]
[src/monitor/monitor.c,20,welcome] Debug: ON
[src/monitor/monitor.c,21,welcome] If debug mode is on, A log file will be generated to record every instruction NEMU executes. This may lead to a larg
e log file. If it is not necessary, you can turn it off in include/common.h.                                                                           [src/monitor/monitor.c,28,welcome] Build time: 12:34:06, Jan 11 2026
Welcome to riscv32-NEMU!
For help, type "help"
[/home/sanstoolow/ics2019/nanos-lite/src/main.c,14,main] 'Hello World!' from Nanos-lite
[/home/sanstoolow/ics2019/nanos-lite/src/main.c,15,main] Build time: 12:34:57, Jan 11 2026
[/home/sanstoolow/ics2019/nanos-lite/src/ramdisk.c,27,init_ramdisk] ramdisk info: start = %p, end = %p, size = -2146429463 bytes
[/home/sanstoolow/ics2019/nanos-lite/src/device.c,35,init_device] Initializing devices...
[/home/sanstoolow/ics2019/nanos-lite/src/irq.c,15,init_irq] Initializing interrupt/exception handler...
[/home/sanstoolow/ics2019/nanos-lite/src/proc.c,25,init_proc] Initializing processes...
[/home/sanstoolow/ics2019/nanos-lite/src/main.c,33,main] Finish initialization
[/home/sanstoolow/ics2019/nanos-lite/src/irq.c,6,do_event] Get _EVENT_YIELD, will return to user program...
[/home/sanstoolow/ics2019/nanos-lite/src/main.c,39,main] system panic: Should not reach here
nemu: HIT BAD TRAP at pc = 0x80100558

[src/monitor/cpu-exec.c,29,monitor_statistic] total guest instructions = 13720
make[1]: Leaving directory '/home/sanstoolow/ics2019/nemu'
```


看起来还是很麻烦的，但是模型使用claude code或者antigravity总是能够完成的（使用glm4.7完成了一部分，后面opus4.5做了工作）实际上工作量没有太多，但是试错成本很高，代码阅读量太大导致进行会很困难。

AI完成任务不一定正确，尤其是这个架构的问题，提示PA2的测试其实并不完全，我其实是想写一个专门的章节的后面想想算了也没必要