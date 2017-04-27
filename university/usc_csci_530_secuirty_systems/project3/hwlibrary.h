/**
 * Class for loading library in dynamic fashion independent
 * of OS calls.
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   03/12/2006 - Created
 */

#ifndef __HWLIBRARY_H__
#define __HWLIBRARY_H__

#include <string>

class HwLibrary {

public:
    
    /**
     * C'tor.
     *
     * @param soPath            Shared libary Path
     */
    HwLibrary( const std::string & soPath );

    /**
     * Virtual D'tor.
     */
    virtual ~HwLibrary();

    /**
     * Loads the library. 
     * 
     * Returns 0 if the library is successfully loaded.
     */
    virtual int load(); 
    
    /**
     * Returns true if the library is loaded
     */
    virtual bool isLoaded() const;

    /**
     * Unloads the library
     *
     * Returns 0 if the library is successfully unloaded.
     */
    virtual int unload();

    /** 
     * Resolves the reference of the symbol.
     *
     * @param symbol            Symbol to be resolved
     * @param symbHandle        Symbol Handle
     *
     * Returns 0 if the symbol is resolved.
     */
    virtual int resolve( const std::string & symbol, void *& symbHandle );

private:
    
    /**
     * Returns the last error set by OS functions.
     *
     * @param prepend           Prepend with this message
     */
    std::string getLastError( const std::string & prepend ) const;

    /**
     * Checks for valid handle. Returns 0 if the handle is valid
     */
    int isValidHandle() const;
    
    std::string _soPath;
    bool _loaded;
    void * _handle;
};

#endif //__HWLIBRARY_H__

