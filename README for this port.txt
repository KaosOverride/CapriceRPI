Caprice RPI2 V1.3 - Release - Adapted/ported by KaosOverride 2015
(Caprice32 4.1.0 for Raspberry PI)

Caprice32 - Amstrad CPC Emulator
(c) Copyright 1997-2004 Ulrich Doewich

-File manager from NK's Snes9x GP2x port
-Autotype feature from Kevin Thacker (Arnold CPC emu) with Troels K's adaptation for Caprice
-Antiriad great skin for CapriceGP2x!
_____________________________________________________________________________________________________

Features:

-GPL Caprice32 4.1.0 emulator core from Ulrich Doewich
-Portable monolithic executable, without external file dependencies (System ROMs, skins, etc) 
-Static link to Raspberry PI hardware scaler SDL library (Optional Makefile)
-Autoconfiguration when called from launchers and frontends (6128 default, 464 for tapes) with partial support for autorun commands for tape and disk
-standard 2 joystick support, and up to the unofficial Fire 3, redefinable as key for many games
-ToTo's interface for joysticks 3 and 4 support.
-Free assign up to 4 USB joypads of the CPC joysticks, even some keyboard combinations.
-Option menus and CPC virtual keyboard. Full use of the CPC from your joystick!!!
-Supports DSK, CDT and SNA from menu and command line. Great for frontends and launchers
-Autorun for DSKs and CDTs (tape loading x5)
-Low-res modes for RPI1. Hi-res modes with progressive, scan-lines and interlaced effects
-SNA saving slot-style for DSK, CDT and SNA.

To do:
-Don't know. Please, tell me what else can I do!!
_____________________________________________________________________________________________________

Options

Main Emulation
===================
F1:  (X11 only) Full Screen (No hardware scaler), OK for RPI2 composite out
F2:  Change wit/without border and Lo-res/Hi-res modes
F3:  Reset
F4:  Start/Stop tape motor
F5:  Start loading tape with RUN" or |TAPE:RUN" depending model
F6:  Volume --
F7:  Volume ++
F8:  Options menu
F9:  Virtual keyboard (+CTRL Change position up/down)
F10: Quit
F11: Change Hi-res render mode, progressive, scan-lines, interlaced.
F12: show FPS / INFO (+CTRL Change realtime or "as fast as it can" 

Joystick:
  Fire 4: Options menu
  Fire 5: Virtual Keyboard
  Fire 6: Show FPS / INFO

Options Menu
============
Icons:
 Disk:		Disk menu
 Tape:		Tape menu
 Chip:		Snapshots menu
 Gear:		Configuration menu
 Red button:	Quit 

Select with cursors + Enter o Joystick + Fire 1
F8 or Fire 4 Return to emulation. 

Submenus
--------
Up/down (cursors/joystick) navigate options
Right (cursors/joystick), Enter or Fire 1 select option
Left (cursors/joystick), ESC or Fire 4 returns to emulation or the submenu before.

File selectors
--------------
Up/down (cursors/joystick) navigate files/folders
Right (cursors/joystick), Enter o Fire 1 select file or open subfolder
Left (cursors/joystick), Fire 2 return to the above folder
ESC o Fire 4 returns to emulation or the submenu before.

Virtual keyboard
================
From keyboard: (Helpfull if some emulated key can't be pressed!!)

Cursors: Move key cursor
CTRL: Simulate CTRL key
Shift: Simulate SHIFT key
Espace: Press selected key
F9: Exit virtual keyboard (+CTRL change position up/down)

From Joystick
Directions: Move key cursor
Fire 3: Simulate CTRL key
Fire 2: Simulate SHIFT key
Fire 1: Press selected key
Fire 5: Exit virtual keyboard
Fire 4: Change position up/down
_____________________________________________________________________________________________________

Release history
================

V1.3 Release
====
-Flexible support for free assign to the 4 USB joypads of CPC
 joysticks 1 and 2, ToTo's MultiPlay joysticks 3 and 4, and some
 keyboard keygroups for joystick use.
-Experimental CRT screen emulation.


V1.2 Release
====
-ToTo's 4-player support
-Autoboot fixes

V1.1 Release
====
-GUI's transparency fixes
-Autoboot fixes
-Extra Makefile for not RPI devices

V1.0 Release
====
-cleaned and checked code for release

V0.9 Pre-release
====
-Support for slot-style SNA saving for DSK, CDT and SNA
-Support for fast loading of slot-style SNA for each DSK, CDT o SNA
-Menus rearrangement. Support for drive B disc loading
-Option --notapeturbo for disabling fast tape load
-RPI 1/2 model detection. Disabling Hi-Resmodes for RPI1. Welcome
 message shows RPI, at the menu shows RPI1 or RPI2 depending model detected.

V0.2 W.I.P 2
====
-Low-res modes for RPI1. Hi-res modes with progressive, scan-lines and interlaced effects
-Autorun support, for DSKs and CDTs.


V0.1 W.I.P 1
====
-Virtual keyboard operative. Program BASIC from your joystick!!!!
-Autorun support, partial for disks, complete for tape.
-New RPI2 splash screen, replaced the GP2x one.


V0.01 ALPHA - GP2x "W.I.P 6" fast port
====
-Configured my old GP2x port for 732x534 instead of 320x200.
-Rewrite of the menu system forthe new resolution.
-Portable monolithic executable without external file dependencies  (ROMs, etc)
-Unfinnished-nonworking virtual keyboard

