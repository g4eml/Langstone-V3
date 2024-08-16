#!/bin/bash
# Langstone-V3 Install script 
# Buster Version G4EML 08/08/24

echo "#########################################"
echo "## Installing Langstone-V3 Transceiver ##"
echo "#########################################"


# -------- Upgrade distribution ------

echo "#################################"
echo "##     Update Distribution     ##"
echo "#################################"

# Update the distribution
sudo apt-get -y update
sudo apt-get -y dist-upgrade


echo "#################################"
echo "##       Install Packages      ##"
echo "#################################"

## Install the packages that we need
sudo apt-get -y install git
sudo apt-get -y install pip 
sudo apt-get -y install gnuradio
sudo apt-get -y install cmake
sudo apt-get -y install libusb-1.0-0-dev
sudo apt-get -y install libavahi-client-dev 
sudo apt-get -y install libxml2-dev
sudo apt-get -y install bison 
sudo apt-get -y install flex 
sudo apt-get -y install libaio-dev
sudo apt-get -y install libzstd-dev

echo "#################################"
echo "##        Install LibIIO       ##"
echo "#################################"

cd ~
git clone https://github.com/analogdevicesinc/libiio.git
cd libiio
##git reset --hard b6028fdeef888ab45f7c1dd6e4ed9480ae4b55e3  # Back to Version 0.25
cmake ./
make all
sudo make install

echo "#################################"
echo "##        Install lgpio      ##"
echo "#################################"

cd ~
wget https://github.com/joan2937/lg/archive/master.zip
unzip master.zip
rm master.zip
mv lg-master lgpio
cd lgpio
make
sudo make install
cd ~




echo "####################################"
echo "##     Installing Langstone-V3    ##"
echo "####################################"

git clone https://github.com/g4eml/Langstone-V3.git
mv Langstone-V3 Langstone
cd Langstone
chmod +x build
chmod +x run
chmod +x stop
chmod +x update
chmod +x set_pluto
chmod +x set_sound

./build

# Set auto login to command line

cd ~

sudo raspi-config nonint do_boot_behaviour B2

# remove annoying nag message about SSH password. 

sudo rm /etc/profile.d/sshpwd.sh


#make Langstone autostart on boot

if !(grep Langstone ~/.bashrc) then
  echo if test -z \"\$SSH_CLIENT\" >> ~/.bashrc 
  echo then >> ~/.bashrc
  echo /home/pi/Langstone/run >> ~/.bashrc
  echo fi >> ~/.bashrc
fi

#Configure the boot parameters

if !(grep lcd_rotate /boot/firmware/config.txt) then
  sudo sh -c "echo lcd_rotate=2 >> /boot/firmware/config.txt"
fi
if !(grep disable_splash /boot/firmware/config.txt) then
  sudo sh -c "echo disable_splash=1 >> /boot/firmware/config.txt"
fi
if !(grep global_cursor_default /boot/firmware/cmdline.txt) then
  sudo sed -i '1s,$, vt.global_cursor_default=0,' /boot/firmware/cmdline.txt
fi


echo "#################################"
echo "##       Reboot and Start      ##"
echo "#################################"

#Reboot and start
sleep 5
sudo reboot
