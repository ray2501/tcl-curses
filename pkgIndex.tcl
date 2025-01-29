# -*- tcl -*-
# Tcl package index file, version 1.1
#
if {[package vsatisfies [package provide Tcl] 9.0-]} {
    package ifneeded curses 0.8.2 \
	    [list load [file join $dir libtcl9curses0.8.2.so] [string totitle curses]]
} else {
    package ifneeded curses 0.8.2 \
	    [list load [file join $dir libcurses0.8.2.so] [string totitle curses]]
}
