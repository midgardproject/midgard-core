#! /bin/sh

src_dir=`pwd`
configure_options=--enable-gtk-doc

autoreconf -i 
automake

$src_dir/configure --enable-gtk-doc

gtkdocize || exit 1

echo
echo "Run \`make\` to compile"
echo


