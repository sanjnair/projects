#include "hwlibrary.h"
#include "hwconstant.h"
#include "hwutil.h"

#ifdef _WIN32
    #include <windows.h> 
#else
    #include <dlfcn.h>
#endif

/**
 * C'tor.
 *
 * @param soPath            Shared libary Path
 */
HwLibrary::HwLibrary( const std::string  & soPath ) {
    _soPath = soPath;
    _loaded = false;
    _handle = NULL;
}

/**
 * Virtual D'tor.
 */
HwLibrary::~HwLibrary() { }

/**
 * Loads the library. 
 * 
 * Returns 0 if the library is successfully loaded.
 */
int HwLibrary::load() {
    int status = 0;    
    std::string errorMsg; 

    if( _soPath.length() > 0 ) {

        #ifdef _WIN32
            HINSTANCE winHandle = LoadLibrary( _soPath.c_str() );
            if( NULL != winHandle ) {
                _handle = winHandle;
            }
        #else
            _handle = dlopen( _soPath.c_str(), RTLD_NOW|RTLD_GLOBAL );

        #endif

        if( NULL == _handle ) {
            errorMsg  = "Unable to open library " + _soPath;
            errorMsg = getLastError( errorMsg );
            HwUtil::setLastErrorMessage( errorMsg );
            status = HwConstant::ERROR_LIBRARY_LOAD;
        } else {
            _loaded = true;
        }
    } else {
        HwUtil::setLastErrorMessage( "Libary path is incorrect (zero length)" );
        status = HwConstant::ERROR_LIBRARY_LOAD;
    }
    
    return status;
}

/**
 * Returns true if the library is loaded
 */
bool HwLibrary::isLoaded() const {
    return _loaded;
}

/**
 * Unloads the library
 *
 * Returns 0 if the library is successfully unloaded.
 */
int HwLibrary::unload() {
    
    int status = 0; 
    std::string errorMsg;

    if( NULL != _handle ) {
        #ifdef _WIN32
            status = FreeLibrary( (HINSTANCE) _handle );
            
            //success will return non-zero value
            if( 0 == status ) {
                status = HwConstant::ERROR_LIBRARY_LOAD;
            } else {
                status = 0;
            }
        #else
            status = dlclose( _handle );
        #endif

        if( 0 != status ) {
            errorMsg = "Unable to close library for " + _soPath;
            errorMsg = getLastError( errorMsg );
            HwUtil::setLastErrorMessage( errorMsg );
            status = HwConstant::ERROR_LIBRARY_LOAD;
        } else {
            _loaded = false;
        }
    } 
    
    return status;
}

/** 
 * Resolves the reference of the symbol.
 *
 * @param symbol            Symbol to be resolved
 * @param symbHandle        Symbol Handle
 *
 * Returns 0 if the symbol is resolved.
 */
int HwLibrary::resolve( const std::string & symbol, void *& symbHandle ) {
    int status = 0; 
    std::string errorMsg;

    status = isValidHandle();
    
    if( 0 == status ) {
        symbHandle = NULL;

        #ifdef _WIN32
            symbHandle = GetProcAddress( (HINSTANCE) _handle, symbol.c_str() );
        #else
            symbHandle = dlsym( _handle, symbol.c_str() );
        #endif

        if( NULL == symbHandle ) {
            errorMsg  = "Unable to resolve symbol "+ symbol;
            errorMsg += " from library " + _soPath;
            errorMsg = getLastError( errorMsg );
            HwUtil::setLastErrorMessage( errorMsg );
            status = HwConstant::ERROR_LIBRARY_RESOLVE;
        }
    } 
    
    return status;
}

/**
 * Checks for valid handle. Returns 0 if the handle is valid
 */
int HwLibrary::isValidHandle() const {
    int status = 0;

    if( NULL == _handle ) {
        HwUtil::setLastErrorMessage( "Invalid handle." );
        status = HwConstant::ERROR_INVALID_INPUT;
    }

    return status;
}


/**
 * Returns the last error set by OS functions.
 *
 * @param prepend           Prepend with this message
 */
std::string HwLibrary::getLastError( const std::string & prepend ) const {

    std::string strError = prepend;
    strError += ". ";
    std::string strTemp;

    
    #ifdef _WIN32
        
        DWORD error = GetLastError();
        LPTSTR lpBuffer;

        FormatMessage(
                FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                NULL,
                error,
                LANG_USER_DEFAULT,
                (LPSTR) &lpBuffer,
                0,
                NULL );

        if( NULL != lpBuffer ) {
            strTemp = lpBuffer;
            LocalFree(lpBuffer);
        }

    #else
        char *errstr = dlerror();
        
        if( NULL != errstr ) {
            strTemp = errstr;
        }

    #endif

    if( strTemp.length() <= 0 ) {
        strError += "No information available";
    } else {
        strError += strTemp;
    }

    return strError;
}

