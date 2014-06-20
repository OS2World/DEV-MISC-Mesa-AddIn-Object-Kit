/**************************************************************************
 *                                                                        *
 *                                                                        *
 *          This code is copyright (c) 1995                               *
 *                     Athena Design, Inc.                                *
 *                                                                        *
 *                                                                        *
 *                ALL RIGHTS RESERVED                                     *
 *                                                                        *
 *                                                                        *
 *                                                                        *
 *                                                                        *
 *                                                                        *
 **************************************************************************/

/*
	This is the external file that must be used be include'd by custom
	AddIn's that will be linked into Mesa.  This file may be included in
	either a C or C++ program.
	
	All functionality is performed via a call the the performAddInFunction()
	function.  The first parameter is an integer that denotes the function to
	be performed.  Additional parameters and the return value is different for each
	function.  Please see the "forthcoming" documentation.
	
	This file is under construction.  All of the documented calls are implemented
	unless there are specific implementation notes within the documentation.  The
	functions that are labeled "FINISH" simply need to be hooked up to the
	model code.  The functions labeled ???? may be finished or may need additional
	work at the model or controller level to finish them.  4/26/95 DPP
*/

#ifndef _MH_ExternalAddIn_h

#define _MH_ExternalAddIn_h

#include <stdarg.h>

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(__HIGHC__) || !defined(M2Z)
extern int performAddInFunction(int,...);
#else
extern int _System performAddInFunction(int,...);
#endif

// Routines for dealing with Mesa's memory management instead of the compilers.
// They all work like thier C equivilents
extern void MFree(void *);
extern char *MRealloc(void *,int);
extern char *MMalloc(int);


// the current list of commands

// HWND performAddInFunction(getMenuWindowMesaAddIn);
// returns the handle to the application menu or 0 if there is no
// application menu
#define getMenuWindowMesaAddIn              1

// int performAddInFunction(registerMenuCallBackMesaAddIn, module handle, function address)
// returns 1
// registers a call-back to the menu handling routine.  The module handle is the paramter
// passed to the AddIn when its AddInInit(void *) is called.  An AddIn can change the routing
// called for the menu call-back by calling this routing again or may de-register the menu
// callback by passing a NULL as the function address.
// the function is called in the following format:
// function(int command,int type) where:
// command is the menu command (an integer between 27000 and 32767 that the AddIn associates)
// with a particular menu command.  An AddIn will be called with other AddIn's commands and
// should ignore these commands and return a 0 in all circumstances
// the type is an integer denoting the action to take:
// 0 - execute the menu command - return 1 if the command was executed
// 1 - enabled? return a 1 if the AddIn can execute the command (i.e., the menu should be enabled)
// 2 - Open a help dialog for the menu item
#define registerMenuCallBackMesaAddIn       2


// void *performAddInFunction(getActiveModelMesaAddIn)
// returns a handle to the currently active model or NULL if there is no currently active
// model
#define getActiveModelMesaAddIn             3

// char *performAddInFunction(getModelNameMesaAddIn,void *model_handle)
// returns a pointer to a string containing the name of the model refered to by the
// handle.  NULL is returned if the model has not been named.  The returned string
// must be free'd from storage with a call to MFree()
#define getModelNameMesaAddIn               4

// void performAddInFunction(setModelNameMesaAddIn, void *model_handle,
//		const char *new_name,int action)
// changes the name of the model to the new name (which should be a complete path name)
// if 0 is passed as the action, the windows displaying the model are redisplayed with the
// new model name.  If -1 is passed as the action, the windows are not redisplayed
#define setModelNameMesaAddIn               5

// void *performAddInFunction(openModelMesaAddIn, const char *file_name)
// If the named model is currently open, it is brought to the front.  If it is not open,
// it is loaded off of disk.  If it cannot be opened, NULL is returned, otherwise the
// model handle is returned
#define openModelMesaAddIn                  6

// char *performAddInFunction(addressToStringMesaAddIn,void *model_handle,
//	int row,int col,int layer)
// returns a character string that is the absolute address in row,col,layer
// this string must be MFree'd after it is used
#define addressToStringMesaAddIn            7

// char *performAddInFunction(rangeToStringMesaAddIn, void *model_handle,
// int row,int col,int layer,int row,int col,int layer)
// returns a character string that is the absolute range in row,col,layer to
// row,col,layer
// this string must be MFree'd after it is used
#define rangeToStringMesaAddIn              8

// void performAddInFunction(selectMesaAddIn, const char *range_name)
// selects the named range on the current window
#define selectMesaAddIn                     9

// int performAddInFunction(getCurrentAddressMesaAddIn, int *row,int *col,
//	int *layer) returns 1 if there is a valid selection (i.e., an open window
// with a spreadsheet layer on top) and thus the row, column, and layer are set
// correctly
#define getCurrentAddressMesaAddIn          10

// int performAddInFunction(getCurrentRangeMesaAddIn, int *row,int *col,int *layer,
//	int *row,int *col,int *layer) returns 1 if there is a valid range selection
// (an open window with a sheet layer showing with a range selected (rather than a single
// cell).  The rectangle of the first part of the selected range is placed into the
// correct integers
#define getCurrentRangeMesaAddIn            11

// void performAddInFunction(getCellValueMesaAddIn, void *model_handle,MesaAddInValue *aiv)
// initialize the MesaAddInValue, set the row, column, and layer and call this
// function.  The value of the cell is set and returned.  For NULL cells or
// cells with no value, the value is a type of error and the error value is 0
#define getCellValueMesaAddIn               12

// int performAddInFunction(setCellValueMesaAddIn, void *model_handle,MesaAddInValue *aiv,
// 		int action) - sets the given cell to the given value.  Set the value and address
// in the MesaAddInValue, and call this funtion to set the value of a given cell.  An error
// is returned.  0 if successful or non-zero if there was an error (the cell address was out of
// range or the range was protected.  A 0 in the action will result in a recalc and a redisplay
// where a -1 will result in neither
#define setCellValueMesaAddIn               13

// int performAddInFunction(setExtentsMesaAddIn, void *model_handle,int layer,int maxRow,
//		int maxCol, int action) - sets the maximum size of a given layer in the model.
// The model must
// have the layer in question as valid for this function to have any effect.  Set
// action to 0 to redisplay the worksheets or -1 to avoid redisplay events.  Returns 0
// if successful, otherwise an error number
#define setExtentsMesaAddIn                 14

// void performAddInFunction(setBlobMesaAddIn, void *model_handle,const char *name,
//		int len,const void *blob_data) - sets a named blob (Binary Length Of Bytes) in the
// model that is saved with the model.  The AddIn may save AddIn specific information
// with the model via this mechanism
#define setBlobMesaAddIn                    15

// int performAddInFunction(getBlobMesaAddIn, void *model_handle,const char *name,
//		int *len,void **blob_data) - retrieves a named blob from the model.  Returns
// 1 if a blob with the given name was found.  The length of the blob is put in the
// length and the blob_data is set to an allocated block of memory that contains the
// blob data.  blob_data must be MFree'd when it is no longer needed
#define getBlobMesaAddIn                    16

// void *performAddInFunction(newModelMesaAddIn)
// creates a new model and displays it and returns a handle to the new model
// returns 0 if a model cannot be created
#define newModelMesaAddIn                   17

// int performAddInFunction(setColumnSizeMesaAddIn, void *model_handle, int layer, int column,
//		int size, int action) - sets the particular column to a particular size in a given
// layer.  If action is 0, workbooks will be redisplayed, -1 no redisplay.  Size is in points.
// returns 0 if successful or error number
#define setColumnSizeMesaAddIn              18

// int performAddInFunction(setRowSizeMesaAddIn, void *model_handle, int layer, int row,
//		int size, int action) - sets the particular row to a particular size in a given
// layer.  If action is 0, workbooks will be redisplayed, -1 no redisplay.  Size is in points
// returns 0 if successful or error number
#define setRowSizeMesaAddIn                 19

// int performAddInFunction(getColumnSizeMesaAddIn, void *model_handle,int layer,int col)
// returns the size of the column in points.
#define getColumnSizeMesaAddIn              20

// int performAddInFunction(getRowSizeMesaAddIn, void *model_handle,int layer,int row)
// returns the size of the row in points.
#define getRowSizeMesaAddIn                 21

// int performAddInFunction(getExtentsMesaAddIn, void *model_handle,int layer, int *nrows,
//		int *ncols) - gets the size of the given layer.  If the layer does not exist in the
// model, 0 is returned, otherwise nrows and ncols are set correctly
#define getExtentsMesaAddIn                 22

// void performAddInFunction(redisplayMesaAddIn, void *model_handle) - causes all the windows
// that are watching a given workbook to redisplay
#define redisplayMesaAddIn                  23

// void performAddInFunction(recalcMesaAddIn, void *model_handle) - causes the model to
// recalculate and redisplay.  The recalculation is done in the background and may be interupted
// by other user activity in the model
#define recalcMesaAddIn                     24

// int performAddInFunction(setCellStringMesaAddIn, void *model_handle,const char *string,
//		int layer,int row, int col,int action) - sets a given cell to a string.  The string
// is parsed (checked for being a formula, etc) as if the user typed the string.  action of
// 0 will result in a redisplay and recalc (if recaclc is enabled, -1 no action.  0 return
// indicates success, otherwise an error number will be returned
#define setCellStringMesaAddIn              25

// char *performAddInFunction(getCellStringMesaAddIn, void *model_handle, int layer, int row,
//		int col) - returns the contents of the "formula view" of the cell.  This string must
// be MFree'd when it is no longer used
#define getCellStringMesaAddIn              26

// char *performAddInFunction(getCellOutputMesaAddIn, void *model_hanlde, int layer, int row,
//		int col) - returns the text of what will be displayed in a cell in the exact format
// that it would appear in the cell.  The string must be MFree'd when it is no longer needed
#define getCellOutputMesaAddIn              27

// void performAddInFunction(registerFunctionMesaAddIn, const char *func_name,
//		const char *fbName,const char *fbPrototype, const char *fbDescription,
//		const char *fbExample, int *fbMembers, void * callBack)
// func_name - the name of the function, eg: "DOG("
// fbName - optional (must be NULL if unused) - name as appears in formula bldr: "DOG"
// fbPrototype - optional (must be NULL if unused) - prototype in formula builder
// fbDescription - optional (must be NULL if unused) - description in formula builder
// fbExample - optional (must be NULL if unused) - text example for formula builder
// fbMembers - optional (must be NULL if unused) - 0 terminated list of function groups for fb
// callBack - the address of the function that executes the AddIn function
// this will be called with the following parameters:
// function(int numberOfParameters, void *stack, void *model_handle)
// use the popElementMesaAddIn to remove values from the
// evaluation stack and process them and push the result back on the stack with pushValueMesaAddIn
#define registerFunctionMesaAddIn           28

// void performAddInFunction(popElementMesaAddIn,void *stack, void *model_handle, MesaAddInValue *)
// pops an element off the evaluation stack.  the MesaAddInValue must be initialized and free'd
// after use
#define popElementMesaAddIn                 29

// void performAddInFunction(pushValueMesaAddIn,void *stack, void *model_handle, MesaAddInValue *)
// pushes an element onto the evaluation stack.
#define pushValueMesaAddIn                  30

// int performAddInFunction(registerDoubleClickMesaAddIn, void * module_handle,
//		void *function_address) - registers the call-back for double-clicking on a cell
// if the user double-clicks on a cell in a sheet view, the AddIn is called in the following
// way: void function(void *model_handle,int layer, int row, int column)
#define registerDoubleClickMesaAddIn        31

// int performAddInFunction(registerAboutToSaveMesaAddIn, void *module_handle,
//		void *function_address) - registers the call-back for the pre-save operation
// just before a model is saved out to disk, this call-back is called.  This is an
// opportunity for the AddIn to place information with the model or to verify that the
// model has the correct contents.  If the AddIn returns '0' the model is saved (assuming
// that other addin's return 0), otherwise, the save operation is aborted and the
// saveBlockedByAddInError error is returned
// int function(void *model_handle)
#define registerAboutToSaveMesaAddIn        32

// int performAddInFunction(registerOpenedMesaAddIn, void *module_handle,
//		void *function_address) - registers the call-back for the post-open operation
// this call-back is called each time a model is opened from disk.  This is an
// oportunity for an AddIn to check to see if a Model could use the AddIn's services
// for example, by checking the BLOB info for a registered blob
// void function(void *model_handle)
#define registerOpenedMesaAddIn             33

// int performAddInFunction(registerAboutToCloseMesaAddIn, void *module_handle,
//		void *function_address) - registers the call-back for the pre-close operation
// this call-back is called just before the last window to a model is closed.  This
// in an opportunity for the AddIn to block the close operation or to place data in the
// model before it is closed.  Return 1 to block the close operation
// int function(void *model_handle)
#define registerAboutToCloseMesaAddIn       34

// int performAddInFunction(registerDataEnteredMesaAddIn, void *module_handle,
//		void *function_address) - registers the call-back for the pre-enter data operation
// this call-back is called just before data is entered into a cell.  This gives the addin
// the opportunity to do data validation and optionally block the entry of data into
// the cell.  return 1 to block data entry or 0 to allow it.  A entryBlockedByAddInError
// is returned if the data addin blocks the data
// int function(void * model_handle,int layer,int row,int column,const char *data)
#define registerDataEnteredMesaAddIn        35

// int performAddInFunction(registerPMCallBackMesaAddIn, module handle, function address)
// returns 1
// registers a call-back to the PM Message handling routine.  The module handle is the paramter
// passed to the AddIn when its AddInInit(void *) is called.  An AddIn can change the routing
// called for the PM call-back by calling this routing again or may de-register the PM
// callback by passing a NULL as the function address.
// the function is called in the following format:
// MRESULT function(HWND window,ULONG msg,MPARAM mp1,MPARAM mp2)
// Messages greater than WM_USER + 1000 get passed to all addins.  It is the addin's job
// to choose a unique PM Message number (sorry guys, no tips).  The MRESULTS are or'ed
// together so if you don't recognize the message, return 0
#define registerPMCallBackMesaAddIn         36

// HWND performAddInFunction(getActiveWindowMesaAddIn)
// returns the HWND for the currently active spreadsheet window or 0 if there is no window
#define getActiveWindowMesaAddIn            37

// HWND performAddInFunction(getApplicationWindowMesaAddIn)
// returns the HWND for the application window.  This function can return 0 if there
// is no application window (i.e., the addin is running in a custom application via MOLI)
#define getApplicationWindowMesaAddIn       38

// void performAddInFunction(initValueMesaAddIn, MesaAddInValue *)
// initializes the data structure to default values.  Must be done before the data
// structure is used in any calls
#define initValueMesaAddIn                  39

// void performAddInFunction(freeValueMesaAddIn, MesaAddInValue *)
// releases any storage used by the MesaAddInValue.  This must be called before
// the MesaAddInValue goes out of scope
#define freeValueMesaAddIn                  40

// int performAddInFunction(getNumberOfLayersMesaAddIn,void *model_handle)
// returns the number of layers in the model
#define getNumberOfLayersMesaAddIn          41

// void performAddInFunction(addALayerMesaAddIn,void *model_handle,int action)
// adds a layer to the model.  if the "action" parameter is 0, redisplay takes place
// if the action parameter is -1, no redisplay takes place
#define addALayerMesaAddIn                  42

// int performAddInFunction(saveModelAddIn, void *model_handle)
// call to save the model.  Returns 0 if successful or error number
#define saveModelAddIn                      43

#define stringToAddressMesaAddIn            44
// int performAddInFunction(saveModelAddIn, void *model_handle,char *str,int *r, int *c, int *l)
// returns 1 if sussessful

#define stringToRangeMesaAddIn              45
// int performAddInFunction(saveModelAddIn, void *model_handle,char *str,int item,
//                          int *r1, int *c1, int *l1,int *r2, int *c2, int *l2)
// returns 0 if unsussessful, the total item count if it is (multi-ranges)

#define getVersionNumberMesaAddIn           46
// int performAddInFunction(getVersionNumberMesaAddin)
// returns the Version number of Mesa.  This is the build number found
// on the About panel of the Mesa 2 Application.

typedef struct _MesaAddInValue
{
	int row;
	unsigned short column;
	unsigned short layer;
	int type;
	double number;
	char *string;
	int error;
    unsigned short rows;
    unsigned short cols;
    _MesaAddInValue * array;
} MesaAddInValue;

#define errorValueMesaAddInType 0
#define stringValueMesaAddInType 1
#define numberValueMesaAddInType 2
#define arrayValueMesaAddInType 3

#ifdef __cplusplus
}
#endif

// ifndef _MH_ExternalAddIn_h
#endif
