SSaver -- 32 bit Windows Screen Saver Sample

SSAVER demonstrates how to write a screen saver that runs under 
Windows 95 and Windows NT.  It has been designed so that it can 
be easily customized with your own animations.  You can add your 
own screen saver configuration controls by modifying the 
configuration dialog resource then adding the appropriate code 
to the ScreenSaverConfigureDialog() in SSAVER.C.  You can change 
the animations that are produced by modifying the code in 
DOFRAME.C.

The following files are a part of the SSAVER sample:

 - DoFrame.h
   DoFrame.C  - Contains all of the functions and code for drawing 
                frames in the screen savers animation.

 - SSaver.h
   SSaver.c   - Contains all the code for setting up the screen 
                saver and processing both configuration and screen 
                saver messages.
