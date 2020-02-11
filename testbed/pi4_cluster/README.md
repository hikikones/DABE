# Raspberry Pi 4 Cluster with web10g and TEACUP

## Web10g

### Building rpi kernel 5.5.y with web10g support

#### Preparing the kernel

Get the kernel for RPI4:

```
git clone --depth=1 --single-branch --branch rpi-5.5.y https://github.com/raspberrypi/linux.git rpi
```

Move into the cloned `rpi` folder with `cd rpi`, and patch the web10g changes onto the kernel:

```
patch -p1 < web10g-5.5.1.patch
```

Add web10g feature to the kernel configuration:

```
echo 'CONFIG_TCP_ESTATS=y' >> arch/arm/configs/bcm2711_defconfig
```

Next, it is time to start building the kernel. You can choose to either build it locally on the RPI4 itself, or cross-compile it on another Linux host (such as Ubuntu). But before that, install the tools needed for building:

```
sudo apt install -y bc bison flex libssl-dev make gcc-arm-linux-gnueabihf
```

#### Building and installing the kernel locally

Build the kernel locally as follows:

```
KERNEL=kernel7l
make bcm2711_defconfig
make -s -j 4 zImage modules dtbs
```

When done, copy over the kernel, modules and device tree blobs as follows:

```
sudo make modules_install
sudo cp arch/arm/boot/dts/*.dtb /boot/
sudo cp arch/arm/boot/dts/overlays/*.dtb* /boot/overlays/
sudo cp arch/arm/boot/dts/overlays/README /boot/overlays/
sudo cp arch/arm/boot/zImage /boot/$KERNEL.img
```

Reboot.

#### Cross-compiling and installing the kernel on the SD card

Cross-compile the kernel for RPI4 as follows:

```
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- bcm2711_defconfig
make -s ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -j6 zImage modules dtbs
```

When done, copy over the kernel, modules and device tree blobs to the SD card as follows:

```
# Mount the sdcard partitions
mkdir -p /media/rpi-boot
mkdir -p /media/rpi-rootfs
sudo mount /dev/mmcblk0p1 /media/rpi-boot
sudo mount /dev/mmcblk0p2 /media/rpi-rootfs

# Install the modules to the rootfs
sudo make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- INSTALL_MOD_PATH=/media/rpi-rootfs modules_install

# Copy the kernel and the device tree files to the sdcard
sudo cp arch/arm/boot/zImage /media/rpi-boot/rpi-5.5.y-web10g.img
sudo cp arch/arm/boot/dts/*.dtb /media/rpi-boot

# Inform to the firmware which kernel image and device tree to use
echo "rpi-5.5.y-web10g.img" >> /media/rpi-boot/config.txt
echo "device_tree=bcm2711-rpi-4-b.dtb" >> /media/rpi-boot/config.txt

# Unmount
sudo umount /media/rpi-boot
sudo umount /media/rpi-rootfs
```

Remove the SD card, plug it in RPI4 and boot.

### Building the web10g kernel module (dlkm)

Get the kernel module source:

```
git clone --depth=1 https://github.com/rapier1/web10g-dlkm.git
```

Move into the cloned folder with `cd web10g-dlkm`, and apply the tiny patch to make it compile for RPI4 ARM architecture:

```
patch -p1 < web10g-dlkm-arm.patch
```

All the patch does is to remove the explicit `u64-bit` division cast. Although RPI4 does support 64-bit, most of the userland tools for it are still on 32-bit (even the official OS). If we do not remove it, the build results in `WARNING: "__aeabi_ldivmod" undefined`. Now start the kernel module build and copy the resulting kernel object to the approriate kernel module folder as follows:

```
make
sudo cp tcp_estats_nl.ko /lib/modules/$(uname -r)/kernel/net/ipv4/
```

### Building the web10g userland tools

Get the web10g userland tools source:

```
git clone --depth=1 https://github.com/rapier1/web10g-userland.git
```

Move into the cloned folder with `cd web10g-userland`, and build the tools as follows:

```
sudo apt install -y autotools-dev automake libtool pkg-config make g++ gcc libmnl-dev
./autogen.sh
./configure
make
sudo make install
```

If any complaint about `autoconf` appears, run `autogen` and `configure` again, followed by `make clean`. Then build again.

### Verify web10g by using it

After installing all of the components of web10g you will need to load the module and instantiate the collection process with the following commands:

```
sudo depmod -a
sudo modprobe tcp_estats_nl
sudo sysctl -w net.ipv4.tcp_estats=127
sudo ldconfig
```

As long as no errors are shown, you are now good to go.

## TEACUP

TODO