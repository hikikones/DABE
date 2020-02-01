# Linux 5.5 web10g patch

Methods so far that hasn't showed much promise:

* Diff between entire mainline 5.5 source tree and web10g resulting in over a million lines of code.
* Diff on only the approriate folders that web10g changes produced too many complicated errors.
* Manually inspecting the official web10g patch for kernel 3.17 and porting it over to kernel 5.5. Surprisingly, this went quite well for the most part, until the final section. The files `tcp_input.c` and `tcp_output.c` were especially challenging due to lots of new variable names and structural changes in the code. It quickly became too complex for me to pinpoint exactly where the web10g changes should be placed in mainline kernel 5.5, so I tried to look at the recent web10g changes, but the resulting build still produced too many complicated errors.

The method that currently shows most promise follows.

### Diff on only relevant files followed by manual cleanup

The most promising method so far is done as follows. First, find which files web10g changes:

```
sudo apt install wget patchutils
wget https://www.web10g.org/images/Software/Web10G_Kernel_Patches/web10g-0.11-patch-3.17.diff.gz
gunzip web10g-0.11-patch-3.17.diff.gz
lsdiff web10g-0.11-patch-3.17.diff
```

Ignore files `tcp_estats_mib_var.h` and `tcp_estats_nl.h` as they are no longer part of the more recent web10g release (probably merged inside `tcp_estats.h` or someting). Then take the `diff` of each file (comparing linux 5.5 to web10g 5.3):

```
# Clone kernel source trees
sudo apt install git
git clone --depth=1 git://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git
git clone --depth=1 https://github.com/rapier1/web10g.git

# Begin diff procedure
diff -u linux/include/linux/tcp.h web10g/include/linux/tcp.h > web10g.patch
diff -u linux/include/net/tcp_est web10g/include/linux/tcp.h >> web10g.patch
diff -u /dev/null web10g/include/net/tcp_estats.h >> web10g.patch
diff -u linux/include/net/tcp.h web10g/include/net/tcp.h >> web10g.patch
diff -u linux/include/uapi/linux/tcp.h web10g/include/uapi/linux/tcp.h >> web10g.patch
diff -u linux/net/ipv4/Kconfig web10g/net/ipv4/Kconfig >> web10g.patch
diff -u linux/net/ipv4/Makefile web10g/net/ipv4/Makefile >> web10g.patch
diff -u linux/net/ipv4/sysctl_net_ipv4.c web10g/net/ipv4/sysctl_net_ipv4.c >> web10g.patch
diff -u linux/net/ipv4/tcp_cong.c web10g/net/ipv4/tcp_cong.c >> web10g.patch
diff -u /dev/null web10g/net/ipv4/tcp_estats.c >> web10g.patch
diff -u linux/net/ipv4/tcp_htcp.c web10g/net/ipv4/tcp_htcp.c >> web10g.patch
diff -u linux/net/ipv4/tcp_input.c web10g/net/ipv4/tcp_input.c >> web10g.patch
diff -u linux/net/ipv4/tcp_ipv4.c web10g/net/ipv4/tcp_ipv4.c >> web10g.patch
diff -u linux/net/ipv4/tcp_minisocks.c web10g/net/ipv4/tcp_minisocks.c >> web10g.patch
diff -u linux/net/ipv4/tcp_output.c web10g/net/ipv4/tcp_output.c >> web10g.patch
diff -u linux/net/ipv4/tcp_timer.c web10g/net/ipv4/tcp_timer.c >> web10g.patch
diff -u linux/net/ipv4/tcp.c web10g/net/ipv4/tcp.c >> web10g.patch
diff -u linux/net/ipv6/tcp_ipv6.c web10g/net/ipv6/tcp_ipv6.c >> web10g.patch
```

The resulting `patch` file from these commands can be found in the `web10g_raw.patch`. Then the big job of manual inspection comes where every section in the `diff` has been checked. For example, since the web10g kernel is behind the official release, it wants to "undo" a lot of the new changes, and so those sections have been removed. Also, a lot of sections require som slight modifications where a relevant web10g change is wanted, but another "undo" change to kernel 5.5 is unwanted. At last a new section was manually added due to build error `net/ipv4/tcp_ipv4.c:2805:12: error: ‘struct netns_ipv4’ has no member named ‘sysctl_estats_delay’`, which was fixed by adding the missing members to file `include/net/netns/ipv4.h`. The final cleaned up result can found in the file `web10g.patch`.

To patch the changes into kernel 5.5 and building it for RPI4, you first need to download [BCM2711_defconfig](https://raw.githubusercontent.com/raspberrypi/linux/rpi-5.5.y/arch/arm/configs/bcm2711_defconfig) for kernel 5.5 and copy it to `linux/arch/arm/configs`. Then you can start building with:

```
sudo apt install gcc-arm-linux-gnueabihf bc make flex bison libssl-dev build-essential
cd linux
patch -p1 < ~/web10g.patch
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- bcm2711_defconfig
make -s ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -j6 zImage modules dtbs
```

The build should complete with no errors. The question remains however, _does it actually work_?