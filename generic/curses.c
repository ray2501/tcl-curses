 /* curses.c
    A "minimalist" tcl package for interfacing to curses.  Goes into
    curses mode on load and automatically comes out at exit
    Venkat Iyer.  VI.  venksi@yahoo.com
 
    Usage:
 
    curses init (no need to call, automatically done at load)
    curses end  (no need to call, automatically done at exit)
    curses attr <on/off> <standout/underline/reverse/blink/dim/bold/alt>
    curses move <row> <column> : move to screen position, 0 0 is top left
    curses puts  : print a string
    curses info  <rows/cols> : return the number of rows/cols in screen
    curses erase : clear the screen
    curses refresh : actually do refresh to physical screen
 
    Build: 
    gcc -I/usr/local/tcl/8.4.5/include -fPIC -c curses.c -o curses.o
 
    (something like this, which is on solaris7):
    ld -r curses.o -o curses.so -Bsymbolic /usr/lib/libcurses.a
    (on linux)
    gcc -shared curses.o -o curses.so /usr/lib/libcurses.a
 */

/*
 * For C++ compilers, use extern "C"
 */

#ifdef __cplusplus
extern "C" {
#endif
 
#include "tcl.h"
#include <string.h>
#include <curses.h>
 
/*
 * Forward declarations for procedures defined later in this file:
 */
 
static int cursesCmd (ClientData dummy, Tcl_Interp *interp, int objc,Tcl_Obj *const*objv);
 
static int curses_start (void);
static void curses_exit (ClientData dummy); /* exit handler */

/*
 * Only the _Init function is exported.
 */

extern DLLEXPORT int    Curses_Init(Tcl_Interp * interp);

/*
 * end block for C++
 */

#ifdef __cplusplus
}
#endif
 
 /*
  *----------------------------------------------------------------------
  *
  * Curses_Init --
  *
  *     This procedure is the main initialisation point of the Curses
  *     extension.
  *
  * Results:
  *     Returns a standard Tcl completion code, and leaves an error
  *     message in the interp's result if an error occurs.  We're OK
  *      to init in a safe interpreter.  No file access done.
  *
  * Side effects:
  *     Adds a command to the Tcl interpreter.  Adds an exit handler
  *      and changes the screen into raw mode
  *
  *----------------------------------------------------------------------
  */
 
 
 int
 Curses_Init (interp)
     Tcl_Interp *interp;                /* Interpreter for application */
 {
     if (Tcl_InitStubs(interp, "8.4", 0) == NULL) {
        return TCL_ERROR;
     }
     if (Tcl_PkgRequire(interp, "Tcl", "8.4", 0) == NULL) {
        return TCL_ERROR;
     }
     if (Tcl_PkgProvide(interp, "curses", "0.8.0") == TCL_ERROR) {
         return TCL_ERROR;
     }
     
     Tcl_CreateObjCommand(interp, "curses", (Tcl_ObjCmdProc *) cursesCmd, 
                         (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
 
     curses_start();
     Tcl_CreateExitHandler(curses_exit,(ClientData) NULL);
     return TCL_OK;
 }
 
 /*
  * curses_exit -- exit handler for curses
  * Results: -- none
  * Side effects: gets out of curses by calling endwin
  */
 
 static void
 curses_exit (ClientData dummy)
 {
     endwin();
 }
 
 
 /* An error reporting routine for varargs results
  * Results : -- always TCL_ERROR, so we can just return
  * the value of this call
  * Side effects: Sets the result in the interpreter.
  */
 
 #define MAX_ERROR_SIZE   1024
 
 static int 
 setTclError TCL_VARARGS_DEF (
     Tcl_Interp *,
     i)
 {
     va_list argList;
     char buf[MAX_ERROR_SIZE];
     char *format;
     
     Tcl_Interp *interp = TCL_VARARGS_START(Tcl_Interp *, i, argList);
     format = va_arg(argList, char *);
     vsnprintf(buf, MAX_ERROR_SIZE, format, argList);
     buf[MAX_ERROR_SIZE-1] = '\0';
     Tcl_SetResult(interp, buf, TCL_VOLATILE);
     return TCL_ERROR;
 } 
 
 /*
  * curses_start -- init handler for curses. called on loading
  * Results: -- always TCL_OK
  * Side effects: gets into curses mode.
  */
 
 static int
 curses_start(void)
 {
     initscr();          /* will exit if there is an error */
     if (has_colors()) {  /* use colors if we have them */
         start_color();
     }
     cbreak();
     noecho();
     nonl();
     intrflush(stdscr,FALSE);
     keypad(stdscr,TRUE);
     return TCL_OK;
 }
 
 /*
  * --------------------------------------------------------------- 
  * cursesCmd --
  *
  * Implmements the "curses" command.  Doesn't do colors yet
  * 
  * Results:
  *      A standard Tcl result. 
  *
  * Side effects:
  *      See the curses man page.  All side effects are inside the
  *      the library or on the screen!
  * 
  * Usage is listed at the top of this file
  */
     
 static int 
 cursesCmd (ClientData dummy, Tcl_Interp *interp, int objc,Tcl_Obj *const*objv)
 {
     int index;
 
     static const char *optionStrings[] = {
         "init", "end", "attr", "move", 
         "puts", "info", "erase", "refresh",
         "bkgd", "bkgdset", "init_pair", "getch",
         NULL
     };
 
     enum options {
         CURSES_INIT, CURSES_END, CURSES_ATTR, CURSES_MOVE, 
         CURSES_PUTS, CURSES_INFO, CURSES_ERASE, CURSES_REFRESH,
         CRUSES_BKGD, CRUSES_BKGDSET, CURSES_INIT_PAIR, CURSES_GETCH,
     };
 
     static const char *attrStrings[] = {
         "standout", "underline", "reverse", 
         "blink", "dim", "bold", "alt", NULL
     };
 
     enum attrs {
         CURSES_A_STANDOUT, CURSES_A_UNDERLINE, CURSES_A_REVERSE,
         CURSES_A_BLINK,    CURSES_A_DIM,       CURSES_A_BOLD,
         CURSES_A_ALT
     };

     static const char *colorStrings[] = {
         "black", "red", "green", "yellow",
         "blue", "magenta", "cyan", "white", NULL
     };
 
     static const char *infoStrings[] = {
         "cols", "lines", NULL
     };
 
     enum infos {
         CURSES_COLS,      CURSES_LINES
     };
 
     static const int attrVals[] = {
         A_STANDOUT,   A_UNDERLINE,   A_REVERSE,
         A_BLINK,      A_DIM,         A_BOLD, 
         A_ALTCHARSET
     };
 
     if (objc < 2) {
         Tcl_WrongNumArgs(interp, 1, objv, "option ?arg ...?");
         return TCL_ERROR;
     }
     
     if (Tcl_GetIndexFromObj(interp, objv[1], optionStrings, "option", 0,
             &index) != TCL_OK) 
         return TCL_ERROR;
 
     switch ((enum options) index) {
        case CURSES_INIT: {
            return curses_start();
        }
        case CURSES_END: {
            endwin();
            return TCL_OK;
        }
        case CURSES_ATTR: { 
            int on, index, attr;
            int number;
            char *attrs = NULL;
            int len;
    
            if (objc != 4 && objc != 5) {
                Tcl_WrongNumArgs(interp, 2, objv, "boolean attribute ?number?");
                return TCL_ERROR;
            }
            if (Tcl_GetBooleanFromObj(interp, objv[2], &on) != TCL_OK) 
                return TCL_ERROR;

            if(objc == 4) {
                if (Tcl_GetIndexFromObj(interp, objv[3], attrStrings, 
                                    "attribute", 0, &index) != TCL_OK) 
                    return TCL_ERROR;   /* perhaps allow an integer here? */
            
                attr = attrVals[index];
            } else if (objc == 5) {
                attrs = Tcl_GetStringFromObj(objv[3], &len);
                if(!attrs || len < 1) return TCL_ERROR;

                if(strcmp(attrs, "color_pair") != 0) {
                    return TCL_ERROR;
                }

                if (Tcl_GetIntFromObj(interp, objv[4], &number) != TCL_OK)
                    return TCL_ERROR;

                attr = COLOR_PAIR(number);
            } 
            if (on) {
                attron(attr);
            }  else {
                attroff(attr);
            }
            return TCL_OK;
        }
        case CURSES_MOVE: {
            int row, col;
            if (objc != 4) {
                Tcl_WrongNumArgs(interp, 2, objv, "row col");
                return TCL_ERROR;
            }
            if (Tcl_GetIntFromObj(interp, objv[2], &row) != TCL_OK)
                return TCL_ERROR;
            if (Tcl_GetIntFromObj(interp, objv[3], &col) != TCL_OK)
                return TCL_ERROR;
            move(row, col);
            return TCL_OK;
        }
        case CURSES_PUTS: {
            if (objc != 3) {
                Tcl_WrongNumArgs(interp, 2, objv, "string");
                return TCL_ERROR;
            }
            addstr(Tcl_GetString(objv[2]));
            return TCL_OK;
        }
        case CURSES_INFO: {
            int index;
    
            if (objc != 3) {
                Tcl_WrongNumArgs(interp, 2, objv, "characteristic");
                return TCL_ERROR;
            }
            if (Tcl_GetIndexFromObj(interp, objv[2], infoStrings,
                                    "characteristic", 0, &index) != TCL_OK)
                return TCL_ERROR;
            switch ((enum infos) index) {
                case CURSES_COLS: {
                    Tcl_SetObjResult(interp,Tcl_NewIntObj(COLS));
                    return TCL_OK;
                }
                case CURSES_LINES: {
                    Tcl_SetObjResult(interp,Tcl_NewIntObj(LINES));
                    return TCL_OK;
                }
                default: {
                    return setTclError(interp, "Couldn't understand info "
                                        "characteristic %d", index);
                }
            }
            return TCL_OK;
        }
        case CURSES_ERASE: {
            if (objc != 2) {
                Tcl_WrongNumArgs(interp, 2, objv, "");
                return TCL_ERROR;
            }
            erase();
            return TCL_OK;
        }
        case CURSES_REFRESH: {
            if (objc != 2) {
                Tcl_WrongNumArgs(interp, 2, objv, "");
                return TCL_ERROR;
            }
            refresh();
            return TCL_OK;
        }
        case CRUSES_BKGD: {
            int number = 1;

            if (objc != 3) {
                Tcl_WrongNumArgs(interp, 2, objv, "number");
                return TCL_ERROR;
            }

            if (Tcl_GetIntFromObj(interp, objv[2], &number) != TCL_OK)
                return TCL_ERROR;

            if(number < 1)
                return TCL_ERROR;

            bkgd(COLOR_PAIR(number));
            return TCL_OK;
        }
        case CRUSES_BKGDSET: {
            int number = 1;

            if (objc != 3) {
                Tcl_WrongNumArgs(interp, 2, objv, "number");
                return TCL_ERROR;
            }

            if (Tcl_GetIntFromObj(interp, objv[2], &number) != TCL_OK)
                return TCL_ERROR;

            if(number < 1)
                return TCL_ERROR;

            bkgdset(COLOR_PAIR(number));
            return TCL_OK;
        }
        case CURSES_INIT_PAIR: {
            int number = 1;
            short bg, fg;

            if (objc != 5) {
                Tcl_WrongNumArgs(interp, 2, objv, "number fgcolor bgcolor");
                return TCL_ERROR;
            }

            if(can_change_color() == FALSE)
                return TCL_ERROR;

            if (Tcl_GetIntFromObj(interp, objv[2], &number) != TCL_OK)
                return TCL_ERROR;

            if(number < 1)
                return TCL_ERROR;

            if (Tcl_GetIndexFromObj(interp, objv[3], colorStrings, 
                                    "color", 0, &index) != TCL_OK) 
                    return TCL_ERROR;
            fg = index;

            if (Tcl_GetIndexFromObj(interp, objv[4], colorStrings, 
                                    "color", 0, &index) != TCL_OK) 
                    return TCL_ERROR;
            bg = index;

            init_pair(number, fg, bg);
            return TCL_OK;
        }
        case CURSES_GETCH: {
            int c = 0;
            int y, x;
            if (objc != 2 && objc != 4) {
                Tcl_WrongNumArgs(interp, 2, objv, "?y x?");
                return TCL_ERROR;
            }

            if(objc == 4) {
                if (Tcl_GetIntFromObj(interp, objv[2], &y) != TCL_OK)
                    return TCL_ERROR;

                if (Tcl_GetIntFromObj(interp, objv[3], &x) != TCL_OK)
                    return TCL_ERROR;

                c = mvgetch(y, x);
            } else if(objc == 2) {
                c = getch();
            }

            Tcl_SetObjResult(interp, Tcl_NewIntObj( c ));
            return TCL_OK;
        }
        default: {
            return setTclError(interp, "Couldn't understand enum %d as "
                                "action type", index);
        }
     }
}
