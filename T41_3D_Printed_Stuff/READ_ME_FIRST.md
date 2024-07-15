The following directories contain both the original KF5N FreeCAD files for printing the T-41 v10 and v11 case along with a directory for modifications I made to some of the files for v12.  A few important points.

>WARNING: I have *not* completed the v12 build, so some modifications may still be required to the design once I start to fit everything together. USE AT YOUR OWN RISK.  Better yet, use them as your own starting point. 

I encourage you to look at the dimensions, especially of the holes and slots to ensure that the components you are using match.  For example, the hole for the microphone connector is tailored to the connector I am using.  Your dimensions may be different.

I tried to keep the filenames consistent with the original K5FN versions to make understanding their placement easier.  The files that I modified end with "_KM5GT_V12." The ones without the KF5N callsign are original.

As I continue my build I will upload further modifications.  I am not a speedy builder, so if you sprint ahead and make modifications let me know and I'll incorporate your updates here.

## Boards Unchanged
* **Base_Left_K5FN** - The mounting base with holes for the board mounting brackets and the case panels. Both the front and back panels, along with the board mounts, attach to this base.
* **Base_Right_K5FN** - Same as the previous, but the right hand side. 

## The Modifications
* **Front_left_panel_KF5N_KM5GT_v12** - The left front panel holding the display, the new on/off pushbutton, a microphone button, and 3.5mm holes for the other functions.  I plan on moving the PTT to the back, so there is no hole on the front for this function. The sizing for the original display is unchanged. 
* **Front_Right_Panel_KF5N_KM5GT_V12** - The right front panel modified to hold the new switch matrix and with holes for the volume, Filter WPN, Center Tune and Fine Tune controls using Bill's, K9HZ encoder boards.
*  **Main_Fan_Bracket_KM5GT_V12** - A slip on fan bracket for the main board.
* **Mounting_Bracket_KF5N_KM5GT_v12** - Because the v12 boards have components right up to the edge of the PCB this mounting bracket allows enough clearance for the boards to mount.  The boards I have tested so far are the Main and the RF boards.  The others fit *without components* so modifications may still be needed here.
* **Switch_Matrix_Spacer_v12** - I am using an 11mm spacer to separate the switch PCB from the front panel control board so I can use a standard header to connect the two boards together. 

**From Greg's, KF5N's original files:**

This is an alternative set of Freecad files for the T41 Software Designed Transceiver Project which were modified by Greg KF5N (and now KM5GT).

This case design is a bit smaller than the original.  It should fit on stock Ender 3 series 3D printers.

  This software is made available under the GNU GPL v3 license agreement. If commercial use of this
  software is planned, we would appreciate it if the interested parties get written approval
  from Jack Purdum, W8TEE, and Al Peter, AC8GY.

  Any and all other commercial uses, written or implied, are forbidden without written permission from
   from Jack Purdum, W8TEE, and Al Peter, AC8GY.


