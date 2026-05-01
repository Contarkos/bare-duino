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

```shell
# Download the archive and copy it in the /opt folder
sudo cp Downloads/avr-gcc-8.5.0-1-x64-linux.tar.xz /opt/
cd /opt

# Untar it directly at the root
sudo tar -xz avr-gcc-8.5.0-1-x64-linux.tar.xz
```

* Go into the folder and run the script **permissions.sh** with sudo rights

```shell
cd ./avr-gcc-8.5.0-1-x64-linux

sudo chmod +x permissions.sh
sudo ./permissions.sh
```

* That's it ! Your toolchain is ready !
* This toolchain may be able to compile your code but you cannot use this project to install the binary on WSL2. You can however use the Arduino IDE to upload your HEX file directly. 

## Compilation and installation

* Create the binary
    * Clone this repository into your workspace
    * Run make (parallel compilation not supported at the moment)

```shell
# In your own workspace
git clone https://github.com/Contarkos/bare-duino.git

# This will build everything you need in the folder env/MAIN/bin
make
```

* Upload the binary
    * Install the avrdude tool.
    * Find the COM port of your Arduino. It is usually found under ```/dev/ttyACM0```.
    * Replace the name of the port in the makefile
    * Launch ```make install```

## About

We will try to implement basic features like a serial console or drivers for specific devices like a RF transmitter/receiver or bluetooth components.

## Using Docker to build

You can build a docker image that will help you build the binaries.

### Install docker, the proper way

You can find the official instructions [here](https://docs.docker.com/engine/install/ "Official instructions for Docker Engine"). You need the docker compose and the docker buildx plugins so install them as well if it is not already done.

```shell
sudo apt update
sudo apt install docker-compose-plugin docker-buildx-plugin
```

Do not forget to add your account to the docker group so that you can run the docker CLI without sudo !
```shell
# You will need to logout and login again for the change to be applied
sudo usermod -aG docker <your_username>
```

### Build the image and tag it

For the compilation to work, you need to build the image used in the docker-compose.yaml and then tag it with the proper name.

```shell
# Move to the repo folder
cd /path/to/bare-duino

# Build the image and tag it in the same process
docker buildx build . --tag avr_build:0.0.1
```

### Compile your binary with your image

Now that your image is ready, you can use it to compile your binary.

```shell
# Export the following variables so that your build is owned by your account
export USERID=$(id -u)
export GROUPID=$(id -g)

docker compose up clean
docker compose up build
```

### Add some extra configuration to avoid doing it all the time

To avoid exporting the variables each time you open a new terminal, you can add these lines to your `.bashrc` file :

```shell
# No need to export them manually now ! 
export USERID=$(id -u)
export GROUPID=$(id -g)

# Now the command to build is simply "dcu build"
alias dcu='docker compose up'
```

You can build custom targets in the container by calling 
```shell
CUSTOM_CMD=<target> dcu custom
```

# Accessing Arduino serial console on WSL2

To access the Serial Console on WSL2, you need to redirect the device from Windows to the virtual environment.

All the following instructions have been derived from this blog [here](https://learn.microsoft.com/en-us/windows/wsl/connect-usb).

1) Install and configure usbipd-win

Install the latest release of the `usbipd-win` tool. You can find it under this [link](https://github.com/dorssel/usbipd-win/releases). Pretty straightforward, no gimmicks.

Then, open a powershell console with admin rights and list all USB devices :

```powershell
usbipd list
```

It should give you a list of devices currently connected to your computer :

```powershell
Connected:
BUSID  VID:PID    DEVICE                                                        STATE
1-12   1bcf:2b95  Integrated Webcam                                             Not shared
2-1    2341:0062  Arduino Uno Mini (COM3)                                       Not shared
```

The important information here is the `BUSID` of our device. In this case, it is `2-1`. We will use that value to bind the device with the following command :

```powershell
usbipd bind --busid 2-1
```

On another console, or in VS Code, start WSL.

```batch
wsl
```

You can now attach the device to your WSL instance from the Admin Powershell console :

```powershell
usbipd attach --wsl --busid 2-1
```

2) Install your binary on the Arduino

Your serial console should now be listed as `/dev/ttyACM0`. You do not need to execute docker to flash your file on the board. Just type : 

```bash
make install
```

3) Listen to the flow

On WSL, you can install `screen` to access the console.

```bash
screen /dev/ttyACM0 9600
```

To quit a session from `screen`, you can type `Ctrl-A + \` or `Ctrl-A + k`.

## Warning

If you bind the serial console to WSL, it will not be available in Windows anymore. To restore the configuration you need to detach the device in an Admin Powershell console :

```powershell
usbipd detach --busid 2-1
```