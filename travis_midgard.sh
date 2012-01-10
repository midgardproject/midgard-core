#!/bin/bash

# Install Midgard2 library dependencies from OBS
sudo apt-get install -y dbus libglib2.0-dev libgda-4.0-4 libgda-4.0-dev libxml2-dev libdbus-1-dev libdbus-glib-1-dev valgrind python-gobject libgirepository1.0-dev libgirepository-1.0-1

./autogen.sh --prefix=/usr 
make
sudo make install
