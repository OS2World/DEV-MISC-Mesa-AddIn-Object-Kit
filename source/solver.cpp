
#define INCL_DOSPROCESS
#define INCL_DOSERRORS
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "range.h"
#include "model.h"
#include "solver.h"

double waitForNewValue(Solver &sv, Model &model, Address &ad, double oldval, int &maxtime)
{
    int totaltime = 1;
    double newValue;
    model.getCellValue(ad,newValue);

    while (( totaltime < maxtime)  && (newValue == oldval)) {
         DosSleep(50);
         totaltime += 50;
         model.getCellValue(ad,newValue);
         if (sv.shouldStop()) {
             DosExit(0,0);  //kill the thread
         } // endif
    }

    if (totaltime < maxtime) {
       maxtime = 0;
    }

    return newValue;
}

double func(Solver & sv,Model &model, Address & x, Address & y, double newX)
{
    double oldX;
    double oldY;
    double newY;
    int maxtime = 30000;  // 30 second wait time

    model.getCellValue(x,oldX);
    model.getCellValue(y,oldY);

    if (oldX == newX) return oldY;   // the new X value is the same as the old one

    model.setCellValue(x,newX);
    newY = waitForNewValue(sv,model,y,oldY,maxtime);
    return newY;
}

int bracketRoot(Solver & sv, Model &model, Address & x, Address & y, double &lft, double &rght, double offset)
{
    int j;
    double f1,f2;
    double gap;

    if (lft > rght) {
       f1 = lft;
       lft = rght;
       rght = f1;
    }
    if (lft == rght) rght += 0.1;
    gap = rght - lft;
    f1 = func(sv,model,x,y,lft) - offset;
    f2 = func(sv,model,x,y,rght) - offset;
    for (j = 0 ; j < 100 ; j++) {
         if (f1 * f2 < 0.0) return 1;
         if (fabs(f1) < fabs(f2)) {
             gap *= 2;
             rght = lft;
             lft -= gap;
             f2 = f1;
             f1 = func(sv,model,x,y,lft) - offset;
         } else {
             gap *=2;
             lft = rght;
             rght += gap;
             f1 = f2;
             f2 = func(sv,model,x,y,rght) - offset;
         }
    }
    return 0;
}

int bisectToRoot(Solver & sv,Model &model,Address & x, Address & y, double &lft, double &rght, double offset, double acc)
{
   int j;
   double dx,f,fmid,xmid,rtb;

    f = func(sv,model,x,y,lft) - offset;
    dx = rght - lft;
    xmid = (rght+lft)/2;   // find midpoint;
    for ( j = 0 ; j < 40 ; j ++ ) {
         fmid=func(sv,model,x,y,xmid) - offset;
         if (((fmid < 0.0)  && (f < 0.0)) || ((fmid > 0.0)  && (f > 0.0))){
            lft = xmid;
            f = fmid;
         } else {
            rght = xmid;
         }
         dx = rght - lft;
         if (fabs(dx) < acc || fmid == 0) {
             lft=rght=xmid;
             return 1;
         }
         xmid = (rght+lft)/2;   // find midpoint;
    }
    return 0;
}

void _System solveThread(unsigned long p)
{
    Solver * sp = (Solver *)p;
    double left,right;

    sp->model().getCellValue(sp->changeCell(),left);
    right = left;

    if (!bracketRoot(*sp,sp->model(),sp->changeCell(), sp->finalCell(), left, right, sp->finalValue())) {
       sp->setError("Could not bracket value!  Try a better guess.");
       return;
    }
    if (!bisectToRoot(*sp,sp->model(),sp->changeCell(),sp->finalCell(),
        left,right,sp->finalValue(),sp->precision())) {
        sp->setError("Could not find value! Try a better guess or lowering the precision.");
        return;
    }
    sp->setPercentComplete(100);
}



int bracketMinMax(Solver &sv,Model &model, Address & x, Address & y,
               double &lft, double &fl,double &rght, double &fr, int min)
{
    int j;
    double fmid, mid;
    double gap;

    if (lft > rght) {
       fl = lft;
       lft = rght;
       rght = fl;
    }
    if (lft == rght) rght += 0.1;
    fl = func(sv,model,x,y,lft);
    fr = func(sv,model,x,y,rght);
    mid = (rght + lft) / 2;
    gap = (rght - lft) / 2;
    fmid = func(sv,model,x,y,mid);

    for (j = 0 ; j < 100 ; j++) {
         if (( fl > fmid ) && ( fr > fmid ) && min) return 1;
         if (( fl < fmid ) && ( fr < fmid ) && !min) return 1;
         if (((fl < fr) && min) || ((fl > fr) && !min)) {
             gap *= 2;
             rght = mid;
             mid = lft;
             lft -= gap;
             fr = fmid;
             fmid = fl;
             fl = func(sv,model,x,y,lft);
         } else {
             gap *=2;
             lft = mid;
             mid = rght;
             rght += gap;
             fl = fmid;
             fmid = fr;
             fr = func(sv,model,x,y,rght);
         }
    }
    return 0;
}



int bisectToMinMax(Solver & sv,Model &model,Address & x, Address & y,
                double &lft,double & fl,double &rght,double &fr, double acc, int min)
{
    double gap = (fabs(rght - lft)) / 7;
    double dx = lft;
    double fx;
    double oldfx, oldoldfx;

    oldfx = fx = oldoldfx = fl;

    for (int j = 0; j < 40; j++ ) {
        while ((dx < rght) && (((oldfx >= fx) && min) ||((oldfx <= fx) && !min)))  {
            oldoldfx = oldfx;
            oldfx = fx;
            dx += gap;
            fx = func(sv,model,x,y,dx);
        } /* endwhile */

        rght = dx;
        fr = fx;

        dx -= 2*gap;

        if (dx < lft) {
           dx = lft;
        } /* endif */

        lft = dx;

        if (fabs(lft-rght) < acc) {
           lft = rght;
            return 1;
        } /* endif */

        fl = oldfx = fx = oldoldfx;
        gap = (fabs(rght - lft)) / 7;
    } /* endfor */
    return 0;
}


void _System minimizeThread(unsigned long p)
{
    Solver * sp = (Solver *)p;
    double left,right, fl,fr;

    sp->model().getCellValue(sp->changeCell(),left);
    right = left;

    if (!bracketMinMax(*sp,sp->model(),sp->changeCell(), sp->finalCell(), left, fl,right,fr, 1)) {
       sp->setError("Could not bracket minimum!  Try a better guess.");
       return;
    }
    if (!bisectToMinMax(*sp,sp->model(),sp->changeCell(),sp->finalCell(),
        left,fl,right,fr,sp->precision(),1)) {
        sp->setError("Could not find minimum! Try a better guess or lowering the precision.");
        return;
    }
    sp->setPercentComplete(100);

}

void _System maximizeThread(unsigned long p)
{
    Solver * sp = (Solver *)p;
    double left,right, fl,fr;

    sp->model().getCellValue(sp->changeCell(),left);
    right = left;

    if (!bracketMinMax(*sp,sp->model(),sp->changeCell(), sp->finalCell(), left, fl ,right, fr, 0)) {
       sp->setError("Could not bracket maximum!  Try a better guess.");
       return;
    }
    if (!bisectToMinMax(*sp,sp->model(),sp->changeCell(),sp->finalCell(),
        left,fl,right,fr,sp->precision(),0)) {
        sp->setError("Could not find maximum! Try a better guess or lowering the precision.");
        return;
    }
    sp->setPercentComplete(100);

}


Solver :: Solver(Model & theModel, Address & change, Address &final,
               double precision, int type, double value) :
    _model(theModel),chng(change),finl(final),prec(precision),
    tp(type),fval(value)
{
    error[0] = 0;
    percentComp = 0;
    dostop = 0;
    switch (tp) {
    case 0:
       DosCreateThread(&tid,&solveThread,(ULONG)this,0L,16384);
       break;
    case 1:
       DosCreateThread(&tid,&minimizeThread,(ULONG)this,0L,16384);
       break;
    case 2:
       DosCreateThread(&tid,&maximizeThread,(ULONG)this,0L,16384);
       break;
    } /* endswitch */
}

    // stop the solving process, blocks until the solving thread is done
Solver & Solver :: stop()
{
//    int rc = DosKillThread(tid);
      // don't use DosKillThread.  it can leave MesaCore in an unknown state
      // and possibly cause crashes
    dostop = 1;
    setError("Stopped before completion!");
    return *this;
}

Solver :: ~Solver()
{
    dostop = 1;
    DosWaitThread(&tid,DCWW_WAIT);
}
