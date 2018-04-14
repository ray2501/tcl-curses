tcl-curses
=====

It is a "minimalist" tcl package for interfacing to curses.
Source cod is from [Minimalist Curses](https://wiki.tcl.tk/10877).
The author is [Venkat Iyer](https://wiki.tcl.tk/8740).

I add TEA files to build and use
[ncurses](https://www.gnu.org/software/ncurses/ncurses.html) library to test
(add sub-command bkgd, bkgdset, init_pair and getch).


License
=====

Tcl


Commands
=====
curses init  
curses end  
curses attr boolean attribute ?number?  
curses move row col  
curses puts string   
curses info characteristic  
curses erase  
curses refresh  
curses bkgd number  
curses bkgdset number  
curses init_pair number fgcolor bgcolor  
curses getch ?y x?  

