
// This is the class that abbstracts all of the functions for interacting with the
// model (worksheet)  You can use it to create new sheets, play with current sheets,
// etc...

#ifndef __MODEL__
#define __MODEL__

#include "extaddin.h"
#include "range.h"

class Model {
   public:
    // The constructors
    // The default constructor creates a NEW model and displays it.
    Model();
    // -1 to get current model, 0 for new model, else the model handle
    Model(int m);
    // simple copy constructor
    Model(const Model &md) { _model = md._model; _redisplay = md._redisplay;};
    // loads a model from a file
    Model(char *name);

    // for interacting with certain functions that need a model handle
    int getModelPtr() { return _model;};
    void setModelPtr(int m) { _model = m;};

    // Functions for accessing/changing the name of the model.
    Model & getName(char * buf,int max);
    Model & setName(char *);

    // Functions to force a save of the model
    Model & save();
    Model & saveAs(char * name);

    // Loads a model
    int load(char * name);   // returns true if successfull

    // Many actions (such as setCellToValue) can be set to recalc and then
    // redisplay or not do anything.  Use these functions to
    // set that.  (0 = recalc/redisplay, -1 = do nothing)
    // (*) Note that there are other flags too.(used internally)  the general
    //     format is what flags NOT to do.  Example -1 is 0xFFFFFFFF or don't
    //     do any of these flags.  0 means do all of the defaults.
    //     Experiment with knowledge at your own risk.
    Model & setToRedisplayOnAction(int t = 0) { _redisplay = t; return *this;};
    Model & setToNotRedisplayOnAction() { _redisplay = -1; return *this;};

    // Takes an address object and converts it to a string.
    Model & addressToString(Address &a,char * buf,int max);
    // Takes a string and converts it to an Address
    Address stringToAddress(char * buf);

    // Does the same thing with ranges.
    Model & rangeToString(Range &r,char * buf, int max);
    Range stringToRange(char * buf);

    // Selects a named range.
    // (*) This only works if the model is the current active window
    Model & selectNamedRange(char *buf);

    // These methods are for accessing/setting the
    // values of a cell directly.  Setting the cells to a value
    // will return 0 if it is successful or non-zero if not.
    // The getCell* will return 0 if successful or non-zero if
    // it wasn't successfull or if the value type is different
    // than what was asked for. (Example: doing a getCellError
    // on a cell with a proper value in it)
    // (*) Empty cells are cells with Error type 0
    // (*) There are other error types, (Last I checked, there were
    //     over 40) experiment at your own risk.
    int setCellValue(const Address &ad,double db);
    int setCellValue(const Address &ad,char * str);
    int setCellError(const Address &ad,int error);

    int getCellValue(const Address &ad,double &db);
    int getCellValue(const Address &ad,char * &str, int max);
    int getCellError(const Address &ad,int &error);

    // The cell string is the string that a user would type
    // into the cell, wither that be a number, like "3.14159"
    // or a formula, like "=if(0>1,A1,A2)".  The cell output
    // is what is displayed in the cell.  You can use the
    // setCellString to set the formula in the string.
    int setCellString(const Address &ad,char * str);
    Model & getCellString(const Address &ad,char * &str, int max);
    Model & getCellOutput(const Address &ad,char * &str, int max);

    // These are used for dealing with the size of the layer
    // (*) Note that these are the VISIBLE extents of the sheet.
    //     Mesa allows cells to exist off of the visible region.
    int setExtentsForLayer(int layer,int rows,int columns);
    int getExtentsForLayer(int layer,int &rows,int &columns);

    // Methods for playing with the layers.  Add layer always
    // add's it as the last layer.
    int getNumberOfLayers();
    Model & addLayer();

    // These methods are for manipulating the size of columns
    // and rows.
    int setRowSize(int layer,int row, int size);
    int getRowSize(int layer,int row);
    int setColSize(int layer,int col, int size);
    int getColSize(int layer,int col);

    // Forces a recalc of the model
    Model & recalc();

    // forces a redisplay of the model.
    Model & redisplay();

    // These methods allow you to save data (BLOBS) (Binare Lengths of Bytes)
    // in the model to be recalled later.
    // (*) These were EXTREMELY broken in 101 (they caused a crash) so make sure
    //     you check the version number if you have any intentions of using them.
    Model & setBlob(char * name, int len, void * data);
    int getBlob(char * name,int max, void * data); // returns number of bytes put in data

    // These functions are used inside of functions that are regstered
    // by the addin.  They work like the CellValue() methods except they
    // operate on the evaluation stack.
    // (*) Stacks are FILO so if the funtion is FOO(1,2,3,4), the first
    //     number poped from the stack is 4 and the last number is 1.
    // (*) If you ask for a certain type(like the double) and the result
    //     is not a double, The value is pushed back onto the stack so
    //     you can query it properly.
    // (*) These were also broken in 101.  Be careful.
    int pushValue(void * stack,double db);
    int pushValue(void * stack,char * str);
    int pushError(void * stack,int error);

    int popValue(void * stack,double &db);
    int popValue(void * stack,char * &str, int max);
    int popError(void * stack,int &error);


    // stuff for dealing directly with the MesaAddInValue structure. Use
    // these for dealing with arrays.  If someone has a briliant way to deal
    // with the array issue so that methods like popValue(stack,array) can
    // be done, let me know.
    void initAddInValue(MesaAddInValue &aiv);
    void freeAddInValue(MesaAddInValue &aiv);

    void pushValue( void * stack, MesaAddInValue &aiv);
    void popValue( void * stack, MesaAddInValue &aiv);

    void getCellValue( Address & ad, MesaAddInValue &aiv);
    void setCellValue( Address & ad, MesaAddInValue &aiv);



    int operator == (const Model &mo) { return (_model == mo._model);};

   private:
    int _model;
    int _redisplay;
};

#endif
