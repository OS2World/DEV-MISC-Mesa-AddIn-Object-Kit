
Mesa for OS/2 AddIn Object Kit

This package constitues a highly un-offical kit to aid in the development
of AddIns for Athena Design's Mesa for OS/2 Spreadsheet.  The base
functionallity and most of the documentation for the functions can be
found in the EXTADDIN.H header file.  The only other piece of knowledge
that is needed is your AddIn DLL must have these functions:

extern "C"
{
void AddInInit(void *);
void AddInFree(void *);
}

The AddInInit function is called when the DLL is loaded and ready for
use.  In this function is where you would initialize any data, add menu
items, register functions, etc...  AddInFree is called when the program
terminates so that you can cleanup any code that needs to be cleaned up.

Also, when you compile, you need to create a DEF file for you DLL that
contains exports for the above functions as well as the imports for the
AddIn functions.  An example DEF file looks like:
LIBRARY     SAMPADD INITINSTANCE TERMINSTANCE

DESCRIPTION 'The Mesa Sample AddIn DLL'

PROTMODE

DATA        MULTIPLE NONSHARED READWRITE LOADONCALL

CODE        LOADONCALL

EXPORTS
			AddInInit
			AddInFree
			
IMPORTS
			MESACORE.performAddInFunction
			MESACORE.MFree
			MESACORE.MMalloc
			MESACORE.MRealloc


To get Mesa to recognize your AddIn, you need to add an entry to the Mesa
section of the User INI file.  The REXX script:

Call RxFuncadd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs'
Call SysLoadFuncs
SysIni("USER", "Mesa2", "AddIn 1", "MESASOLV")

will do the trick, but the proper way to do it would be to see if
"AddIn 1" exists first and increment until you get to an "AddIn X"
that doesn't exist yet. That way, you don't disable someone elses
AddIn.

The RANGE.*, ADDIN.*, and MODEL.* are C++ classes to abstract the
functionality into easy to use objects.  These objects were design with a
minimalistic approach.  They do very little error detecting, etc... but I
find it much easier to use the objects that trying to deal with the
performAddInFunction() method of AddIn programming.  The header files for
these classes constain some of the notes I discovered while programming
them.  These notes are noted by a (*) in the comments.

Do to time constraints, etc... I can't fully support this code.  But if
you do have comments, suggestions, bug fixes, etc... I'll be glad to here
it.  I also may look into seeing if I can find the error codes for the
errors and also the other action flags.  Suggestions for other AddIn
functions can also be mailed to me.  If we do decide to enhance the AddIn
functionality, I may look over the responses to decide which ones to
implement first.  (Number one on my list is a
registerRecalcDoneFunction)

As a sample of the AddIns, I have included a simple Goal Seeker AddIn
along with the sorce code.  I a NOT a mathematician (or a speller for
that matter :) so the algorithms probably suck, but it at least shows
how to use the addins.  It also registers a BEEP(FREQUENCY,DURATION)
function.  Just think, what other spreadsheet can play songs during a
ReCalc.  :)  It also registers a NUMIN(ITEM,RANGE) that returns the
count of the number of times ITEM appears in RANGE.  Also,
LAYERNAME(LAYERNUMBER) returns the name of the layer.  MESASOLV.*
handles the AddIn level code as well as the functions.  SOLVER.* is a
solver object to handle the Goal Seeking.

Here are some suggestions for other AddIns:
1) Using the getMenuWindowMesaAddIn you can get the HWND for the menu.
   You could reorganize the menu to match Lotus, Excel, etc....
2) More functions such as the erf() function, database lookup 
   functions, etc....
3) You can create forms (dialogs) to enter data into the sheet.
4) A more robust solver than the one I wrote.
5) Many, many more things can be done.....


Enjoy the AddIns,
J. Daniel Kulp
Software Development Engineer
Athena Design, Inc.
dkulp@athena.com,dkulp@ccs.neu.edu
COMPUSERVE: 75052,1436

LEGAL STUFF:
The AddIn code in Mesa is extremely minimal and for the most part
un-tested.  Athena Design, being a relatively small company, doesn't have
the resources to testing this completey.  Basically this means that you
use this at your own risk.  We cannot offer support at this time.
Remember that Mesa is a fast evolving product and as it changes, so may
the AddIn code.  What works in the current version might not work in
future versions.  Also, because the functions are for the most part
untested, we cannot guarantee that any of it actually works.  Don't
complain to us if something you need doesn't seem to work.  We also
cannot guarantee that the AddIns won't change in future versions.  We may
increase the functionality, we might not.  Querying the version number in
the AddInInit() and taking appropriate action is probably a good idea.

The code contained herein by no means reflects the code used to create
Mesa for OS/2.  This code is completely written by me using my code
styles and to fullfill my wants/needs/desires. It should not be taken
that any "bad" code styles found within this code reflect similar
deficiencies in the Mesa code.  This code is COMPLETELY separate (except
for EXTADDIN.H) from Mesa for OS/2.  Remember, the Mesa programmers
are being paid to do things RIGHT.  I'm doing this on my own free
time (not that I have much of it) so I tend to cut corners and
simplify things when possible.

Trademarks and other stuff found in the code:
Mesa  is a trademark of Athena Design, Inc.
OS/2 is a registered trademark of IBM
REXX is a tademark of IBM
C-Set is a trademark of IBM
VisualAge C++ is a trademark of IBM

