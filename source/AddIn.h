
// This is the basic AddIn class.  It handles many of the aspects that
// would need to be done on a more global scale such as registering
// handlers and dealing with menus.  To use this, just create an object
// with the constructors argument being the handle passed in the
// AddInInit(void *) function.

#ifndef __ADDIN__
#define __ADDIN__

class Range;
class Address;

class AddIn {
   public:

    // this is the connstructor.  Just pass in the handle given
    // in the AddInInit() method.
    // (*)  the model handle is actually the HMODULE for the dll
    //      use this knowledge at your own discretion because it
    //      may change in future versions.
    AddIn(int mh) { mod_handle = mh;};

    // gets the handle for the module
    int getHandle() { return mod_handle;};

    // These functions are used for dealing with the application level
    // menu.  The getMenu() function just returns the HWND to the application
    // menu.  If you use this, you have to make sure you don't overwrite any
    // menu ID's (or duplicate them) and also add submenus yourself
    HWND getMenu(); // get the HWND to the application window

    // This function takes a string arguement and adds the item to
    // the AddIn submenu if it exists.  If it doesn't exist, it
    // creates the submenu.  It also does all the checking to
    // make sure the ID's don't exist, etc...  It returns the
    // ID assigned to the item so you can use it in your command
    // handler. (see registerMenuCallBackFunction(void *pfn))
    int addMenuItem(char * str);   // returns the menu ID for the menu item

    // This gets the internal build number of the application.  Check this
    // during init to make sure it supports features you intend to use.
    // Also, versions prior to 116 were VERY broken, if working at all,
    // so you may want to check for this.
    int version();

    //returns the HWND of the application window
    HWND getApplicationWindow();

    //returns the HWND of the current spreadsheet window
    HWND getActiveWindow();

    // these functions are used to register functions
    // that are called when certain actions or events
    // occur.  Look in EXTADDIN.H for the parameter list
    // for each function.
    // (*) Right now, all functions are called with C-Set's
    //      _Optlink linkage so only C-Set can be used to
    //      use these functions.  This may change if/when
    //      we change compilers or if we decide to use
    //      _System linkage.  If that happens, the addin
    //      will break until a re-compile of the addin.
    void registerMenuCallBackFunction(void *pfn);
    void registerPMCallBackFunction(void *pfn);
    void registerDataEnteredFunction(void *pfn);
    void registerAboutToCloseFunction(void * pfn);
    void registerJustOpenedFunction(void * pfn);
    void registerAboutToSaveFunction(void * pfn);
    void registerDoubleClickFunction(void * pfn);

    // Returns the current address or range.  It's up to
    // you to check the current model that these belong to.
    Address getCurrentAddress();
    Range getCurrentRange();


    // This allows you to register a function to be used in the
    // spreadsheet.
    // (*) Right now, adding the stuff to the formula builder
    //     is broken (the formula builder ignores it) so you don't need to
    //     specify it, but you might want to specify it anyway so
    //     if we add support for it, it works without any changes
    //     on your part.
    // (*) Function names always end in a (.  For example the name of
    //     the SIN function is SIN(.  Don't forget the paren when
    //     specifying the name or it wont work.
    void registerFunction(char *func_name,char *fbName,char *fbPrototype, char *fbDescription,
		char *fbExample, int *fbMembers, void * callBack);

   private:
    int mod_handle;
};

#endif
