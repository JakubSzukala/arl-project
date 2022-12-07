# arl-project

This repository contains source files for extending functionalities of Crazyflie
drone and Flapper Drone.

# WARNING

This is not complete instruction on how to handle Crazyflie drones. This is more
of a reminder for people familiar with the procedure and for pretty specific case.
Please use official documentation if it is Your first encounter with the
drone / build procedure.

#### Setup

First clone the [crazyflie-firmware](https://github.com/bitcraze/crazyflie-firmware) repository **with all submodules**:

```bash
$ git clone --recursive https://github.com/bitcraze/crazyflie-firmware.git
```
Source files from this repository are used in the build process of our application,
so it is crucial to have them. Our application is built into one binary together
with FreeRTOS.

Install toolchain:

```bash
$ sudo apt-get install make gcc-arm-none-eabi
```

#### Building and flashing

The build is performed as OOT (Out Of Tree) build. This procedure needs special
setup. For Detailed project setup refer to the
[documentation](https://www.bitcraze.io/documentation/repository/crazyflie-firmware/master/userguides/app_layer/)
of OOT build from Crazyflie to familiarize Yourself with project tree structure.

When building examples from this repository, all of them are already set up in
correct way so You can immediatly go to build procedure.

First, export CRAZYFLIE\_BASE environment variable which will point to the root
of crazyflie-firmware directory. It is used by build system to compile all the
basic Crazyflie firmware (FreeRTOS, drivers etc) and then on top of it Your
application. For example:

```bash
$ export CRAZYFLIE_BASE=/path/to/your/crazyflie-firmware
```

Now You should be ready to build the application. Go to Your application's root
directory, where Makefile is stored, and run:

```bash
$ make clean # this can throw error on first build, ignore it
$ make -j12
```

Application should be compiled now. To flash it to the drone, You will need to follow
flash and build instructions from Bitcraze. Bellow is presented simple summary.

Follow these steps:
* Connect [Crazyradio](https://www.bitcraze.io/products/crazyradio-pa/)
* Run [cfclient](https://www.bitcraze.io/documentation/repository/crazyflie-clients-python/master/userguides/userguide_client/)
* Turn off the drone with a power button
* Run `make cload` from application root directory with Makefile to start flashing procedure
* You will then have 10 sec to get drone to bootloader mode
* To do that, hold the same power button for 3 sec, until blue on-board diodes start to blink
* Now, the flash procedure shold begin, it will be indicated in terminal

**When in any doubt, refer to the official Crazyflie [documentation](https://www.bitcraze.io/documentation/repository/crazyflie-firmware/master/building-and-flashing/build/)**

#### Details on building the application layer

In general, the build is pretty straight forward and well explained in the
[official instruction](https://www.bitcraze.io/documentation/repository/crazyflie-firmware/master/userguides/app_layer/).

We will need to modify it though, so it will suit the Flapper Drone.

The build of application layer will build both the RTOS and the application layer
on top of it and merge them into single binary.

Crucial files in the build process (structure of [examples/app_hello_world](https://github.com/bitcraze/crazyflie-firmware/tree/master/examples/app_hello_world)):
* app-config - overrides settings from .config file
* Kbuild - specifies what files / dirs will be included in the build process
* Makefile - it will execute main crazyflie-firmware Makefile for OOT build `include $(CRAZYFLIE_BASE)/tools/make/oot.mk` (Out Of Tree) but with custom configuration passed as
`OOT_CONFIG := $(PWD)/app-config`. Little bit strange thing about this is that this main OOT Makefile has hardcoded `alldefconfig` as a defconfig, which will build for cf2 with all options enabled. What we need to do here to build for flapper, is to override specific config options in app-config to make it suitable for flapper, for example:
    ```
    CONFIG_APP_ENABLE=y
    CONFIG_APP_PRIORITY=1
    CONFIG_APP_STACKSIZE=350
    CONFIG_PLATFORM_FLAPPER=y
    CONFIG_DECK_FORCE="bcUSD:bcLedRing:bcFlapperDeck"
    CONFIG_DECK_FLAPPER=y
    CONFIG_POWER_DISTRIBUTION_FLAPPER=y
    ```
When we perofrm build for alldefconfig after compilation we get:
```
Build for the cf2!
```

After modifying the app-config for Flapper we should get:
```
Build for the flapper!
```

# TODOs

- [ ] Compile in the FreeRTOS
    - [ ] Test crazyflie application-layer program with it
- [ ] Compile in Drivers from vendor/CMSIS/CMSIS/DoxyGen/Driver/src/
    - [ ] Check their source and version
- [ ] Find the way to keep the libs taken from crazyflie-firmware in sync with this repo
    - [ ] Git hooks - scripts that will run events in git repo [link to examples](https://github.com/aitemr/awesome-git-hooks)
- [ ] Make a CI that will compile binaries in cloud
- [ ] Find a way to upload binaries to the crazyflie, other way than make cload

- Prepare a simple library, with basic I2C driver (or even simple GPIO driver)
- Prove that it can be compiled into the FreeRTOS
- Setup a test suite with Renode and vanilla FreeRTOS
- Compile in the driver into vanilla FreeRTOS
- Test it with renode with some super basic peripheral like temp or humidity
sensors (included in Renode already)

- Make renode simulation for STM32F4xx
- Prepare project in cubemx, include STM32F4xx_StdPeriph_Driver in version **1.8** https://www.st.com/en/embedded-software/stsw-stm32065.html#overview

