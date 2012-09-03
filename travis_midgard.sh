#!/bin/bash

# Install Midgard2 library dependencies from OBS
sudo apt-get install -y dbus libglib2.0-dev libgda-4.0-4 libgda-4.0-dev libxml2-dev libdbus-1-dev libdbus-glib-1-dev valgrind python-gobject libgirepository1.0-dev libgirepository-1.0-1 libgda-4.0-mysql libgda-4.0-postgres gir1.2-freedesktop gobject-introspection gir1.2-gda-4.0 mysql-server mysql-client 

./autogen.sh --prefix=/usr 
make
sudo make install

sudo mysql -e 'create database test_gir;'
sudo mysql -e 'SET GLOBAL sql_mode="";'
psql -c 'create database test_gir;' -U postgres
