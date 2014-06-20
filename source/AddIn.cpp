#define INCL_WIN
#include <os2.h>
#include <stdlib.h>
#include <string.h>
#include "extaddin.h"
#include "range.h"
#include "addin.h"


int AddIn :: addMenuItem(char * str)
{
    HWND menu = getMenu();
    MENUITEM mi = { MIT_END, MIS_TEXT,      0, 0,       NULL, 0 };
    HWND hwndPullDown;

    BOOL found = FALSE;
    USHORT x = 27000;

    while (!found ) {   // see if the Add In sub menu is there
        SHORT id = SHORT1FROMMR(WinSendMsg(menu,MM_ITEMPOSITIONFROMID,MPFROM2SHORT(x,TRUE),0));
        if (id != MID_NONE) {
            char buffer[100];
            WinSendMsg(menu,MM_QUERYITEMTEXT,MPFROM2SHORT(x,100),(MPARAM)&buffer);
            if (strcmp(buffer,"~Addins") == 0) {
                MENUITEM tMenuItem;
                found = TRUE;
                WinSendMsg(menu,MM_QUERYITEM,MPFROM2SHORT(x,TRUE),(MPARAM)&tMenuItem);
                hwndPullDown = tMenuItem.hwndSubMenu;
            } else {
                x++;
            } /* endif */
        } else {  // the AddIn Menu wasn't found, add it
            MENUITEM tMenuItem;
            found = TRUE;
            tMenuItem.iPosition = (SHORT1FROMMR(WinSendMsg (menu,
                             MM_QUERYITEMCOUNT, 0, 0))) - 1;
            tMenuItem.afStyle = MIS_TEXT | MIS_SUBMENU;
            tMenuItem.afAttribute = 0;

            tMenuItem.id = x;

            hwndPullDown = WinCreateWindow (HWND_OBJECT, WC_MENU, NULL,
                                   NULLHANDLE, 0, 0, 0, 0,
                                   menu, HWND_BOTTOM,
                                   x + 1, NULL, NULL);
            tMenuItem.hwndSubMenu = hwndPullDown;
            tMenuItem.hItem = 0;
            WinSendMsg (menu, MM_INSERTITEM, &tMenuItem, "~Addins");
            WinSendMsg (WinQueryWindow (menu, QW_PARENT), WM_UPDATEFRAME,
                    (MPARAM) FCF_MENU, 0);
        } /* endif */
    } /* endwhile */

    // find first unused ID
    found = FALSE;

    while (!found) {
        SHORT id = SHORT1FROMMR(WinSendMsg(menu,MM_ITEMPOSITIONFROMID,MPFROM2SHORT(x,TRUE),0));
        if (id == MID_NONE) {
           found = TRUE;
        } else {
           x++;
        } /* endif */
    } /* endwhile */

    mi.id = x;
    WinSendMsg(hwndPullDown,MM_INSERTITEM,(MPARAM)&mi,(MPARAM)str);
    return x;
}

int AddIn :: version()
{
    return (int)performAddInFunction(getVersionNumberMesaAddIn);
}

HWND AddIn :: getMenu()
{
    return (HWND)performAddInFunction(getMenuWindowMesaAddIn);
}

HWND AddIn :: getApplicationWindow()
{
   return (HWND)performAddInFunction(getApplicationWindowMesaAddIn);
}

HWND AddIn :: getActiveWindow()
{
    return (HWND)performAddInFunction(getActiveWindowMesaAddIn);
}

void AddIn :: registerMenuCallBackFunction(void *pfn)
{
    performAddInFunction(registerMenuCallBackMesaAddIn, mod_handle,pfn);
}

void AddIn :: registerPMCallBackFunction(void *pfn)
{
    performAddInFunction(registerPMCallBackMesaAddIn, mod_handle,pfn);
}

void AddIn :: registerDataEnteredFunction(void *pfn)
{
    performAddInFunction(registerDataEnteredMesaAddIn, mod_handle,pfn);
}

void AddIn :: registerAboutToCloseFunction(void * pfn)
{
    performAddInFunction(registerAboutToCloseMesaAddIn, mod_handle,pfn);
}

void AddIn :: registerJustOpenedFunction(void * pfn)
{
    performAddInFunction(registerOpenedMesaAddIn, mod_handle,pfn);
}

void AddIn :: registerAboutToSaveFunction(void * pfn)
{
    performAddInFunction(registerAboutToSaveMesaAddIn, mod_handle,pfn);
}

void AddIn :: registerDoubleClickFunction(void * pfn)
{
    performAddInFunction(registerDoubleClickMesaAddIn, mod_handle,pfn);
}

void AddIn :: registerFunction(char *func_name,char *fbName,char *fbPrototype, char *fbDescription,
		char *fbExample, int *fbMembers, void * callBack)
{
    performAddInFunction(registerFunctionMesaAddIn,func_name,fbName,fbPrototype,fbDescription,
		                 fbExample,fbMembers,callBack);
}

Address AddIn :: getCurrentAddress()
{
    int r,c,l;
    int rc = (int) performAddInFunction(getCurrentAddressMesaAddIn,&r, &c, &l);
    if (rc) {
       return Address(r,c,l);
    }
    return Address(-1,-1,-1);
}

Range AddIn :: getCurrentRange()
{
    int r,c,l;
    int r2,c2,l2;
    Range ra;
    Address a,b;
    int rc = (int) performAddInFunction(getCurrentRangeMesaAddIn,&r, &c, &l, &r2,&c2,&l2);
    if (rc) {
        a.set(r,c,l);
        b.set(r2,c2,l2);
        ra.add(a,b);
    } /* endif */
    return ra;
}

