# Langstone-V3 SDR Transceiver by Colin Durbridge G4EML

# Currently only supports the Adalm Pluto, the Raspberry Pi 5  and the official 7" LCD Display.

This is an experimental project to produce a simple VHF, UHF and Microwave SDR Transceiver operating on SSB CW and FM.

It was inspired by the very successful Portsdown Amateur Television system created by the British Amateur Television Club.

To install the software on a raspberry pi please follow the instructions further down the page. 

**More information can also be found on the UK Microwave group wiki at https://wiki.microwavers.org.uk/Langstone_Project**

Currently only the following hardware is supported:-

- Raspberry Pi 5

- Official Raspberry Pi 7" touchscreen

- Adalm Pluto SDR Module

- USB Audio module. Connected to loudspeaker or headphones and microphone. 
 
- USB Scroll mouse
 

- PTT via Raspberry Pi GPIO 17 (pin 11). This needs a pull up resistor to 3.3V. Grounding this pin will switch to Transmit.

- CW Key is via Raspberry Pi GPIO 18 (pin 12). This needs a pull up resistor to 3.3V. Grounding this pin will key the transmitter. 

- Tx Output is via Raspberry Pi GPIO 21 (pin 40). This output goes high when the Langstone is transmitting. This can be used to switch antenna relays and amplifiers. (100ms delay included for sequencing)

- 8 Band select Outputs on GPIO 1 (pin 28), GPIO 19 (pin 35), GPIO 4 (pin 7), GPIO 25 (pin 22), GPIO 22 (pin 16), GPIO 24 (pin 18), GPIO 10 (pin 19), and GPIO 9 (Pin 21). These can be used to select external filters, amplifiers or Transverters. The state of these outputs is defined using the Band Bits setting. 

- On the Adalm Pluto the TX output and first three of the Band Select outputs are also available on the Internal Pluto GPO connector. GPO0 is the Tx Output, GPO1-3 are the Band Select outputs.The main use for these is for when the Pluto is remotely mounted. Care must be taken as these pins are low voltage. They will need to be buffered before use. 

To build a complete functional transceiver you will need to add suitable filters, preamplifiers and power amplifiers to the Adalm Pluto. 

All control is done using the touchscreen and mouse.

Tuning uses the mouse scrollwheel. The mouse left and right buttons select the tuning step. The centre button is used for the CW key.  Mouse movement is not used.

A mouse is used to provide the tuning input because it effectively hands the task of monitoring the tuning knob to a seperate processor (in the mouse). Rotary encoders can be tricky to handle reliably in linux. 

It is easy to modify a cheap mouse by disconnecting the existing switches and wiring the PCB to larger switches on the Langstone front panel. The scroll wheel can likewise be replaced with a panel mounted tuning knob. 

Microphone input and headphone output uses the USB audio device. (a couple of pounds on Ebay)

The software consists of two parts. The SDR itself uses a python GNURadio Flowgraph (Lang_TRX_Pluto.py) which can be created on a PC running GNUradio companion with Lang_TRX_Pluto.grc. This Python program is then manually edited by adding the code from ControlTRX_Pluto.py so it can be controlled by the GUI part of the software (LangstoneGUI_Pluto.c). These are written in C and communicate with GNURadio using a Linux Pipe. However to build and use a Langstone transceiver you do not need to know this!



# Installation for Langstone Transceiver

The preferred installation method only needs a Windows PC connected to the same (internet-connected) network as your Raspberry Pi.  Do not connect a keyboard or HDMI display directly to your Raspberry Pi.

- Download and install the Raspberry Pi Imager utility from https://downloads.raspberrypi.org/imager/imager_latest.exe 

- Start the Imager Utility, Select 'Raspberry Pi 5' as the Raspberry Pi Device. Select 'Raspberry Pi OS (Other)'' then 'Raspberry Pi OS Lite (64 Bit)'' as the opertating system.

- Insert your micro SD card into a card reader and select that device for the Storage.  Note:- Ignore and close any message boxes about reformatting the drive. This is not needed. 

- When asked 'Use OS customisation' Select 'EDIT SETTINGS' then select 'Services' make sure that 'Enable SSH' is ticked and 'Use password authentication' is selected. 

- Click Save, then click 'YES' to start writing the SD card.

- Make sure you use a good quality class 10 Micro-SD Card. (16GB is OK) The performance of the Raspberry Pi can be badly affected by poor quality cards. 

- Connect the touchscreen display, USB mouse, USB Sound Card, and Pluto.   Power up the RPi with the new card inserted, and a network connection.  Do not connect a keyboard or HDMI display to the Raspberry Pi.

- The Rasperry Pi may restart several times as it configures the SD card. Eventually it should display a full boot on the LCD. 

- Find the IP address of your Raspberry Pi by looking at your internet router or by using an IP Scanner (such as Advanced IP Scanner http://filehippo.com/download_advanced_ip_scanner/ for Windows, or Fing on an iPhone) to get the RPi's IP address 

- From your windows PC use Putty (http://www.chiark.greenend.org.uk/~sgtatham/putty/download.html) to log in to the IP address that you noted earlier.  You will get a Security warning the first time you try; this is normal.

- Log in (user: pi, password: raspberry) then cut and paste the following code in, one line at a time:

```sh
wget https://raw.githubusercontent.com/g4eml/Langstone-V3/master/install.sh
chmod +x install.sh
./install.sh
```

The initial build can take some time, however it does not need any user input until the very end, so go and make a cup of coffee and keep an eye on the touchscreen.  When the build is finished the Pi will reboot and start-up with the Langstone Transceiver. If it does not appear to be working then see the Langstone wiki for some things to look at.

# Updating the Software. 

If you have a running Langstone you can update by doing the following. 

Log into the Pi using SSH as described above. 

cd Langstone

./stop

./update

sudo reboot


