/*
 * This source code is a part of coLinux source package.
 *
 * Nuno Lucas <lucas@xpto.ath.cx>, 2005 (c)
 * Dan Aloni <da-x@gmx.net>, 2003-2004 (c)
 * Some code taken from WINE's X11 keyboard driver (c).
 *
 * The code is licensed under the GPL. See the COPYING file at
 * the root directory.
 *
 */ 
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <FL/x.h>

/* 
 * Stolen WINE goodies...
 *
 * For coLinux's keyboard driver we need something than can convert
 * X11 keyboard events to keyboard scancode. Because WINE already did
 * something similar for converting to Windows events, I just took
 * some code from there.
 *
 * - DA
 *
 * I fixed the tables for some scan codes that weren't right (NumLock was one).
 * The problem is I only have my Portuguese keyboard to test, so I need
 * others to test this (but I'm confident it's ok).
 *
 * ~Nuno Lucas
 */

static const short int nonchar_key_scan[256] =
{
	/* unused */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,              /* FF00 */
	/* special keys */
	0x0E, 0x0F, 0x00,    0, 0x00, 0x1C, 0x00, 0x00,              /* FF08 */
	0x00, 0x00, 0x00, 0x45, 0x46, 0x00, 0x00, 0x00,              /* FF10 */
	0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,              /* FF18 */
	/* unused */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,              /* FF20 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,              /* FF28 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,              /* FF30 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,              /* FF38 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,              /* FF40 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,              /* FF48 */
	/* cursor keys */
	0x147,0x14B,0x148,0x14D,0x150,0x149,0x151,0x14F,             /* FF50 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,              /* FF58 */
	/* misc keys */
	   0,0x137,    0,0x152, 0x00, 0x00, 0x00,0x15D,              /* FF60 */
	   0,    0, 0x38,0x146, 0x00, 0x00, 0x00, 0x00,              /* FF68 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,              /* FF70 */
	/* keypad keys */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x138,??0x45??,              /* FF78 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,              /* FF80 */
	0x00, 0x00, 0x00, 0x00, 0x00,0x11C, 0x00, 0x00,              /* FF88 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x47, 0x4B, 0x48,              /* FF90 */
	0x4D, 0x50, 0x49, 0x51, 0x4F, 0x4C, 0x52, 0x53,              /* FF98 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,              /* FFA0 */
	0x00, 0x00, 0x37, 0x4E,    0, 0x4A, 0x53,0x135,              /* FFA8 */
	0x52, 0x4F, 0x50, 0x51, 0x4B, 0x4C, 0x4D, 0x47,              /* FFB0 */
	0x48, 0x49, 0x00, 0x00, 0x00, 0x00,                          /* FFB8 */
	/* function keys */
	0x3B, 0x3C,
	0x3D, 0x3E, 0x3F, 0x40, 0x41, 0x42, 0x43, 0x44,              /* FFC0 */
	0x57, 0x58, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,              /* FFC8 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,              /* FFD0 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,              /* FFD8 */
	/* modifier keys */
	0x00, 0x2A, 0x36, 0x1D,0x11D, 0x3A, 0x00, 0x38,              /* FFE0 */
	0x138,0x38,0x138,0x15B,0x15C, 0x00, 0x00, 0x00,              /* FFE8 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,              /* FFF0 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x153               /* FFF8 */
};

static const short int nonchar_key_scan2[256] =
{
	0x00, 0x00, 0x00,0x138, 0x00, 0x00, 0x00, 0x00,              /* FE00 */
	0x1D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,              /* FE08 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,              /*   10 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,              /*      */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,              /*   20 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,              /*      */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,              /*   30 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,              /*      */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,              /*   40 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,              /*      */
	0x00, 0x1B, 0x00, 0x2B, 0x00, 0x00, 0x00, 0x00,              /*   50 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,              /*      */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,              /*   60 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,              /*      */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,              /*   70 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,              /*      */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,              /*   80 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,              /*      */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,              /*   90 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,              /*      */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,              /*   A0 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,              /*      */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,              /*   B0 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,              /*      */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,              /*   C0 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,              /*      */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,              /*   D0 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,              /*      */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,              /*   E0 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,              /*      */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,              /*   F0 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,              /*      */
};


/**
 * Called to handle FLTK key events.
 *
 * Returns non-zero if event was handled, zero if not.
 */
int console_input::handle_key_event( )
{
    if ( fl_xevent == NULL )
        return 0;

    XEvent xev = *fl_xevent;

    if ( xev.type != KeyPress && xev.type != KeyRelease )
        return 0;   // Not the event we were looking for

    // Get Keysym for the event
    KeySym          ksym  = 0;
    char            xlated[64]; // dummy
    unsigned short  scan  = 0;

    ::XLookupString( &xev.xkey, xlated, sizeof(xlated), &ksym, NULL );

    if ( ksym != 0 )
    {
        // Get scan code from tables
        if ( ksym>>8 == 0xFF )
            scan = nonchar_key_scan[ ksym & 0xFF ];
        else if ( ksym>>8 == 0xFE )
            scan = nonchar_key_scan2[ ksym & 0xFF ];
        else
            scan = static_cast<unsigned short>( xev.xkey.keycode - 8 );
    }
    else
    {
        fprintf( stderr, "Unknown keycode 0x%02X! Multimedia key?\n",
                        xev.xkey.keycode );
        return 0;
    }

    // If key release, or scancode with 0x80
    if ( xev.type == KeyRelease )
        scan |= 0x80;

    // Send key scancode
    send_scancode( scan );

    // We want all possible keys into colinux
    return 1;
}
