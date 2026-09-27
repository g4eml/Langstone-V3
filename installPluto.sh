#!/bin/bash
# Langstone-V3 Install script 

set -e

echo "#########################################"
echo "## Installing Langstone-V3 Transceiver ##"
echo "#########################################"

# --- Enable permanent passwordless sudo for all members of the sudo group ---
SUDOERS_FILE="/etc/sudoers.d/010_sudo-group-nopasswd"

if [ ! -f "$SUDOERS_FILE" ]; then
    echo "Enabling passwordless sudo for all sudo group members."
    echo "You may be asked for your password once."
    echo "%sudo ALL=(ALL) NOPASSWD: ALL" | sudo tee "$SUDOERS_FILE" > /dev/null
    sudo chmod 0440 "$SUDOERS_FILE"

    # Remove the file if it's invalid, so sudo can never be broken
    if ! sudo visudo -cf "$SUDOERS_FILE" > /dev/null; then
        echo "Sudoers file invalid, removing it."
        sudo rm -f "$SUDOERS_FILE"
        exit 1
    fi
    echo "Passwordless sudo enabled."
fi
# --- End of sudo setup ---

# -------- Upgrade distribution ------

echo "#################################"
echo "##     Update Distribution     ##"
echo "#################################"
sleep 2

# Update the distribution
sudo apt-get -y update
sudo apt-get -y dist-upgrade


echo "#################################"
echo "##       Install Packages      ##"
echo "#################################"
sleep 2

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
sudo apt-get -y install hackrf
sudo apt-get -y install sshpass

echo "#################################"
echo "##        Install LibIIO       ##"
echo "#################################"
sleep 2

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
sleep 2

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
sleep 2

git clone https://github.com/g4eml/Langstone-V3.git
mv Langstone-V3 Langstone
cd Langstone
chmod +x build
chmod +x run
chmod +x stop
chmod +x update
chmod +x set_pluto
chmod +x set_sound
chmod +x run_hack
chmod +x stop_hack
chmod +x run_pluto
chmod +x stop_pluto

./build

# Set auto login to command line

cd ~

sudo raspi-config nonint do_boot_behaviour B2

# remove annoying nag message about SSH password. 

sudo rm /etc/profile.d/sshpwd.sh

sudo sed -i '/dtoverlay=vc4-kms-v3d/s/^/#/' /boot/firmware/config.txt

#make Langstone autostart on boot

cd Langstone
cp run_pluto run
cp stop_pluto stop

if !(grep Langstone ~/.bashrc) then
  echo if test -z \"\$SSH_CLIENT\" >> ~/.bashrc 
  echo then >> ~/.bashrc
  echo "stty -echo -icanon" >> ~/.bashrc
  echo $HOME/Langstone/run >> ~/.bashrc
  echo fi >> ~/.bashrc
fi

#Configure the boot parameters

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
