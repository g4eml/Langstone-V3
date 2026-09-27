# Langstone-V3 SDR Transceiver by Colin Durbridge G4EML

# Currently supports the Adalm Pluto or the HackRF One, the Raspberry Pi 5 and the official 7" V1 or V2 LCD Displays.

This is an experimental project to produce a simple VHF, UHF and Microwave SDR Transceiver operating on SSB CW and FM.

It was inspired by the very successful Portsdown Amateur Television system created by the British Amateur Television Club.

To install the software on a raspberry pi please follow the instructions further down the page. 

**More information can also be found on the UK Microwave group wiki at https://wiki.microwavers.org.uk/Langstone_Project**

Currently only the following hardware is supported:-

- Raspberry Pi 5 (Pi 4 is not officially supported but is reported to work OK with the Pluto. Not with the HackRF)

- Official Original Raspberry Pi 7" 800 x 480 Version 1 touchscreen or Version 2 7" 1280 x 720 touchscreen. 

- RPi5 to Touchscreen flat cable. (this may need to be purchased seperately as the cable supplied with the V1 Touch Screen is not suitable)

- Adalm Pluto or HackRF One SDR Modules. 

- USB Audio module. Connected to loudspeaker or headphones and microphone. Devices using the CM108 Chip can also use the Volume Down button as a PTT input. 
 
- USB Scroll mouse
 
- PTT via Raspberry Pi GPIO 17 (pin 11). This needs a pull up resistor to 3.3V. Grounding this pin will switch to Transmit.

- CW Key is via Raspberry Pi GPIO 18 (pin 12). This needs a pull up resistor to 3.3V. Grounding this pin will key the transmitter. 

- Tx Output is via Raspberry Pi GPIO 21 (pin 40). This output goes high when the Langstone is transmitting. This can be used to switch antenna relays and amplifiers. (100ms delay included for sequencing)

- 8 Band select Outputs on GPIO 1 (pin 28), GPIO 19 (pin 35), GPIO 4 (pin 7), GPIO 25 (pin 22), GPIO 22 (pin 16), GPIO 24 (pin 18), GPIO 10 (pin 19), and GPIO 9 (Pin 21). These can be used to select external filters, amplifiers or Transverters. The state of these outputs is defined using the Band Bits setting. 

- On the Adalm Pluto the TX output and first three of the Band Select outputs are also available on the Internal Pluto GPO connector. GPO0 is the Tx Output, GPO1-3 are the Band Select outputs.The main use for these is for when the Pluto is remotely mounted. Care must be taken as these pins are low voltage. They will need to be buffered before use. 

To build a complete functional transceiver you will need to add suitable filters, preamplifiers and power amplifiers to the SDR Module. 

All control is done using the touchscreen and mouse.

Tuning uses the mouse scrollwheel. The mouse left and right buttons select the tuning step. The centre button is used for the CW key.  Mouse movement is not used.

A mouse is used to provide the tuning input because it effectively hands the task of monitoring the tuning knob to a seperate processor (in the mouse). Rotary encoders can be tricky to handle reliably in linux. 

It is easy to modify a cheap mouse by disconnecting the existing switches and wiring the PCB to larger switches on the Langstone front panel. The scroll wheel can likewise be replaced with a panel mounted tuning knob. 

Microphone input and headphone output uses the USB audio device. (a couple of pounds on Ebay)

The software consists of two parts. The SDR itself uses a python GNURadio Flowgraph (Lang_TRX_Pluto.py or Lang_TRX_Hack.py) which can be created on a PC running GNUradio companion with Lang_TRX_Pluto.grc or Lang_TRX_Hack.grc. This Python program is then manually edited by adding the code from ControlTRX_Pluto.py or ControlTRX_HAck.py so it can be controlled by the GUI part of the software (LangstoneGUI_Pluto.c or LangstoneGui_Hack.c). These are written in C and communicate with GNURadio using a Linux Pipe. However to build and use a Langstone transceiver you do not need to know this!



# Installation for Langstone Transceiver

The preferred installation method only needs a Windows PC connected to the same (internet-connected) network as your Raspberry Pi.  Do not connect a keyboard or HDMI display directly to your Raspberry Pi.

- Download and install the Raspberry Pi Imager utility from https://downloads.raspberrypi.org/imager/imager_latest.exe
 
- These instructions are based on verdsion 2.0.6 of the Imager program. Other versions may differ but you should try to acheive the same settings. 

- Start the Imager Utility, Select 'Raspberry Pi 5' as the Raspberry Pi Device.

-  Select 'Raspberry Pi OS (Other)' then 'Raspberry Pi OS Lite (64 Bit)' as the operating system.

- **Note... It is important to check that you have selected the Lite version of the OS and not the Full version.** 

- Insert your micro SD card into a card reader and select that device for the Storage.  Note:- Ignore and close any message boxes about reformatting the drive. This is not needed. 

- When asked to choose a Hostname, you can choose anything you like or leave it at the Suggested Name.

- Set Your City, TimeZone and keyboard layout. You can normally accept the default settings.

- Set the username to 'pi' and the password to 'raspberry'

- Leave the WIFI settings Blank
 
- Selact 'Enable SSH' and 'Use Password authentication'

- Disable Raspberry Pi Connect

- Finally Click 'WRITE' to start writing the SD card.

- Make sure you use a good quality class 10 Micro-SD Card. (16GB is OK) The performance of the Raspberry Pi can be badly affected by poor quality cards. 

- Connect the touchscreen display, USB mouse, USB Sound Card, and SDR Module.   Power up the RPi with the new card inserted, and a network connection.  Do not connect a keyboard or HDMI display to the Raspberry Pi.

- The Raspberry Pi may restart several times as it configures the SD card. Eventually it should display a full boot on the LCD. 

- Find the IP address of your Raspberry Pi by looking at your internet router or by using an IP Scanner (such as Advanced IP Scanner http://filehippo.com/download_advanced_ip_scanner/ for Windows, or Fing on an iPhone) to get the RPi's IP address 

- From your windows PC use Putty (http://www.chiark.greenend.org.uk/~sgtatham/putty/download.html) to open an SSH login to the IP address that you noted earlier.  You will get a Security warning the first time you try; this is normal.

- Log in (user: pi, password: raspberry) then cut and paste the following code in, one line at a time:

## For the Adalm Pluto
```sh
wget https://raw.githubusercontent.com/g4eml/Langstone-V3/master/installPluto.sh
chmod +x installPluto.sh
./installPluto.sh
```
## For the HackRF One
```sh
wget https://raw.githubusercontent.com/g4eml/Langstone-V3/master/installHack.sh
chmod +x installHack.sh
./installHack.sh
```

At the start of the Install you will be asked for the Sudo password. Enter this as 'raspberry' , the same password you logged on with. 

The initial build can take some time, however it does not need any further user input, so go and make a cup of coffee and keep an eye on the touchscreen.  When the build is finished the Pi will reboot and should start-up with the Langstone Transceiver. Occasionally a second reboot may be required. If after this it still does not appear to be working then see the Langstone wiki for some things to look at.

# Updating the Software. 

If you have a running Langstone-V3 you can update by doing the following. 

Log into the Pi using SSH as described above. 

cd Langstone

./stop

./update

sudo reboot


