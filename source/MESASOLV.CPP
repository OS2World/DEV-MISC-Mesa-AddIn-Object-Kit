// we are including all the PM header files here.  Please replace with you
// stock "Include all the OS/2 PM stuff" include file
#define INCL_WIN

extern "C"
{
#include <os2.h>
}

// include the Mesa AddIn header file
#include "extaddin.h"
#include <string.h>
#include <stdio.h>                 // added for printf
#include <stdlib.h>

// to avoid the C++ name mangling, let's define the names of the external
// function here

extern "C"
{
void AddInInit(void *);
void AddInFree(void *);
}


#include "mesasolv.h"
#include "model.h"
#include "range.h"
#include "solver.h"
#include "addin.h"

// a couple of global values
AddIn *thisAddIn = NULL;
int solverMenuId = 0;
HWND *theHwnds = NULL;
int count = 0;

// the data needed in the goal seeker.
struct _solveData {
   int type;
   Model *md;
   Solver *sp;
   ULONG idTimer;
};


// this is the procedure that processes the "goal seeker" Dialog
MRESULT EXPENTRY SolvDlgProc(HWND hwnd,ULONG msg,MPARAM mp1,MPARAM mp2)
{
   MRESULT ret = (MRESULT) 0L;
   _solveData *sd = (_solveData *)WinQueryWindowULong(hwnd,QWL_USER);
   char buffer[100];
   int tmp, x;
   Address ad;

    switch (msg) {
    case WM_INITDLG:
            x = 0;  // add itself to the array of hwnds
            while (x < count && theHwnds[x]) {
               x++;
            } /* endwhile */

            if (x == count) {
                count *= 2;
                theHwnds = (HWND *)realloc(theHwnds,count*sizeof(HWND));
            } /* endif */

            theHwnds[x] = hwnd;

            sd = new _solveData;
            WinSetWindowULong(hwnd,QWL_USER,(ULONG) sd);
            sd->md = new Model(-1);
            sd->sp = NULL;

            ad=thisAddIn->getCurrentAddress();

            if(! sd->md->getBlob("SOLVER_CHNGCELL",100,buffer))
                sd->md->addressToString(ad,buffer,100);
            WinSetDlgItemText(hwnd,DID_CHANGECELL,buffer);

            if(! sd->md->getBlob("SOLVER_FINALCELL",100,buffer))
                sd->md->addressToString(ad,buffer,100);
            WinSetDlgItemText(hwnd,DID_SETCELL,buffer);

            if(! sd->md->getBlob("SOLVER_PREC",100,buffer))
                strcpy(buffer,"0.0001");
            WinSetDlgItemText(hwnd,DID_PRECISION,buffer);

            if(! sd->md->getBlob("SOLVER_FVAL",100,buffer))
                strcpy(buffer,"0.0");
            WinSetDlgItemText(hwnd,DID_FINALVALUE,buffer);

            if(! sd->md->getBlob("SOLVER_TYPE",100,buffer))
                sd->type = 0;
            else
                sd->type = atoi(buffer);

            if (sd->type < 0 || sd->type > 2) {
               sd->type = 0;
            } /* endif */

            WinSendDlgItemMsg(hwnd,sd->type+DID_VALUE,BM_SETCHECK,(PVOID)TRUE,0L);

            sd->md->getName(buffer,100);
            WinSetDlgItemText(hwnd,DID_MODELNAME,buffer);

            WinEnableWindow(WinWindowFromID(hwnd,DID_STOP),FALSE);
            WinEnableWindow(WinWindowFromID(hwnd,DID_START),TRUE);
       break;
    case WM_CONTROL:
        switch (SHORT1FROMMP(mp1)) {
        case DID_MINIMUM:
           sd->type = 1;
           break;
        case DID_MAXIMUM:
           sd->type = 2;
           break;
        case DID_VALUE:
           sd->type = 0;
           break;
        default:
            return WinDefDlgProc ( hwnd,msg,mp1,mp2 ) ;
        } // end switch
       break;
    case WM_COMMAND:
       switch (LOUSHORT(mp1)) {
       case DID_START:
          {
            Address adc,ads;
            double prec,fv;
            int type;

            WinQueryDlgItemText(hwnd,DID_CHANGECELL,100,buffer);
            sd->md->setBlob("SOLVER_CHNGCELL",strlen(buffer)+1,buffer);
            adc = sd->md->stringToAddress(buffer);
            WinQueryDlgItemText(hwnd,DID_SETCELL,100,buffer);
            sd->md->setBlob("SOLVER_FINALCELL",strlen(buffer)+1,buffer);
            ads = sd->md->stringToAddress(buffer);

            WinQueryDlgItemText(hwnd,DID_FINALVALUE,100,buffer);
            sd->md->setBlob("SOLVER_FVAL",strlen(buffer)+1,buffer);
            fv = strtod(buffer,NULL);
            WinQueryDlgItemText(hwnd,DID_PRECISION,100,buffer);
            sd->md->setBlob("SOLVER_PREC",strlen(buffer)+1,buffer);
            prec = strtod(buffer,NULL);

            sprintf(buffer,"%d",sd->type);
            sd->md->setBlob("SOLVER_TYPE",strlen(buffer)+1,buffer);

            sd->sp = new Solver(*sd->md,adc,ads,prec,sd->type,fv);

            WinEnableWindow(WinWindowFromID(hwnd,DID_START),FALSE);
            WinEnableWindow(WinWindowFromID(hwnd,DID_STOP),TRUE);

            sd->idTimer = WinStartTimer(WinQueryAnchorBlock(hwnd),hwnd,TID_USERMAX - 50,500);
          }
          break;
      case DID_STOP:
          WinStopTimer(WinQueryAnchorBlock(hwnd),hwnd,TID_USERMAX - 50);
          if (sd->sp) {
              if(sd->sp->isError())
                 WinMessageBox(HWND_DESKTOP,HWND_DESKTOP,sd->sp->getError(),"Goal Seeker Error!",
                     24L,MB_OK | MB_ERROR);
              else if (sd->sp->percentComplete() > 99 ) {
                 DosBeep(1440,100);
              } else {
                 sd->sp->stop();
                 WinMessageBox(HWND_DESKTOP,HWND_DESKTOP,sd->sp->getError(),"Goal Seeker Error!",
                     24L,MB_OK | MB_ERROR);
               }
              delete sd->sp;
              sd->sp = NULL;
          }
          WinEnableWindow(WinWindowFromID(hwnd,DID_STOP),FALSE);
          WinEnableWindow(WinWindowFromID(hwnd,DID_START),TRUE);
          break;
       default:
         ret = WinDefDlgProc(hwnd,msg,mp1,mp2);
         break;
       } /* endswitch */
       break;
    case WM_TIMER:
       if ((ULONG)mp1 == sd->idTimer) {
          if (sd->sp && (sd->sp->isError() || (sd->sp->percentComplete() > 99 ))) {
             WinStopTimer(WinQueryAnchorBlock(hwnd),hwnd,TID_USERMAX - 50);
             sd -> idTimer = 0;
             WinPostMsg(hwnd,WM_COMMAND,MPFROMSHORT(DID_STOP),0L);
          } /* endif */
       } else {
          WinDefDlgProc(hwnd,msg,mp1,mp2);
       } /* endif */
       break;
    case WM_GETMODELPTR:
       return (MPARAM)sd->md;
       break;
    case WM_CLOSE:
       for (x = 0; x < count ; x++) {   // remove from the hwnd Array
          if (theHwnds[x] == hwnd) {
             theHwnds[x] = NULL;
          } /* endif */
       } /* endfor */

       if (sd->sp) {
          delete sd->sp;
       } /* endif */
       if (sd->md) {
          delete sd->md;
       } /* endif */
       delete sd;
       WinDismissDlg(hwnd,DID_OK);
       break;
    default:
        ret = WinDefDlgProc(hwnd,msg,mp1,mp2);
      break;
    } /* endswitch */

    return ret;
}

// this routine is called by Mesa when it needs to either check to see if a menu
// item should be enabled or disabled or if if the command should be executed
// A command may be passed to the call-back function even if the command is not
// recognized by the call-back.  Ignore commands that your AddIn does not know
// about and return a 0 for all commands that you do not recognize
int myMenuCallBackRoutine(int command,int function)
{
   int ret = 0; // the default return value
   HWND hwnd;

   switch (function)
   {
      // execute the command
   case 0:
         if (command == solverMenuId) {
             int x = 0;
             Model mo(-1);
             int found = 0;
             // check to see if the model already has a solver open for it
             for (x = 0;x < count ; x++ ) {
                if (theHwnds[x] && (*(Model *)WinSendMsg(theHwnds[x],WM_GETMODELPTR,0,0) ==  mo)) {
                   found = 1;
                   hwnd = theHwnds[x];
                } /* endif */
             } /* endfor */

             if (!found) {
                 hwnd = WinLoadDlg(HWND_DESKTOP,thisAddIn-> getActiveWindow(),
                      SolvDlgProc,(HMODULE)thisAddIn->getHandle(),ID_DLG_GOALSEEK,0L);
             } else {
                 WinSetFocus(HWND_DESKTOP,hwnd);
             } /* endif */
          }
      break;

      // return 1 if the menu should be enabled or a 0 if this command
      // is disabled or
      // if we do not recognize this command.  All other values are
      // reserved.
      case 1:
         // only activate when there is a window
         if ((command == solverMenuId) && thisAddIn -> getActiveWindow())
               ret = 1;
         break;

      // open the help screen
      case 2:
         break;
   }

   return ret;
}

// this function will be called whenever a model is closed
int aboutToClose(void *model_handle)
{
   int x = 0;
   Model mo((int)model_handle);
    HWND hwnd;


   // check to see if the model already has a solver open for it
   for (x = 0;x < count ; x++ ) {
       if (theHwnds[x] && (*(Model *)WinSendMsg(theHwnds[x],WM_GETMODELPTR,0,0) ==  mo)) {
           WinDismissDlg(theHwnds[x],0);  // if so, close it
            theHwnds[x] = NULL;
       } /* endif */
    } /* endfor */

    return 0;
}

void BeepFunction(int numParam, void *stack, void *model_handle)
{
    Model *mo = new Model((int) model_handle);
    int x;

    if (numParam != 2) {  // the number of parameters is incorrect
        for (x = 0; x < numParam ; x++ ) { // clear the stack
            MesaAddInValue aiv;
            mo -> initAddInValue(aiv);
            mo -> popValue(stack,aiv);
            mo -> freeAddInValue(aiv);
        } // endfor
        mo->pushError(stack,21);   // push the error on the stack
                             // 21 is incorrect number of parameters
    } else {
        double a,b;
        int error = 0;
        if (mo->popValue(stack,a)) error = 1;  // pop the duration
        if (mo->popValue(stack,b)) error = 1;  // pop the frequency
        if (!error) {  // if none of the parameters was in error,
            int d = a,f = b;      // Beep
            DosBeep(f,d);
            mo->pushValue(stack,"BEEP");  // We need to push something onto the stack
        } else {
            mo->pushError(stack,26); // push an error onto the stack
        } // endif
    } // endif

    delete mo;
}

void numIn(int numParam, void *stack, void *model_handle)
{
    Model *mo = new Model((int) model_handle);
    MesaAddInValue aiv, aiv2;
    MesaAddInValue *ap;
    int count = 0;


    if (numParam != 2) { // incorrect number of parameters
        for (int x = 0; x < numParam ; x++ ) { // clear the stack
            mo -> initAddInValue(aiv);
            mo -> popValue(stack,aiv);
            mo -> freeAddInValue(aiv);
        } // endfor
        mo->pushError(stack,21);   // push the error on the stack
        delete mo;
        return;
    } // endif

    mo -> initAddInValue(aiv);
    mo -> initAddInValue(aiv2);

    mo -> popValue(stack,aiv);  // get the array
    mo -> popValue(stack,aiv2); // get the search value

    ap = aiv.array;

    for (int r = 0; r < aiv.rows ; r++ ) {  // increment throught the rows and columns
        for (int c = 0; c < aiv.cols ; c++ ) {
            if (ap->type == aiv2.type) {
                switch (ap->type) {
                case errorValueMesaAddInType:
                    if (ap->error == aiv2.error) {
                        count++;
                    } // endif
                    break;
                case stringValueMesaAddInType:
                    if (!strcmp(ap->string,aiv2.string)) {
                        count++;
                    } // endif
                    break;
                case numberValueMesaAddInType:
                    if (ap->number == aiv2.number) {
                        count++;
                    } // endif
                    break;
                } // endswitch
            } // endif
            ap++;
        } // endfor
    } // endfor


    mo -> freeAddInValue(aiv);
    mo -> freeAddInValue(aiv2);

    double result = count;
    mo -> pushValue(stack,count); // push the rsult onto the stack
    delete mo;
}

void layername(int numParam, void *stack, void *model_handle)
{
    Model *mo = new Model((int) model_handle);
    int x;
    double d;
    char buffer[1000];

    if (numParam != 1) {  // this only takes one parameter
        for (x = 0; x < numParam ; x++ ) { // clear the stack
            MesaAddInValue aiv;
            mo -> initAddInValue(aiv);
            mo -> popValue(stack,aiv);
            mo -> freeAddInValue(aiv);
        } // endfor
        mo->pushError(stack,21);   // push the error on
                             // 21 is incorrect number of parameters
    } else {
        double a,b;
        int error = 0;
        if (mo->popValue(stack,d)) error = 1;  // pop a value off
                                           // if it isn;t a number, mthe parameter is wrong
        if (!error) {
            int l = d;
            Address ad(0,0,l); // create an address out of the result
            mo -> addressToString(ad,buffer,1000);  // make it into a string, the string is of the form
                                 // "[$LAYERNAME]$A$1"
            char * c=strrchr(buffer,']');
            *c = 0;          // Make it into "[$LAYERNAME"
            mo->pushValue(stack,&buffer[2]); // push it on the stack
        } else {
            mo->pushError(stack,26);
        } // endif
    } // endif

    delete mo;
}

// initialize everything needed by this addin
void AddInInit(void *handle)
{
    // create an AddIn object from the handle
    thisAddIn = new AddIn((int)handle);

    // check the version number.  This addin only works with 116 or higher
    if (thisAddIn->version() < 116) {
        return;   // don't regiser any new features
    } // endif

    // If there is a Menu... (DOK apps might not have a Menu)
    if (thisAddIn->getMenu())  {
       // Add the Goal seeker to the menu
       solverMenuId = thisAddIn -> addMenuItem("~Goal Seeker...");

       // Register a menu call back so we can get that command when pressed
       thisAddIn -> registerMenuCallBackFunction(myMenuCallBackRoutine);
    } // endif

    // space to hold the dialog window handles
    count = 1;
    theHwnds = (HWND *)malloc(sizeof(HWND)*count);
    theHwnds[0]=NULL;

    // When workbooks are closed we want to close the solver window
    thisAddIn -> registerAboutToCloseFunction(aboutToClose);

    // Register some AddIn functions
    thisAddIn -> registerFunction("NUMIN(","NUMIN","NUMIN(ITEM,RANGE)",
                    "Counts the number of times ITEM appears in the range",
                    "NUMIN(12,A1:C25)",0,numIn);
    thisAddIn -> registerFunction("LAYERNAME(","LAYERNAME","LAYERNAME(LAYER)",
                   "Returns the string representation of the layer",
                   "LAYERNAME(THISLAYER)",
                   0,layername);
    thisAddIn -> registerFunction("BEEP(","BEEP","BEEP(FREQUENCY,DURATION)",
        "Beeps at the specified frequency (hz) for the specified duration (milliseconds)",
		"BEEP(1440,100)",
        0,BeepFunction);

}


// we need to free the hwnd list because all of those windows are now closed
void AddInFree(void *)
{
    free(theHwnds);
    delete thisAddIn;
}


