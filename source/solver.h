
#ifndef __SOLVER__
#define __SOLVER__

#include <string.h>

class Address;
class Model;

class Solver {
    public:
/* Valid types are              */
/* 0 - Set cell to value        */
/* 1 - Minimum (value ignored)  */
/* 2 - Maximum (value ignored)  */
    Solver(Model & theModel, Address & change, Address &final,
               double precision, int type = 0, double value = 0.0);
    ~Solver();

    // stop the solving process
    Solver & stop();

    // get the solver parameters.
    Address &changeCell() {return chng;};
    Address &finalCell() {return finl;};
    double precision() {return prec;};
    double finalValue() {return fval;};
    int type() {return tp;};
    Model &model() {return _model;};

    // set a query how far the solution is along
    int percentComplete() { return percentComp;};
    Solver & setPercentComplete(int v) { percentComp = v; return *this;};

    int isError() { return (int)error[0];};
    Solver &setError(char *er) { strcpy(error,er); return *this;};
    char * getError() { return error;};

    int shouldStop() { return dostop;};

    private:
    Solver() {};   // can't create a solver without parameters
    Solver(const Solver &) {};  // can't copy a solver object

    Address chng;
    Address finl;
    double fval;
    int tp;
    double prec;
    Model _model;
    int percentComp;

    int dostop;

    char error[1000];
    TID tid;
};
#endif
