
Debian
====================
This directory contains files used to package xdiskd/xdisk-qt
for Debian-based Linux systems. If you compile xdiskd/xdisk-qt yourself, there are some useful files here.

## xdisk: URI support ##


xdisk-qt.desktop  (Gnome / Open Desktop)
To install:

	sudo desktop-file-install xdisk-qt.desktop
	sudo update-desktop-database

If you build yourself, you will either need to modify the paths in
the .desktop file or copy or symlink your xdisk-qt binary to `/usr/bin`
and the `../../share/pixmaps/xdisk128.png` to `/usr/share/pixmaps`

xdisk-qt.protocol (KDE)

