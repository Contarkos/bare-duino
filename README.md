# bare-duino
Bare metal project for Arduino UNO board

This project aims to understand how to create programs for the Arduino UNO board outside of the Arduino toolbox.
It will primarily use the AVR GCC toolchain for compilation and the avrdude tool to upload the binaries on the board.

## Setup

This "tutorial" is for Linux users but it also works for people who use the Windows Subsystem for Linux (or wsl)

* Download AVR gcc toolchain : [AVR GCC for linux](https://www.microchip.com/en-us/tools-resources/develop/microchip-studio/gcc-compilers "Link to AVR GCC compilers")
* Install and configure the toolchain
    * Copy the archive in your /opt folder
    * Unzip it with tar :

```console
foo@bar:~$ sudo cp Downloads/avr-gcc-8.5.0-1-x64-linux.tar.xz /opt/
foo@bar:~$ cd /opt
foo@bar:/opt$ sudo tar -xz avr-gcc-8.5.0-1-x64-linux.tar.xz
```

* Go into the folder and run the script **permissions.sh** with sudo rights

```console
foo@bar:/opt$ cd /avr-gcc-8.5.0-1-x64-linux
foo@bar:/opt/avr-gcc-8.5.0-1-x64-linux$ sudo chmod +x permissions.sh
foo@bar:/opt/avr-gcc-8.5.0-1-x64-linux$ sudo ./permissions.sh
```

* That's it ! Your toolchain is ready !

## Compilation and installation

* Create the binary
    * Clone this repository into your workspace
    * Run make (parallel compilation not supported at the moment)

```console
foo@bar:~/bare-duino$ git clone https://github.com/Contarkos/bare-duino.git
foo@bar:~/bare-duino$ make
```

* Upload the binary
    * Find the COM port of your Arduino

## About

We will try to implement basic features like a serial console or drivers for specific devices like a RF transmitter/receiver or bluetooth components.
