#/bin/bash
# Steps to be taken to setup the environment:

# Install python-serial (and git)
sudo apt update
sudo apt install python-serial git unzip

# Clone the arduino-mk project by sudar into the PCD directory
git clone https://github.com/sudar/Arduino-Makefile.git

#Download and extract arduino into the PCD directory
mkdir tmp && cd tmp
wget https://downloads.arduino.cc/arduino-1.8.8-linux64.tar.xz
tar xf arduino-1.8.8-linux64.tar.xz -C ..

# Download and add the needed libraries:
wget http://arduiniana.org/Streaming/Streaming5.zip
unzip Streaming5.zip -d ../arduino-1.8.8/libraries/
cd ../arduino-1.8.8/libraries/
git clone https://github.com/JChristensen/DS3232RTC.git
git clone https://github.com/PaulStoffregen/Time.git
mv ./Time ./TimeLib



# Optional: add youself to the dialout group and restart computer (so you don't need to run make as sudo):
#sudo usermod -a -G dialout <username>