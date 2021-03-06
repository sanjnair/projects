desbase.h                                                                                           0100600 0270275 0012256 00000005134 10415620151 0014146 0                                                                                                    ustar 00sanjaysn                        usc-maj                         0000566 0005431                                                                                                                                                                        /**
 * DES base class
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   03/12/2006 - Created
 */
#ifndef __DESBASE_H__
#define __DESBASE_H__

#include <string>
#include <openssl/sha.h>
#include <openssl/des.h>

class DesBase {

public:

    /**
     * Virtual D'tor.
     */
    virtual ~DesBase();

protected:
    
    /**
     * C'tor.
     */
    DesBase();

    /**
     * Returns DES prompt string
     *
     * @param encrypt               Encrption of decryption
     */
    std::string getDesPrompt( bool encrypt ) const;

    /**
     * Returns the sha1 value for the string
     *
     * @param str                   String for the sha1 hash to be computed
     * @param sha1Buf               Sha-1 output
     */
    int getSha1FromStr( 
                    const std::string & str, 
                    unsigned char sha1Buf[SHA_DIGEST_LENGTH] ) const;

    /**
     * Gets the password from the user and returns the SHA-1 Hash
     * of the password.
     *
     * @param encrypt               If true, password will be verified.
     * @param password              Password
     * @param sha1_ptr              Returns the Sha1 value of the password
     *
     * @return                      0 if OK, else non-zero value
     */
    int getPassword( 
            bool encrypt,
            std::string & password,
            unsigned char sha1Buf[SHA_DIGEST_LENGTH] ) const;

    /**
     * Intitializes the DES keys
     *
     * @param sha1Buf               Sha1 value
     * @param soPath                So Path
     */
    int initDesKeys( 
            unsigned char sha1Buf[SHA_DIGEST_LENGTH],
            const std::string & soPath );


    /**
     * Returns DES key schedule
     * 
     * @param key                   des key
     * @param ks                    
     */
    int getDesKeySchedule( 
            const unsigned char key[8],
            DES_key_schedule *ks ) const;

    /**
     * Returns the file size. If error occurs, error is recorded.
     *
     * @param fileName              Name of the file
     * @param fileSize              Size of the file
     */
    int getFileSize( 
             const std::string & fileName,
             long & fileSize ) const;

    /**
     * Validates the init
     */
    int validateInit() const;
    
    //-------------------------------------------------------------------------
    long _fileSize;
    bool _initialized;
    
    std::string _fileFrom;
    std::string _fileTo;
    FILE * _fpFrom;
    FILE * _fpTo;
    
    unsigned char _iv[8];
    DES_key_schedule _ks1;
    DES_key_schedule _ks2;
    DES_key_schedule _ks3;
};

#endif //__DESBASE_H__

                                                                                                                                                                                                                                                                                                                                                                                                                                    desdecdump.h                                                                                        0100600 0270275 0012256 00000001237 10415620151 0014655 0                                                                                                    ustar 00sanjaysn                        usc-maj                         0000566 0005431                                                                                                                                                                        /**
 * Class for DES decoding
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   01/21/2005 - Created
 */
#ifndef __DESDECDUMP_H__
#define __DESDECDUMP_H__

#include "basedump.h"

class DesDecDump : public BaseDump {

public:
    
    /**
     * C'tor.
     * 
     * @param filePath          File path
     * @param soPath            Shared libary Path
     */
    DesDecDump(
            const std::string filePath,
            const std::string soPath );
        
    /**
     * Virtual D'tor.
     */
    virtual ~DesDecDump();

    /**
     * Dumps the output (based on input file or stdin)
     */
    virtual int dump();
};

#endif //__DESDECDUMP_H__

                                                                                                                                                                                                                                                                                                                                                                 desdecryptor.h                                                                                      0100600 0270275 0012256 00000005060 10415620151 0015245 0                                                                                                    ustar 00sanjaysn                        usc-maj                         0000566 0005431                                                                                                                                                                        /**
 * Class for Decrypting the DES data
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   03/12/2006 - Created
 */
#ifndef __DESDECRYPTOR_H__
#define __DESDECRYPTOR_H__

#include "desbase.h"

class DesDecryptor : public DesBase {

public:
    
    /**
     * C'tor.
     */
    DesDecryptor();

    /**
     * Virtual D'tor.
     */
    virtual ~DesDecryptor();

    /**
     * Intializes the des
     *
     * @param soPath            SoPath
     * @param filePath          File Path
     */
    int init( 
        const std::string & soPath,
        const std::string & filePath );

    /**
     * decrypts the data and write to fp
     *
     * @param fpFrom            From file
     * @param fpTo              To File
     */
    int decrypt(
            FILE * fpFrom,
            FILE * fpTo );

    /**
     * encrypts the data and write to fp
     *
     * @param fileFrom          From file
     * @param fileTo            To File
     */
    int decrypt(
            const std::string & fileFrom,
            const std::string & fileTo );

    /**
     * Returns the password
     */
    std::string getPwd() const;

private:

    /**
     * encrypts the data 
     */
    int decrypt();

    /**
     * Checks the input file to make sure that it is a valid
     * DES file.
     *
     * @param fileSize              Size of the input file
     */
    int checkInputFile( long fileSize ) const;

    /**
     * Processes the DES header from the DES file
     * 
     * @param fp                        File pointer
     * @param numByteLastBlock          Number of bytes padded towads the end of the file
     */
    int processDesHeader( 
            FILE * fp,
            unsigned char &numByteLastBlock ) const;

    /**
     * Verifies the DES header
     * 
     * @param buffer_ptr            Buffer
     * @param numByteLastBlock      Number of bytes in the last 8-byte block of the original file
     */
    int verifyDesHeader( 
            const unsigned char *buffer_ptr,
            unsigned char &numByteLastBlock ) const;

    /**
     * Compares the Sha-1 value of original file and Sha-1 value of the
     * decryped file.
     * 
     * @param origSha1              Orignal Sha1 that is stored in the cipher file
     * @param newSha1               Sha1 that is computed for the deciphered text
     *
     * @return                      0 if comparison is successful, else non-zero
     */
    int compareSha1( 
            const unsigned char * origSha1,
            const unsigned char * newSha1 ) const;

    std::string _pwd;
};

#endif //__DESDECRYPTOR_H__

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                desencdump.h                                                                                        0100600 0270275 0012256 00000001227 10415620151 0014666 0                                                                                                    ustar 00sanjaysn                        usc-maj                         0000566 0005431                                                                                                                                                                        /**
 * Class for DES encoding
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   01/21/2005 - Created
 */
#ifndef __DESENCDUMP_H__
#define __DESENCDUMP_H__

#include "basedump.h"

class DesEncDump : public BaseDump {

public:
    
    /**
     * C'tor.
     * 
     * @param filePath          File path
     * @param soPath            Shared libary Path
     */
    DesEncDump(
            const std::string filePath,
            const std::string soPath );

    /**
     * Virtual D'tor.
     */
    virtual ~DesEncDump();

    /**
     * Dumps the output (based on input file or stdin)
     */
    virtual int dump();
};

#endif //__DESENCDUMP_H__

                                                                                                                                                                                                                                                                                                                                                                         desencryptor.h                                                                                      0100600 0270275 0012256 00000003561 10415620151 0015263 0                                                                                                    ustar 00sanjaysn                        usc-maj                         0000566 0005431                                                                                                                                                                        /**
 * Class for Encrypting the DES data
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   03/12/2006 - Created
 */
#ifndef __DESENCRYPTOR_H__
#define __DESENCRYPTOR_H__

#include "desbase.h"

class DesEncryptor : public DesBase {

public:
    
    /**
     * C'tor.
     */
    DesEncryptor();

    /**
     * Virtual D'tor.
     */
    virtual ~DesEncryptor();

    /**
     * Intializes the des
     *
     * @param soPath            SoPath
     * @param filePath          File Path
     */
    int init( 
        const std::string & soPath,
        const std::string & filePath );

    /**
     * encrypts the data and write to fp
     *
     * @param fpFrom            From file
     * @param fpTo              To File
     */
    int encrypt(
            FILE * fpFrom,
            FILE * fpTo );

    /**
     * encrypts the data and write to fp
     *
     * @param fileFrom            From file
     * @param fileTo              To File
     */
    int encrypt(
            const std::string & fileFrom,
            const std::string & fileTo );

    /**
     * Sets the password
     *
     * @param password
     */
    void setPwd( const std::string & password );

protected:

    /**
     * Intitializes the DES keys
     *
     * @param sha1Buf               Sha1 value
     * @param soPath                So Path
     */
    int initDesKeys( 
            unsigned char sha1Buf[SHA_DIGEST_LENGTH],
            const std::string & soPath );

private:

    /**
     * encrypts the data 
     */
    int encrypt();

    /**
     * Gets the header
     *
     * @param fp                File pointer to write to
     */
    int writeHeader( FILE * fp ) const;

    /**
     * Checks to see if the key is valid
     * 
     * @param ks                DES key schedule
     */
    int validateKey( DES_key_schedule *ks ) const;

    std::string _pwd;
};

#endif //__DESENCRYPTOR_H__

                                                                                                                                               editordump.h                                                                                        0100600 0270275 0012256 00000005256 10415620151 0014721 0                                                                                                    ustar 00sanjaysn                        usc-maj                         0000566 0005431                                                                                                                                                                        /**
 * Class for Editor Dump
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   03/28/2006 - Created
 */
#ifndef __EDITORDUMP_H__
#define __EDITORDUMP_H__

#include "basedump.h"
#include "sha1data.h"


class EditorDump : public BaseDump {

public:
    
    /**
     * C'tor.
     * 
     * @param filePath          File path
     */
    EditorDump( const std::string filePath, const std::string soPath );

    /**
     * Virtual D'tor.
     */
    virtual ~EditorDump();

    /**
     * Dumps the output (based on input file or stdin)
     */
    virtual int dump();
    
private:

    /**
     * Perform the edit operation
     */
    int performEditOperation();

    /**
     * Gets the temp file (with complete path)
     *
     * @param tempFile          Temp File
     */
    int getTempFile( std::string & tempFile ) const;

    /**
     * Creates the File.
     * 
     * @param strFile           File
     */
    int createFile( const std::string & strFile ) const;

    /**
     * Deletes the file.
     *
     * @param strFile           File
     */
    int deleteFile( const std::string & strFile ) const;

    /**
     * Creates the directory specified by the path
     *
     * @param path              Directory path
     */
    int createDir( const std::string & path ) const;
    
    /**
     * Returns true if the file exists, else false.
     *
     * @param strFile           File
     * @param isDir             True if it is dir
     * @param bExist            return value
     */
    int isFileExist( 
            const std::string & strFile,
            bool isDir,
            bool & bExist ) const;

    /**
     * Starts the editor with the given file name
     */
    int editFile( const std::string & tmpFile);

    /**
     * Decrypts the input file and puts in the tmp File
     *
     * @param tmpFile           Temp file
     * @param password          Password
     */
    int decryptFile( 
            const std::string & tmpFile,
            std::string & password );

    /**
     * Encrypts the input file and puts in the tmp File
     *
     * @param tmpFile           Temp file
     * @param password          Password
     */
    int encryptFile( 
            const std::string & tmpFile,
            const std::string & password );

    /**
     * Starts the editor
     *
     * @param tmpFile           Tempfile
     */
    int startEditor( const std::string & tmpFile ) const;

    /**
     * Gets the sha1 value for the file
     *
     * @param file              File
     * @param sha1Buf           Sha1 buffer
     */
    int getSha1( 
            const std::string & file,
            unsigned char sha1Buf[SHA_DIGEST_LENGTH] ) const;
    
};

#endif //__EDITORDUMP_H__

                                                                                                                                                                                                                                                                                                                                                  hwconstant.h                                                                                        0100600 0270275 0012256 00000003745 10415620151 0014736 0                                                                                                    ustar 00sanjaysn                        usc-maj                         0000566 0005431                                                                                                                                                                        /**
 * Constant file for HW1
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   03/12/2006 - Created
 */

#ifndef __HWCONSTANT_H__
#define __HWCONSTANT_H__

#include <string>

class HwConstant {

public:
    static const int ERROR_FILE_OPEN;
    static const int ERROR_FILE_CLOSE;
    static const int ERROR_FILE_READ;
    static const int ERROR_OUT_OF_MEMORY;
    static const int ERROR_INVALID_INPUT;
    static const int ERROR_INVALID_STATE;
    static const int ERROR_DES_WEAK_KEY;
    static const int ERROR_LIBRARY_LOAD;
    static const int ERROR_LIBRARY_UNLOAD;
    static const int ERROR_LIBRARY_RESOLVE;

    static const std::string OPTION_SECRET;
    static const std::string OPTION_SIGN;
    static const std::string OPTION_VERIFY;
    static const std::string OPTION_ENC;
    static const std::string OPTION_DEC;
    static const std::string OPTION_EDIT;
    static const std::string PARAM_SO;

    static const std::string ENV_EDITOR;
    static const std::string ENV_HOME;
    static const std::string TMP_DIR;
    static const std::string DIR_SEPARATOR;

    static const std::string DES_HEADER;

    static const std::string SYMBOL_SC_INIT;
    static const std::string SYMBOL_SC_CLEANUP;
    static const std::string SYMBOL_SC_GETSIGNATURESIZE;
    static const std::string SYMBOL_SC_SIGN;
    static const std::string SYMBOL_SC_3DESKEYGEN;
    static const std::string SYMBOL_SC_PUBLICSIZEQUERY;
    static const std::string SYMBOL_SC_PUBLICQUERY;
   
    static const int BUFFER_SIZE;
    static const int MAX_PASSWD_LENGTH;
    static const int DES_HEADER_LENGTH;

    static const std::string RSA_PRIVATE_KEY;
    static const std::string RSA_PUBLIC_EXPONENT;
    static const std::string RSA_PUBLIC_MODULUS;
    static const std::string RSA_RANDOM_BITS_1;
    static const std::string RSA_RANDOM_BITS_2;

    static const int RSA_PUBLIC_MODULUS_SIZE;

private:
    /**
     * Constructor is private
     */
    HwConstant();

    static int _errorStart;
};

#endif //__HWCONSTANT_H__

                           hwlibrary.h                                                                                         0100600 0270275 0012256 00000003025 10415620151 0014540 0                                                                                                    ustar 00sanjaysn                        usc-maj                         0000566 0005431                                                                                                                                                                        /**
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

e library is successfully loaded.
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
     * Returns 0 ifhwutil.h                                                                                            0100600 0270275 0012256 00000013665 10415620151 0014064 0                                                                                                    ustar 00sanjaysn                        usc-maj                         0000566 0005431                                                                                                                                                                        /**
 * Constant file for HW1
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   03/12/2006 - Created
 */

#ifndef __HWUTIL_H__
#define __HWUTIL_H__

#include <string>

class HwUtil {

public:

    /**
     * Converts number to string
	 *
	 * @param number                number
     */
    static std::string getString(int number);

    /**
     * Returns Hex string for the given character
	 *
	 * @param c                     Character 
     */
    static std::string getHexString(unsigned char c);

    /**
     * Returns Hex string for the given character
	 *
	 * @param c                     Character 
     */
    static std::string getHexString(unsigned int c);

    /** 
     * Returns the hex string for the given bytearray
     *
     * @param buffer                Buffer to write
     * @param bufferLen             Length of the buffer
     */
    static std::string getHexString( 
                    const unsigned char * buffer,
                    int bufferLen );

    /**
     * Opens the file. 
     * 
     * @param filePath              File Path
     * @param mode                  Mode
     * @param fp                    File pointer
     */
    static int openFile( 
                    const std::string & filePath,
                    const std::string & mode,
                    FILE *& fp );

    /**
     * Closes the file
     *
     * @param fp                    File pointer
     */
    static int closeFile( FILE * fp );

    /**
     * Writes the give string to the file pointed b file pointer.
     *
     * @param data                  Data
     * @param size                  Size
     * @param dataSize              Size of data
     * @param fp                    File pointer
     */
    static int HwUtil::writeToFile( 
                    const void * data,
                    unsigned int size,
                    unsigned int dataSize,
                    FILE * fp );
    
    /**
     * Writes the give string to the file pointed b file pointer.
	 *
	 * @param fp                    Valid file pointer
	 * @param str                   String to write
     */
    static int writeToFile( 
                    FILE * fp, 
                    const std::string & str);

    /**
     * Writes to file
	 *
     * @param fp                    File pointer
	 * @param buffer                Charracter buffer
     * @param bufferLen             Length of the buffer
     */
    static int writeToFile( 
                    FILE * fp,
                    const unsigned char * buffer, 
                    unsigned int bufferLen );

    /**
     * Writes to stdout
	 *
	 * @param str                   String to write
     */
    static int writeOutput( const std::string & str);

    /**
     * Writes to stdout and inserts a new line after the string.
	 *
	 * @param str                   String to write
     */
    static int writeOutputLn( const std::string & str);


    /**
     * Writes to stdout
	 *
	 * @param buffer                Charracter buffer
     * @param bufferLen             Length of the buffer
     */
    static int writeOutput( 
                    const unsigned char * buffer, 
                    unsigned int bufferLen );


    /**
     * Writes error to stderr. Program will append a newline after 
     * error being written.
	 *
	 * @param fp                    Valid file pointer
	 * @param str                   String to write
     * @param newLine               Append newline
     */
    static int writeError( 
                    const std::string & str, 
                    bool newLine = true );

    /**
     * Sets the last error message
     */
    static void setLastErrorMessage(const std::string & str );

    /**
     * Returns the last error message
     */
    static std::string getLastErrorMessage();

    /**
     * Returns the file size of the file
     *
     * @param fileName              Name of the file
     * @param size_out              File Size
     *
     * @return                      0, if successful.
     */
    static int getFileSize( 
                    const std::string & fileName, 
                    long & size_out );

    /**
     * Creates the buffer and returns the pointer to buffer.
     * If the buffer could not be created, this method returns
     * error code and buffer pointer will points to NULL.
     *
     * NOTE: Caller need to de-allocate the memory after use.
     *
     * @param size                  Size of the buffer
     * @param buffer_ptr            Reference to buffer pointer
     */
    static int createBuffer( int size, unsigned char *& buffer_ptr );

    /**
     * Deletes the buffer
     */
    static void deleteBuffer( unsigned char *& buffer_ptr );

    /**
     * Creates the buffer and returns the pointer to buffer.
     * If the buffer could not be created, this method returns
     * error code and buffer pointer will points to NULL.
     *
     * NOTE: Caller need to de-allocate the memory after use.
     *
     * @param size                  Size of the buffer
     * @param buffer_ptr            Reference to buffer pointer
     */
    static int createCharBuffer( int size, char *& buffer_ptr );

    /**
     * Deletes the buffer
     */
    static void deleteCharBuffer( char *& buffer_ptr );

    /**
     * Checks the read status of the file. If the read status is not
     * correct, this method will update the error message and returns 
     * error code.
     */
    static int checkFileReadStatus( FILE * fp );

    /**
     * Gets the value of the environment variable.
     * If value is null, then this method will return a non-zero return value.
     *
     * @param strName               Name of the environment variable
     * @param strValue              Valud of the environment variable
     */
    static int getEnvValue( 
                            const std::string & strName,
                            std::string & strValue );

private:
    /**
     * Constructor is private
     */
    HwUtil();

    static std::string _errorMsg;
};

#endif //__HWUTIL_H__

                                                                           scintrf.h                                                                                           0100600 0270275 0012256 00000010243 10415620151 0014205 0                                                                                                    ustar 00sanjaysn                        usc-maj                         0000566 0005431                                                                                                                                                                        #ifndef __SMARTCARD_H__
#define __SMARTCARD_H__


//typedef definitions for export
//-----------------------------------------------------------------------------

#ifdef _WIN32
    #define EXPORT_FUNC  __declspec( dllexport )
#else
    #define EXPORT_FUNC  
#endif


/**
 * State of smart card
 */
typedef void *SmartcardState;

/**
 * Initializes the smart card state.
 *
 * The SC_init() function is to be called immediately after 
 * the shared library is loaded. You can create state information 
 * and store it in the SmartcardState. This state information is 
 * to be freed in SC_cleanup().  You must call SC_cleanup() 
 * when you are finished with the shared library.
 */
typedef SmartcardState (SC_Init_Func)(void);

/**
 * Cleans up the smart card state.
 *
 * routine frees up all resources used by the smartcard and 
 * any resource associated with SmartcardState. The application 
 * must not call any of the smartcard functions above 
 * (except for SC_init()) after this function is called. 
 */
typedef void (SC_Cleanup_Func)(SmartcardState);

/**
 * Function returns the size of the digital signature. 
 * This is to be used in conjunction with the SC_sign() routine.
 */
typedef int (SC_GetSignatureSize_Func)(SmartcardState);

/**
 * Routine writes the digital signature of sha1_buf in buf_return. 
 * Caller must allocate a large enough buf_return 
 * (determined by SC_get_signature_size()) before this routine is called. 
 * This function returns 0 if successful, otherwise, it should return -1.
 */
typedef int (SC_Sign_Func) (
                    SmartcardState,
                    unsigned char sha1_buf[SHA_DIGEST_LENGTH], 
                    unsigned char * );

/**
 * Routine returns the size of the RSA public exponent in 
 * (*p_public_exponent_size_return) and the size of the 
 * RSA public modulus in (*p_public_modulus_size_return). 
 * These values are to be used in conjunction with the 
 * SC_public_query() routine. This function returns 0 if successful, 
 * otherwise, it should return -1.
 */ 
typedef int (SC_PublicSizeQuery_Func)(
                SmartcardState,
                int *p_public_exponent_size_return, 
                int *p_public_modulus_size_return);


/**
 * Routine returns the RSA public exponent in public_exponent_return 
 * and the RSA public modulus in public_modulus_return. Caller must 
 * allocate a large enough space for the buffers 
 * (determined by calling SC_public_size_query()) before this routine 
 * is called. This function returns 0 if successful, 
 * otherwise, it should return -1. 
 */
typedef int (SC_PublicQuery_Func)(SmartcardState,
        unsigned char *public_exponent_return,
        unsigned char *public_modulus_return);

/**
 * Routine performs secret key generation. 
 * This function returns 0 if successful, otherwise, it should return -1. 
 */
typedef int (SC_3DesKeyGen_Func)(
                SmartcardState,
                unsigned char sha1_buf[SHA_DIGEST_LENGTH], 
                unsigned char iv[8],
                unsigned char key1[8], 
                unsigned char key2[8], 
                unsigned char key3[8] );

//-----------------------------------------------------------------------------

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

    extern EXPORT_FUNC SmartcardState SC_init(void);

    extern EXPORT_FUNC void SC_cleanup( SmartcardState pSS );

    extern EXPORT_FUNC int SC_get_signature_size( SmartcardState pSS );

    extern EXPORT_FUNC int SC_sign(
		    SmartcardState pSS,
		    unsigned char sha1_buf[SHA_DIGEST_LENGTH],
		    unsigned char *buf_return);

    extern EXPORT_FUNC int SC_public_size_query(
		    SmartcardState pSS,
		    int *p_public_exponent_size_return,
		    int *p_public_modulus_size_return);

    extern EXPORT_FUNC int SC_public_query(
		    SmartcardState pSS,
		    unsigned char *public_exponent_return,
		    unsigned char *public_modulus_return);

    extern EXPORT_FUNC int SC_3des_key_gen(
		    SmartcardState pSS,
		    unsigned char sha1_buf[SHA_DIGEST_LENGTH],
		    unsigned char iv[8],
		    unsigned char key1[8],
		    unsigned char key2[8],
		    unsigned char key3[8]);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif //__SMARTCARD_H__


                                                                                                                                                                                                                                                                                                                                                             secretdump.h                                                                                        0100600 0270275 0012256 00000004711 10415620151 0014713 0                                                                                                    ustar 00sanjaysn                        usc-maj                         0000566 0005431                                                                                                                                                                        /**
 * Class for Secret encoding
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   03/12/2006 - Created
 */
#ifndef __SHA1DUMP_H__
#define __SHA1DUMP_H__

#include "basedump.h"
#include <openssl/rsa.h>


class SecretDump : public BaseDump {

public:
    
    /**
     * C'tor.
     * 
     * @param filePath          File path
     */
    SecretDump( const std::string filePath, const std::string soPath );

    /**
     * Virtual D'tor.
     */
    virtual ~SecretDump();

    /**
     * Dumps the output (based on input file or stdin)
     */
    virtual int dump();
    
private:

    /**
     * Generate the code for the RSA
     *
     * @param rsa_ptr           RSA pointer
     * @param strCode           code returned
     */
    int getRsaCode( 
                const RSA *rsa_ptr,
                std::string & strCode ) const;

    /**
     * Get the aray code for byte array
     *
     * @param buffer            Byte buffer
     * @param bufferLen         Length of the buffer
     * @param name              Name of the array
     * @param genSizeConst      To generate size constant
     * @param strCode           code returned
     *
     */
    int getArrayCode(
            const unsigned char * buffer,
            int bufferLen,
            const std::string & name,
            bool genSizeConst,
            std::string & strCode ) const; 

    /**
     * Gets the Array code for BIGNUM.
     *
     * @param bigNum_ptr        BIGNUM pointer
     * @param name              Name of the array
     * @param lastNumBytes      Number of bytes to be considered in the last number
     * @param genSizeConst      To generate size constant
     * @param strCode           code returned
     */
    int getArrayCode( 
            const BIGNUM * bigNum_ptr,
            const std::string & name,
            bool genSizeConst,
            int lastNumBytes,
            std::string & strCode ) const; 

    /**
     * Gets the array code for the random numbers
     * 
     * @param name              Name of the array
     * @param strCode           code returned
     */
    int getArrayCodeForRandom( 
            const std::string & name,
            std::string & strCode ) const; 
    
   /**
     * generates the random bytes
     *
     * @param buffer            Buffer to hold data
     * @param bufferLen         Lengh of the buffer
     */
    int genRandomBytes(
            unsigned char * buffer,
            int bufferLen ) const;

};

#endif //__SHA1DUMP_H__

                                                       sha1data.h                                                                                          0100600 0270275 0012256 00000002413 10415620152 0014224 0                                                                                                    ustar 00sanjaysn                        usc-maj                         0000566 0005431                                                                                                                                                                        /**
 * Class for Sha-1 storing Sha-1 data
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   03/12/2006 - Created
 */
#ifndef __SHA1DATA_H__
#define __SHA1DATA_H__

#include <stdio.h>
#include <string>
#include <openssl/sha.h>

class Sha1Data {

public:
    
    /**
     * C'tor.
     */
    Sha1Data();

    /**
     * Virtual D'tor.
     */
    virtual ~Sha1Data();

    /**
     * Calculates SHA-1 on the file pointed by fpSha
     *
     * @param fp                    File pointer
     */
    int calculate( FILE * fp );

    /**
     * updates the sha-1 with the data
     *
     * @param buffer_ptr            Buffer
     * @param bufferLen             Buffer Len
     */
    int update( 
            const unsigned char * buffer_ptr,
            unsigned long bufferLen );

    /**
     * updates the sha-1 with the data
     *
     * @param buffer_ptr            Buffer
     * @param bufferLen             Buffer Len
     */
    int final();

    /**
     * Returns the sha-1 Buffer        
     */
    unsigned char * getBuffer() const;

    /**
     * Returns the Hex representation of the sha-1
     */
    std::string getHexString() const;
    
private:

    unsigned char * _sha1Buffer_ptr;
    SHA_CTX _shaCtx;
    bool _initialized;
};

#endif //__SHA1DATA_H__

                                                                                                                                                                                                                                                     signdump.h                                                                                          0100600 0270275 0012256 00000001400 10415620152 0014357 0                                                                                                    ustar 00sanjaysn                        usc-maj                         0000566 0005431                                                                                                                                                                        /**
 * Class for Sign Dump
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   03/12/2006 - Created
 */
#ifndef __SIGNDUMP_H__
#define __SIGNDUMP_H__

#include "basedump.h"


class SignDump : public BaseDump {

public:
    
    /**
     * C'tor.
     * 
     * @param filePath          File path
     */
    SignDump( const std::string filePath, const std::string soPath );

    /**
     * Virtual D'tor.
     */
    virtual ~SignDump();

    /**
     * Dumps the output (based on input file or stdin)
     */
    virtual int dump();
    
private:
    
    /**
     * Gets the signed SHA-1 signature for the file.
     *
     * @param strSign           Signed string
     */
    int getSignedStr( std::string & strSign ) const;
};

#endif //__SIGNDUMP_H__

                                                                                                                                                                                                                                                                smartcardproxy.h                                                                                    0100600 0270275 0012256 00000004712 10415620152 0015624 0                                                                                                    ustar 00sanjaysn                        usc-maj                         0000566 0005431                                                                                                                                                                        /**
 * Class for Smart Card Proxy,
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   03/12/2006 - Created
 */
#ifndef __SMARTCARD_PROXY_H__
#define __SMARTCARD_PROXY_H__

#include "hwlibrary.h"
#include <openssl/sha.h>

extern "C" {
#include "scintrf.h"
}

class SmartCardProxy {

public:
    
    /**
     * C'tor.
     */
    SmartCardProxy();

    /**
     * Virtual D'tor.
     */
    virtual ~SmartCardProxy();

    /**
     * Initializes the Smart card
     *
     * @param soPath        Shared library path
     */
    int init( const std::string & soPath );

    /**
     * Cleans up the Smart card
     */
    int cleanup();

    /**
     * Returns the size of the digital signature.
     * @param size              Signature Size
     */
    int getSignatureSize(  int & size ) const;

    /**
     * Signs the Sha1 signature
     *
     * @param sha1_buf              SHA-1 buffer
     * @param buf_return            Return signature
     */
    int sign(
            unsigned char sha1_buf[SHA_DIGEST_LENGTH],
		    unsigned char * buf_return ) const;

    /**
     * Returns the public key size query 
     *
     * @param p_public_exponent_size    Public exponent Size
     * @param p_public_modulus_size     Public modulus Size
     */
    int publicSizeQuery(
            int * p_public_exponent_size,
            int * p_public_modulus_size ) const;
    

    /**
     * Returns the public key query 
     *
     * @param p_public_exponent_size    Public exponent
     * @param p_public_modulus_size     Public modulus
     */
    int publicQuery(
		    unsigned char *public_exponent,
		    unsigned char *public_modulus);

    /**
     * Routine performs secret key generation. 
     * This function returns 0 if successful, otherwise, it should return -1. 
     */
    int desKeyGen (
            unsigned char sha1_buf[SHA_DIGEST_LENGTH], 
            unsigned char iv[8],
            unsigned char key1[8], 
            unsigned char key2[8], 
            unsigned char key3[8] );

    /**
     * Gets the open ssl error
     */
    static std::string getOpenSslError();

private:
    
    /**
     * Shuts down the smart card proxy
     */
    int shutdown();
    
    /**
     * Returns 0 if the proxy is initialized.
     */
    int isInitialized() const;
    /**
     * Sets the library error.
     */
    void setLibraryError( const std::string & methodName ) const;

    SmartcardState _smartCardState;
    HwLibrary * _library;
};

#endif //__SMARTCARD_PROXY_H__

                                                      verifydump.h                                                                                        0100600 0270275 0012256 00000006302 10415620152 0014731 0                                                                                                    ustar 00sanjaysn                        usc-maj                         0000566 0005431                                                                                                                                                                        /**
 * Class for Verify Dump
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   03/18/2006 - Created
 */
#ifndef __VERIFYDUMP_H__
#define __VERIFYDUMP_H__

#include "basedump.h"
#include <openssl/rsa.h>


class VerifyDump : public BaseDump {

public:
    
    /**
     * C'tor.
     * 
     * @param filePath          File path
     */
    VerifyDump( const std::string filePath, const std::string soPath );

    /**
     * Virtual D'tor.
     */
    virtual ~VerifyDump();

    /**
     * Dumps the output (based on input file or stdin)
     */
    virtual int dump();
    
private:

    /**
     * Gets the encrypted data.
     *
     * @param dataLen               [in] Length of data
     * @param dataBuffer            [out] Buffer containing data.
     *                                    Buffer should be allocated to dataLen /2
     *                              
     */
    int getEncyptedData( 
                int dataLen,
                unsigned char * dataBuffer ) const;

    
    /**
     * Returns the octet array for the given hex string.
     * 
     * @param hexString             [in] Hex String
     * @param dataLen               [in] Expected data length
     * @param dataBuffer            [out] converted buffer
     */
    int getOctetArray( 
            const std::string & hexString,
            int dataLen,
            unsigned char * dataBuffer ) const;

    /**
     * Gets the signed SHA-1 signature for the file.
     *
     * @param data_ptr              [in] Data
     * @param modulus_ptr           [in] Modulus 
     * @param modulusSize           [in] Modulus size
     * @param exponent_ptr          [in] Exponent 
     * @param exponentSize          [in] Exponent size
     * @param strVerify             [out]Verifyed string
     */
    int getVerifyedStr( 
            unsigned char *data_ptr,
            const unsigned char * modulus_ptr,
            int modulusSize,
            const unsigned char * exponent_ptr,
            int exponentSize,
            std::string & strVerify ) const;

    /**
     * Gets the public key info.
     *
     * @param modulus_ptr           [out] Modulus 
     * @param modulusSize           [out] Modulus size
     * @param exponent_ptr          [out] Exponent 
     * @param exponentSize          [out] Exponent size
     */
    int getPublicKeyInfo(
            unsigned char *& modulus_ptr,
            int & modulusSize,
            unsigned char *& exponent_ptr,
            int & exponentSize ) const;

    /**
     * Creates a new RSA and initializes it with the keys.
     * CALLER MUST CALL RSA_free(RSA *rsa) to free the memory created by 
     * this call.
     *
     * If RSA cannot be created, this method will return -1. Else 0.
     *
     * @param modulus_ptr           [in] Modulus 
     * @param modulusSize           [in] Modulus size
     * @param exponent_ptr          [in] Exponent 
     * @param exponentSize          [in] Exponent size
     * @param rsa                   [out] RSA to be created
     */ 
    int getNewRsa( 
            const unsigned char * modulus_ptr,
            int modulusSize,
            const unsigned char * exponent_ptr,
            int exponentSize,
            RSA *& rsa ) const; 

};

#endif //__VERIFYDUMP_H__

                                                                                                                                                                                                                                                                                                                              basedump.cc                                                                                         0100600 0270275 0012256 00000005473 10415620151 0014504 0                                                                                                    ustar 00sanjaysn                        usc-maj                         0000566 0005431                                                                                                                                                                        #include "basedump.h"
#include "hwconstant.h"
#include "hwutil.h"

#include <openssl/err.h>

/**
 * C'tor.
 *
 * @param filePath          File path
 */
BaseDump::BaseDump( const std::string filePath, const std::string soPath) {
    _filePath = filePath;      
    _soPath = soPath;
    _fp = NULL;
}

/**
 * Virtual D'tor.
 */
BaseDump::~BaseDump() { }

/**
 * Opens the input file. If the file path is valid and open is successful,
 * this methid creates a valid file pointer and keeps it. In this case, the
 * return code will be 0. Otherwise, return code will be non-zero.
 *
 * @param mustExist             Parameter to check if the file exists.
 *                              If this parameter is false and the filepath
 *                              is empty, stdin will be the default file pointer.
 */
int BaseDump::openInputFile( bool mustExist ) {

    int status = 0;

    if( _filePath.length() > 0 ) {
        _fp = fopen(_filePath.c_str(), "rb");
    } else {
        if( ! mustExist ) {
            _fp = stdin;    
        }
    }

    if( NULL == _fp ) {
        std::string msg =  "Unable to open input file \"";
        msg += _filePath;
        msg += "\" for reading.";
        HwUtil::setLastErrorMessage(msg);

        status = HwConstant::ERROR_FILE_OPEN;; 
    }
       
    return status;
}

/**
 * Opens the input file. If the file path is valid and open is successful,
 * this methid creates a valid file pointer and keeps it. In this case, the
 * return code will be 0. Otherwise, return code will be non-zero.
 * 
 */
int BaseDump::closeInputFile() {

    int status = 0;

    if( NULL != _fp ) {
        if( _filePath.length() > 0 ) {
            status = fclose(_fp);
            _fp = NULL;
        }

        if( 0 != status ) {
            std::string msg =  "Unable to close input file \"";
            msg += _filePath;
            msg += "\"";
            HwUtil::setLastErrorMessage(msg);

            status = HwConstant::ERROR_FILE_CLOSE;
        }
    }

    return status;
}

/**
 * Checks the read status of the file. If the read status is not
 * correct, this method will update the error message and returns 
 * error code.
 */
int BaseDump::checkFileReadStatus() const {
    int status = 0;

    status = HwUtil::checkFileReadStatus( _fp );

    return status;
}

/**
 * Returns the openssl error string 
 *
 * @param errorCode         Error code. 
 *
 * @return                  Error string if available,else "No information available"
 */
std::string BaseDump::getOpenSslErrStr( int errorCode ) const {

    std::string str;
 
    char * error_str = ERR_error_string( ERR_get_error(), NULL );
    if( NULL != error_str ) {
        str += "[ ";
        str += error_str;
        str += " ]";

        str = error_str;
    }

    if( str.length() <= 0 ) {
        str = "No information available"; 
    }

    return str;
}

                                                                                                                                                                                                     desbase.cc                                                                                          0100600 0270275 0012256 00000013775 10415620151 0014316 0                                                                                                    ustar 00sanjaysn                        usc-maj                         0000566 0005431                                                                                                                                                                        #include "desbase.h"
#include "hwconstant.h"
#include "hwutil.h"
#include "smartcardproxy.h"
#include "sha1data.h"

/**
 * C'tor.
 */
DesBase::DesBase() { 
    _fileSize = 0;
    _initialized = false;
    _fpFrom = NULL;
    _fpTo = NULL;
}

/**
 * Virtual D'tor.
 */
DesBase::~DesBase() {}

/**
 * Returns DES prompt string
 *
 * @param encryption            Encrption of decryption
 */
std::string DesBase::getDesPrompt( bool encrypt ) const {

    std::string str = "Enter passpharse for ";
    
    if( encrypt ) {
        str += "encryption";
    } else {
        str += "decryption";
    }
    
    str += " (max chars = ";
    str += HwUtil::getString(HwConstant::MAX_PASSWD_LENGTH );
    str += ")";
    str += ":";

    return str;
}

/**
 * Returns the sha1 value for the string
 *
 * @param str                   String for the sha1 hash to be computed
 * @param sha1_ptr              Sha-1 output
 */
int DesBase::getSha1FromStr( 
        const std::string & str, 
        unsigned char sha1Buf[SHA_DIGEST_LENGTH]  ) const {
    
    int status = 0;
    Sha1Data sha1data;

    if( str.length() > 0 ) {
        if( 0 == status ) {
            status = sha1data.update( (unsigned char *) str.c_str(), str.length() );
            
            if( 0 == status ) {
                status = sha1data.final();
            }
        }
        if( 0 == status ) {
            if( NULL != sha1data.getBuffer() ) {
                memcpy(sha1Buf, sha1data.getBuffer(), SHA_DIGEST_LENGTH ); 
            } else {
                status = -1;
                HwUtil::setLastErrorMessage("DesBase::getSha1FromStr: Invalid sha buffer"); 
            }
        }
        //SHA1( (unsigned char *) str.c_str(), str.length(), sha1Buf );
    } else {
        status = HwConstant::ERROR_INVALID_INPUT;
        HwUtil::setLastErrorMessage("Zero length password is not allowed");
    }

    return status;
}

/**
 * Gets the password from the user and returns the SHA-1 Hash
 * of the password.
 *
 * @param encrypt               If true, password will be verified.
 * @param password              Password
 * @param sha1Buf               Returns the Sha1 value of the password
 *
 * @return                      0 if OK, else non-zero value
 */
int DesBase::getPassword( 
                    bool encrypt,
                    std::string & password,
                    unsigned char sha1Buf[SHA_DIGEST_LENGTH] ) const {

    int status  = 0;
    int verify  = 0;
    char * buf  = NULL;
    char * buff = NULL;
    std::string msg; 


    std::string strPrompt;
    
    strPrompt = getDesPrompt( encrypt );
    verify = (encrypt) ? 1 : 0;
    
    status = HwUtil::createCharBuffer( HwConstant::MAX_PASSWD_LENGTH, buf );
    
    if( 0 == status ) {
        status = HwUtil::createCharBuffer( HwConstant::MAX_PASSWD_LENGTH, buff );
    }
    
    if( 0 == status ) {
        status = des_read_pw( 
                        buf, 
                        buff,
                        HwConstant::MAX_PASSWD_LENGTH, 
                        strPrompt.c_str(),
                        verify );
    }
   
    if( 0 != status ) {
        if( -2 == status ) { //ctrl-C
            HwUtil::setLastErrorMessage("User canceled the operation");
        } else {
            if( strlen(buff) <= 0 ) {
                msg  = "Password length should not be more than ";
                msg +=  HwUtil::getString(HwConstant::MAX_PASSWD_LENGTH );
                msg += " characters";
                HwUtil::setLastErrorMessage(msg);
            } else if( encrypt ) {
                msg = "Passwords does not match";
                HwUtil::setLastErrorMessage(msg);
            }
        }
    } else {
        password = buf;
        status = getSha1FromStr(password, sha1Buf );
    }
    
    HwUtil::deleteCharBuffer( buf );
    HwUtil::deleteCharBuffer( buff );

    return status;
}

/**
 * Intitializes the DES keys
 *
 * @param sha1Buf               Sha1 value
 * @param soPath                So Path
 */
int DesBase::initDesKeys( 
                unsigned char sha1Buf[SHA_DIGEST_LENGTH],
                const std::string & soPath ) {

    int status = 0;
    
    unsigned char key1[8];
    unsigned char key2[8];
    unsigned char key3[8];
    SmartCardProxy proxy;


    status = proxy.init( soPath );
    
    if( 0 == status ) {
        status = proxy.desKeyGen( sha1Buf, _iv, key1, key2, key3 );
    }

    if( 0 == status ) {
        DES_set_odd_parity( (DES_cblock*)key1 );
        DES_set_odd_parity( (DES_cblock*)key2 );
        DES_set_odd_parity( (DES_cblock*)key3 );
    }

    if( 0 == status ) {
        status = getDesKeySchedule( key1, &_ks1 );
    }
    if( 0 == status ) {
        status = getDesKeySchedule( key2, &_ks2);
    }
    if( 0 == status ) {
        status = getDesKeySchedule( key3, &_ks3 );
    }
    
    return status;
}

/**
 * Returns DES key schedule
 * 
 * @param key                   des key
 * @param ks                    key schedule 
 */
int DesBase::getDesKeySchedule( 
        const unsigned char key[8],
        DES_key_schedule *ks ) const {

    int status = 0;

    status = DES_set_key_checked( (DES_cblock*)key, ks );

    if( 0 != status ) {
        HwUtil::setLastErrorMessage("Unable to set the key schedule");
    }
    return status;
}

/**
 * Returns the file size. If error occurs, error is recorded.
 *
 * @param fileName              Name of the file
 * @param fileSize              Size of the file
 *
 */
int DesBase::getFileSize( 
                 const std::string & fileName,
                 long & fileSize ) const {
    
    int status = 0;

    status = HwUtil::getFileSize( fileName, fileSize );

    if( 0 == status ) {
        if( fileSize <= 0 ) {
            status = HwConstant::ERROR_FILE_READ;
            HwUtil::setLastErrorMessage("Input file length should be greater than 0");
        }
    }
    
    return status;
}

/**
 * Validates the init
 */
int DesBase::validateInit() const {
    
    int status = 0;

    if( ! _initialized ) {
        HwUtil::setLastErrorMessage("DesBase::validateInit: Not initialized");
        status = -1;
    }

    return status;
}
   desdecdump.cc                                                                                       0100600 0270275 0012256 00000001363 10415620151 0015013 0                                                                                                    ustar 00sanjaysn                        usc-maj                         0000566 0005431                                                                                                                                                                        #include "desdecdump.h"
#include "desdecryptor.h"

/**
 * C'tor.
 *
 * @param filePath          File path
 */
DesDecDump::DesDecDump(
                const std::string filePath,
                const std::string soPath ) : BaseDump(filePath, soPath) {
}

/**
 * Virtual D'tor.
 */
DesDecDump::~DesDecDump() {}

/**
 * Dumps the output (based on input file or stdin)
 */
int DesDecDump::dump() {

    int status = 0;
    DesDecryptor decryptor;
    
    status = openInputFile( true );

    if( 0 == status ) {
        status = decryptor.init( _soPath, _filePath );
        
        if( 0 == status ) {
            status = decryptor.decrypt( _fp, stdout );
        }

        //close input file
        closeInputFile();
    }
    
    return status;

}

                                                                                                                                                                                                                                                                             desdecryptor.cc                                                                                     0100600 0270275 0012256 00000025601 10415620151 0015406 0                                                                                                    ustar 00sanjaysn                        usc-maj                         0000566 0005431                                                                                                                                                                        #include "desdecryptor.h"
#include "hwconstant.h"
#include "hwutil.h"
#include "sha1data.h"

/**
 * C'tor.
 */
DesDecryptor::DesDecryptor() : DesBase() {

}

/**
 * Virtual D'tor.
 */
DesDecryptor::~DesDecryptor() { }

/**
 * Intializes the des
 *
 * @param soPath            SoPath
 * @param filePath          File Path
 */
int DesDecryptor::init( 
        const std::string & soPath,
        const std::string & filePath )  {

    int status = 0;
    unsigned char sha1Buf[SHA_DIGEST_LENGTH];

    if( 0 == status ) {
        status = getFileSize( filePath, _fileSize );
    }

    if( 0 == status ) {
        status = checkInputFile( _fileSize );
    }

    if( 0 == status ) {
        status = getPassword( false, _pwd, sha1Buf );
    }

    if( 0 == status ) {
        status = initDesKeys( sha1Buf, soPath );
    }
    
    if( 0 == status ) {
        _initialized = true;
    }

    return status;
}

/**
 * Returns the password
 */
std::string DesDecryptor::getPwd() const {
    return _pwd;
}

/**
 * decrypts the data and write to fp
 *
 * @param _fpFrom            From file pointer
 * @param fpTo              To File pointer
 */
int DesDecryptor::decrypt(
        FILE * fpFrom,
        FILE * fpTo ) {

    int status = 0;

    _fpFrom = fpFrom;
    _fpTo = fpTo;

    status = decrypt();

    return status;
}

/**
 * decrypts the data and write to fp
 *
 * @param fileFrom            From file
 * @param fileTo              To File
 */
int DesDecryptor::decrypt(
        const std::string & fileFrom,
        const std::string & fileTo ) {

    int status = 0;

    _fileFrom = fileFrom;
    _fileTo = fileTo;

    status = decrypt();
    
    if( NULL != _fpFrom ) {
        HwUtil::closeFile( _fpFrom );
        _fpFrom = NULL;
        _fileFrom = "";
    }
    if( NULL != _fpTo ) {
        HwUtil::closeFile( _fpTo );
        _fpTo = NULL;
        _fileTo = "";
    }
    
    return status;
}


/**
 * decrypts the data and write to fp
 */
int DesDecryptor::decrypt() {

    int status = 0;
    
    bool headerProcessed    = false;
    long readLen            = 0;
    int bytesRead           = 0;
    int shaRead             = 0;
    int size                = 0;
    int numBytesToWrite     = 0;
    int numPaddedBytes      = 0;
    
    unsigned char *inBuf    = NULL;
    unsigned char *outBuf   = NULL;
    
    unsigned char numByteLastBlock = 0;             //Number of bytes in the last 8-byte block of the original file
    unsigned char fileSha[SHA_DIGEST_LENGTH];       //sha1 value of the original data
    Sha1Data sha1data;

    //--------------------------------------------------
    
    if( 0 == status ) {
        status = HwUtil::createBuffer(HwConstant::BUFFER_SIZE, inBuf );
    }
    if( 0 == status ) {
        status = HwUtil::createBuffer(HwConstant::BUFFER_SIZE, outBuf );
    }

    if( (0 == status) && (NULL == _fpFrom) ) {
        status = HwUtil::openFile( _fileFrom, "rb", _fpFrom );
    }

    //open the output file if not already open
    if( (0 == status) && (NULL == _fpTo ) ) {
        status = HwUtil::openFile( _fileTo, "wb", _fpTo );
    }

    if( 0 == status ) {
        while( (0 == status) && (!feof(_fpFrom)) ) {
            //process the header first
            if( ! headerProcessed ) {
                status = processDesHeader( _fpFrom, numByteLastBlock );
                headerProcessed = true;
                bytesRead += HwConstant::DES_HEADER_LENGTH;

            } else {
                memset( inBuf, 0, HwConstant::BUFFER_SIZE );
                memset( outBuf, 0, HwConstant::BUFFER_SIZE );
                
                readLen = fread( inBuf, sizeof(unsigned char), HwConstant::BUFFER_SIZE, _fpFrom );
                status = HwUtil::checkFileReadStatus( _fpFrom );

                if( (0 == status) && (readLen > 0)  ) {
                    bytesRead += readLen;

                    if( 0 == shaRead ) {
                       size = _fileSize - bytesRead;
                       
                       if( 0 == size ) { //that is the entire sha is inside the current buffer
                            size = SHA_DIGEST_LENGTH;   
                       }
                        
                       if( size <= SHA_DIGEST_LENGTH ) {
                            shaRead = size;
                            memcpy( fileSha, inBuf + (readLen - shaRead), shaRead );
                            readLen -= shaRead; //adjust the readlen based on 
                        }
                    } else {
                        size = readLen;
                        if( SHA_DIGEST_LENGTH != (shaRead + size) ) {
                            std::string msg(" Invalid file. Unable to get the sha value shaRead = ");
                            msg += HwUtil::getString( shaRead );
                            msg += " size = ";
                            msg += HwUtil::getString( size );

                            status = HwConstant::ERROR_INVALID_INPUT;
                            HwUtil::setLastErrorMessage(msg);
                        } else {
                            memcpy( fileSha + shaRead, inBuf, readLen );
                        }
                    }
                }

                if( (0 == status) && (readLen > 0)  ) {
                    DES_ede3_cbc_encrypt( inBuf, outBuf, readLen, &_ks1,&_ks2, &_ks3, (DES_cblock *)_iv, 0);

                    if( (bytesRead + SHA_DIGEST_LENGTH) >= _fileSize ) {
                        //calculate the padded byte in the last block of 8 bytes block
                        //in the original file
                        numPaddedBytes = 8 - numByteLastBlock;
                        
                        //subtract the padded bytes as they weren't part of the original file
                        numBytesToWrite = readLen - numPaddedBytes;
                    } else {
                        numBytesToWrite = readLen;
                    }
                    
                    //calculate SHA-1 for the decrypted data
                    status = sha1data.update( outBuf, numBytesToWrite );

                    if( 0 == status ) {
                        status = HwUtil::writeToFile( _fpTo, outBuf, numBytesToWrite );
                    }
                }
            }
        } //end while
            
        if( 0 == status ) {
            sha1data.final();
            status = compareSha1( fileSha, sha1data.getBuffer() );
        }
    }
        
    HwUtil::deleteBuffer(inBuf);
    HwUtil::deleteBuffer(outBuf);


    return status;
}

/**
 * Checks the input file to make sure that it is a valid
 * DES file.
 *
 * @param fileSize              Size of the input file
 */
int DesDecryptor::checkInputFile( long fileSize ) const {

    int status = 0;
    unsigned char remainder = 0;

    if( fileSize <= HwConstant::DES_HEADER_LENGTH ) {
        status = HwConstant::ERROR_INVALID_INPUT;
        HwUtil::setLastErrorMessage("Invalid DES file. File size is less than DES header size");

    } else {
        fileSize -= ( HwConstant::DES_HEADER_LENGTH + SHA_DIGEST_LENGTH );
        remainder = fileSize % 8;
        
        if( 0 != remainder ) {
            status = HwConstant::ERROR_INVALID_INPUT;
            HwUtil::setLastErrorMessage("Invalid DES file. File size should be multiple of 8");
        }
    }
    
    return status;
}

/**
 * Processes the DES header from the DES file
 * 
 * @param fp                    File pointer
 * @param numByteLastBlock      Number of bytes padded towads the end of the file
 */
int DesDecryptor::processDesHeader( 
            FILE * fp,
            unsigned char &numByteLastBlock ) const {

    int status = 0;
    int readLen = 0;
    std::string msg;
    unsigned char *buffer_ptr = NULL;

    status = HwUtil::createBuffer( HwConstant::DES_HEADER_LENGTH, buffer_ptr );

    if( 0 == status ) {
        readLen = fread( 
                    buffer_ptr, 
                    sizeof(unsigned char), 
                    HwConstant::DES_HEADER_LENGTH, 
                    fp );

        status = HwUtil::checkFileReadStatus( fp );
    }

    if( 0 == status ) {        
        if( HwConstant::DES_HEADER_LENGTH == readLen ) {
            status = verifyDesHeader(buffer_ptr, numByteLastBlock );

        } else {
            status = HwConstant::ERROR_INVALID_INPUT;
            msg = "Invalid DES file. Header length is incorrect. (Expected = ";
            msg += HwUtil::getString( HwConstant::DES_HEADER_LENGTH );
            msg += " actual = ";
            msg += HwUtil::getString( readLen );
            msg += ")";

            HwUtil::setLastErrorMessage(msg);
        }
    }
    
    HwUtil::deleteBuffer( buffer_ptr );

    return status;
}

/**
 * Verifies the DES header
 * 
 * @param buffer_ptr            Buffer
 * @param numByteLastBlock      Number of bytes in the last 8-byte block of the original file
 */
int DesDecryptor::verifyDesHeader( 
        const unsigned char *buffer_ptr,
        unsigned char &numByteLastBlock ) const {
    
    int status = 0;
    char des_array[4];
    std::string errorMsg;
    
    memcpy(des_array, buffer_ptr, 3 );
    des_array[3] = '\0';

    std::string strDes( des_array );

    if( HwConstant::DES_HEADER != strDes ) {
        status = HwConstant::ERROR_INVALID_INPUT;
        errorMsg = "Invalid DES header. Header should start with ";
        errorMsg += HwConstant::DES_HEADER;
        errorMsg += ". Found ";
        errorMsg += strDes;
    }

    if( 0 == status ) {
        numByteLastBlock = buffer_ptr[3];
        
        if( ( numByteLastBlock <=0 ) || ( numByteLastBlock > 8 ) ) {
            status = HwConstant::ERROR_INVALID_INPUT;
            errorMsg = "Invalid DES header. Byte Padding should be between 1 and 8. Found ";
            errorMsg += HwUtil::getString( numByteLastBlock );
        }
    }

    if( 0 != status ) {
        HwUtil::setLastErrorMessage( errorMsg );
    }

    return status;
}

/**
 * Compares the Sha-1 value of original file and Sha-1 value of the
 * decryped file.
 * 
 * @param origSha1              Orignal Sha1 that is stored in the cipher file
 * @param newSha1               Sha1 that is computed for the deciphered text
 *
 * @return                      0 if comparison is successful, else non-zero
 */
int DesDecryptor::compareSha1( 
        const unsigned char * origSha1,
        const unsigned char * newSha1 ) const {

    int status = 0;

    if( (NULL != origSha1) && (NULL != newSha1) ) {
    
        status = memcmp( origSha1, newSha1, SHA_DIGEST_LENGTH );

        if( 0 != status ) {
            std::string str = "\nSha-1 value of the original file and Sha-1 value of the deciphered file does not match\n";
            str += "orig = ";
            str += HwUtil::getHexString( origSha1, SHA_DIGEST_LENGTH );
            str += "\nnew  = ";
            str += HwUtil::getHexString( newSha1, SHA_DIGEST_LENGTH );
        
            HwUtil::setLastErrorMessage( str );
        }
    } else {
        status = HwConstant::ERROR_INVALID_INPUT;
        HwUtil::setLastErrorMessage( "DesDecryptor::compareSha1: Input sha buffers are null" );
    }

    return status;
}
                                                                                                                               desencdump.cc                                                                                       0100600 0270275 0012256 00000001447 10415620151 0015030 0                                                                                                    ustar 00sanjaysn                        usc-maj                         0000566 0005431                                                                                                                                                                        #include "desencdump.h"
#include "desencryptor.h"

/**
 * C'tor.
 *
 * @param filePath          File path
 * @param soPath            Shared libary Path
 */
DesEncDump::DesEncDump( 
                const std::string filePath,
                const std::string soPath ) : BaseDump(filePath, soPath) {
    
}

/**
 * Virtual D'tor.
 */
DesEncDump::~DesEncDump() {}

/**
 * Dumps the output (based on input file or stdin)
 */
int DesEncDump::dump() {

    int status = 0;
    DesEncryptor encryptor;
    
    status = openInputFile( true );

    if( 0 == status ) {
        status = encryptor.init( _soPath, _filePath );
        
        if( 0 == status ) {
            status = encryptor.encrypt( _fp, stdout );
        }

        //close input file
        closeInputFile();
    }
    
    return status;
}

h) {
    
}

/**
 * Virtual D'tor.
 */
DesEncDump::~DesEncDump() {}

/**
 * Dumps the output (based on input file or stdin)
 */
int DesEncDump::dump() {

    int status = 0;
    DesEncryptor encryptor;
    
    statusdesencryptor.cc                                                                                     0100600 0270275 0012256 00000013514 10415620151 0015420 0                                                                                                    ustar 00sanjaysn                        usc-maj                         0000566 0005431                                                                                                                                                                        #include "desencryptor.h"
#include "hwconstant.h"
#include "hwutil.h"
#include "sha1data.h"

/**
 * C'tor.
 */
DesEncryptor::DesEncryptor() : DesBase() {
}

/**
 * Virtual D'tor.
 */
DesEncryptor::~DesEncryptor() { }

/**
 * Intializes the des
 *
 * @param soPath            SoPath
 * @param filePath          File Path
 */
int DesEncryptor::init( 
        const std::string & soPath,
        const std::string & filePath )  {

    int status = 0;
    unsigned char sha1Buf[SHA_DIGEST_LENGTH];

    if( 0 == status ) {
        status = getFileSize( filePath, _fileSize );
    }

    if( 0 == status) {
        if( _pwd.length() <= 0 ) {
            status = getPassword( true, _pwd, sha1Buf );
        } else {
            status = getSha1FromStr(_pwd, sha1Buf );
        }
    }

    if( 0 == status ) {
        status = initDesKeys( sha1Buf, soPath );
    }
    
    if( 0 == status ) {
        _initialized = true;
    }

    return status;
}


/**
 * Sets the password
 *
 * @param password
 */
void DesEncryptor::setPwd( const std::string & password ) {
    _pwd = password;
}

/**
 * encrypts the data and write to fp
 *
 * @param fpFrom            From file pointer
 * @param fpTo              To File pointer
 */
int DesEncryptor::encrypt(
        FILE * fpFrom,
        FILE * fpTo ) {

    int status = 0;

    _fpFrom = fpFrom;
    _fpTo = fpTo;

    status = encrypt();

    return status;
}

/**
 * encrypts the data and write to fp
 *
 * @param fileFrom            From file
 * @param fileTo              To File
 */
int DesEncryptor::encrypt(
        const std::string & fileFrom,
        const std::string & fileTo ) {

    int status = 0;

    _fileFrom = fileFrom;
    _fileTo = fileTo;

    status = encrypt();
    
    if( NULL != _fpFrom ) {
        HwUtil::closeFile( _fpFrom );
        _fpFrom = NULL;
        _fileFrom = "";
    }
    if( NULL != _fpTo ) {
        HwUtil::closeFile( _fpTo );
        _fpTo = NULL;
        _fileTo = "";
    }
    
    return status;
}

/**
 * encrypts the data and write to fp
 */
int DesEncryptor::encrypt() {

    int status = 0;
    long readLen = 0;
    int remainder = 0;
    bool headerWritten = false;
    Sha1Data shaData;
    unsigned char *inBuf  = NULL;
    unsigned char *outBuf = NULL;

    status = validateInit();
    
    if( 0 == status ) {
        status = HwUtil::createBuffer(HwConstant::BUFFER_SIZE, inBuf );
    }
    if( 0 == status ) {
        status = HwUtil::createBuffer(HwConstant::BUFFER_SIZE, outBuf );
    }
    
    if( (0 == status) && (NULL == _fpFrom) ) {
        status = HwUtil::openFile( _fileFrom, "rb", _fpFrom );
    }

    //open the output file if not already open
    if( (0 == status) && (NULL == _fpTo ) ) {
        status = HwUtil::openFile( _fileTo, "wb", _fpTo );
    }

    
    while( (0 == status) && (!feof( _fpFrom)) ) {
    
        memset( inBuf, 0, HwConstant::BUFFER_SIZE );
        memset( outBuf, 0, HwConstant::BUFFER_SIZE );
        
        readLen = fread( inBuf, sizeof(unsigned char), HwConstant::BUFFER_SIZE, _fpFrom );
        status = HwUtil::checkFileReadStatus( _fpFrom );
        
        if( (0 == status) && (readLen > 0)  ) {
        
            //calculate the SHA data for input
            status = shaData.update( inBuf, readLen );
        
            if( (0 == status) && (! headerWritten) ) {
                status = writeHeader( _fpTo );
                headerWritten = true;
            }
            
            if( 0 == status ) {
                //if readlen is not a multiple of 8, we have to pad 
                //the input buffer with 0's until the next 8 byte boundary
                remainder = readLen % 8;

                if( 0 != remainder ) {
                    readLen += ( 8 - remainder );
                }

                DES_ede3_cbc_encrypt( 
                        inBuf, 
                        outBuf, 
                        readLen, 
                        &_ks1,
                        &_ks2, 
                        &_ks3, 
                        (DES_cblock *)_iv, 
                        1);

                status = HwUtil::writeToFile( _fpTo, outBuf, readLen );
            }
        }
    }

    //write the sha1 value of input
    if( 0 == status ) {
        status = shaData.final();
        if( 0 == status ) {
            status = HwUtil::writeToFile( _fpTo, shaData.getBuffer(), SHA_DIGEST_LENGTH );
        }
    }
    
    return status;
}


/**
 * Gets the header
 *
 * @param fp                File pointer to write to
 */
int DesEncryptor::writeHeader( FILE * fp ) const {
    
    int status = 0;
    unsigned char remainder = 0;

    //determine how many bytes are in the last block of the file
    remainder = _fileSize % 8;     

    if( 0 == remainder ) {
        remainder = 8;
    }
    
    unsigned char num[1];
    num[0] = remainder;
    
    HwUtil::writeToFile(fp, HwConstant::DES_HEADER);
    HwUtil::writeToFile(fp, num, 1 );

    return status;
}

/**
 * Intitializes the DES keys
 *
 * @param sha1Buf               Sha1 value
 * @param soPath                So Path
 */
int DesEncryptor::initDesKeys( 
        unsigned char sha1Buf[SHA_DIGEST_LENGTH],
        const std::string & soPath ) {

    int status = 0;

    status = DesBase::initDesKeys( sha1Buf, soPath );

    if( 0 == status ) {
        status = validateKey( &_ks1 );
    }
    if( 0 == status ) {
        status = validateKey( &_ks2 );
    }
    if( 0 == status ) {
        status = validateKey( &_ks3 );
    }

    return status;
}


/**
 * Checks to see if the key is valid
 * 
 * @param ks                DES key schedule
 */
int DesEncryptor::validateKey( DES_key_schedule *ks ) const {

     int status = 0;
 
     if( DES_is_weak_key( (DES_cblock *) ks->ks->cblock) ) {
        
        std::string msg = "Key ";
        msg += HwUtil::getHexString(ks->ks->cblock, 8);
        msg += "is a weak key. Try again";

        HwUtil::writeOutput( msg );
    }

    return status;

}
                                                                                                                                                                                    editordump.cc                                                                                       0100600 0270275 0012256 00000023574 10415620151 0015062 0                                                                                                    ustar 00sanjaysn                        usc-maj                         0000566 0005431                                                                                                                                                                        #include "editordump.h"
#include "hwutil.h"
#include "hwconstant.h"
#include "desencryptor.h"
#include "desdecryptor.h"

#ifndef _WIN32
  #include <sys/stat.h>
  #include <unistd.h>
  #include <dirent.h>
  #include <errno.h>
#endif

/**
 * C'tor.
 *
 * @param filePath          File path
 */
EditorDump::EditorDump( const std::string filePath, const std::string soPath ) 
                : BaseDump( filePath, soPath ) {
}

/**
 * Virtual D'tor.
 */
EditorDump::~EditorDump() { }

/**
 * Dumps the output (based on input)
 */
int EditorDump::dump() {

    int status = 0;
    
    status = performEditOperation();

    return status;
}

//----------------------------------------------------------------

/**
 * Perform the edit operation
 */
int EditorDump::performEditOperation() {

    int status = 0;
    std::string tmpFile;
    
    status = getTempFile( tmpFile );
    
    if( 0 == status ) {
        status = editFile( tmpFile );
    }

    return status;
}

/**
 * Gets the temp file (with complete path)
 *
 * @param tempFile          Temp File
 */
int EditorDump::getTempFile( std::string & tempFile ) const {
    
    int status = 0;
    char * buffer = NULL;

    status = HwUtil::getEnvValue( HwConstant::ENV_HOME, tempFile );

    if( 0 == status ) {
        tempFile += HwConstant::DIR_SEPARATOR + HwConstant::TMP_DIR;
        status = createDir( tempFile );
    }
    
    if( 0 == status ) {
        tempFile += HwConstant::DIR_SEPARATOR + "hwtmpXXXXXX";
            
        status = HwUtil::createCharBuffer( tempFile.length(), buffer );
    }

    if( 0 == status ) {
        memcpy( buffer, tempFile.c_str(), tempFile.length() );

        #ifndef _WIN32
            status = mkstemp( buffer );
        #endif
        
        if( -1 == status ) {
            std::string msg = "Unable to cretate temporary file";
            msg += tempFile;
            HwUtil::setLastErrorMessage(msg);
        } else{
            status = 0;
            tempFile = buffer;
        }
    }

    HwUtil::deleteCharBuffer( buffer );

    return status;
}

/**
 * Creates the File.
 * 
 * @param strFile           File
 */
int EditorDump::createFile( const std::string & strFile ) const {

    int status = 0;
    std::string msg;

    FILE * fp = fopen( strFile.c_str(), "w" );
    if( NULL != fp ) {
        fclose(fp);
        fp = NULL;

    } else {
        status = -1;
        msg = "Unable to create file ";
        msg += strFile;
        HwUtil::setLastErrorMessage(msg);
    }

    return status;
}

/**
 * Deletes the file.
 *
 * @param strFile           File
 */
int EditorDump::deleteFile( const std::string & strFile ) const {
    int status = 0;
    std::string msg; 
    
    status = unlink( strFile.c_str() );

    if( 0 != status ) {
        msg = "Unable to delete file ";
        msg += strFile;
        
        HwUtil::setLastErrorMessage(msg);
    }

    return status;
}


/**
 * Creates the directory specified by the path
 *
 * @param path              Directory path
 */
int EditorDump::createDir( const std::string & path ) const {

    int status = 0;
    
    #ifndef _WIN32

        std::string msg;
        bool bExist = false;

        status = isFileExist( path, true, bExist );
    
        if( (0 == status) && (! bExist ) ) {
            status = mkdir( path.c_str(), 0700 );

            if( 0 != status ) {
                msg = "unable to create directory ";
                msg += path;
            }
        }
    
        if( msg.length() > 0 ) {
            HwUtil::setLastErrorMessage(msg);
        }
    
    #endif

    return status;
}

/**
 * Returns true if the file exists, else false.
 *
 * @param strFile           File
 * @param isDir             True if it is dir
 * @param bExist            return value
 */
int EditorDump::isFileExist( 
                    const std::string & strFile,
                    bool isDir,
                    bool & bExist ) const {
    
    int status = 0;
    
    #ifndef _WIN32

        std::string msg;
        struct stat buf;
    
        errno = 0;
        status = stat( strFile.c_str(), &buf );
    
        if( 0 == status ) {
            bExist = true;

        } else {
            if( ENOENT != errno ) {
                status = -1;
                std::string msg = "Unable to perform stat operation on ";
                msg += strFile;
            } else {
                status = 0;
            }
        }

        if( 0 == status ) {
            if( bExist ) {
                if( isDir ) {
                    if( S_IFDIR != (S_IFDIR & buf.st_mode) ) {
                        msg = strFile;
                        msg += " is not a directory";
                        status = -1;
                    }
                } else {
                    if( S_IFREG != (S_IFREG & buf.st_mode) ) {
                        msg = strFile;
                        msg += " is not a regular file";
                        status = -1;
                    }
                }
            }
        }
    
        if( msg.length() > 0 ) {
            HwUtil::setLastErrorMessage(msg);
        }
    #endif

    
    return status;
}

/**
 * Starts the editor with the given file name
 */
int EditorDump::editFile( const std::string & tmpFile) {

    int status = 0;
    bool bExist = false;
    bool bEncrypt = true;
    std::string msg;
    std::string password;
    long fileSize = 0;
    
    unsigned char shaBefore[SHA_DIGEST_LENGTH];
    unsigned char shaAfter[SHA_DIGEST_LENGTH];

    //------------------------------------------

    if( 0 == status ) {
        status = isFileExist( _filePath, false, bExist );
    }
    
    if( (0 == status) && bExist ) {
        status = HwUtil::getFileSize( _filePath, fileSize );
        
        if( (0 == status) && (fileSize > 0) ) {
            status = decryptFile( tmpFile, password );
        }

        if( 0 == status ) {
            status = getSha1( tmpFile, shaBefore );
        }
    }
    
    if(0 == status) {
        status = startEditor( tmpFile );
    }
    
    if( (0 == status) && bExist ) {
        status = getSha1( tmpFile, shaAfter );
        
        if( 0 == status ) {
            if( 0 == memcmp( shaBefore, shaAfter, SHA_DIGEST_LENGTH ) ) {
                //file did not change
                bEncrypt = false;
            }
        }
    }

    if( (0 == status) && bEncrypt ) {
        status = encryptFile( tmpFile, password );
    }

    //delete the file if temp fil exists. Preserve error if it is there
    //-----------------------------------------------------------------
    int tempStatus = 0;
    std::string tempMsg;

    if( 0 != status ) {
        tempMsg = HwUtil::getLastErrorMessage();
    }
    
    tempStatus = isFileExist( tmpFile, false, bExist );
    
    if( 0 == tempStatus ) {
        tempStatus = deleteFile( tmpFile );
    }
    
    if( 0 != status ) {
        HwUtil::setLastErrorMessage(tempMsg);
    } else{
        status = tempStatus;
    }
    //-----------------------------------------------------------------
    
    return status;
}

/**
 * Decrypts the input file and puts in the tmp File
 *
 * @param tmpFile           Temp file
 * @param password          Password
 */
int EditorDump::decryptFile( 
                    const std::string & tmpFile,
                    std::string & password ) {
    
    int status = 0;
    DesDecryptor decryptor;

    status = decryptor.init( _soPath, _filePath );

    if( 0 == status ) {
        status = decryptor.decrypt( _filePath, tmpFile );
        password = decryptor.getPwd();
    }

    return status;
}

/**
 * Encrypts the input file and puts in the tmp File
 *
 * @param tmpFile           Temp file
 * @param password          Password
 */
int EditorDump::encryptFile( 
        const std::string & tmpFile,
        const std::string & password ) {
    
    int status = 0;
    long tmpFileSize = 0;
    DesEncryptor encryptor;
    
    status = HwUtil::getFileSize( tmpFile, tmpFileSize ); 

    if( 0 == status ) {
        if( tmpFileSize > 0 ) {
            if( password.length() > 0 ) {
                encryptor.setPwd( password );
            }
            status = encryptor.init( _soPath, tmpFile );
        
            if( 0 == status ) {
                status = encryptor.encrypt( tmpFile, _filePath );
            }
        } else {
            FILE * fp = NULL;
            status = HwUtil::openFile( _filePath, "wb", fp );

            if( 0 == status ) {
                HwUtil::closeFile( fp );
            }
        }
    }

    return status;
}

/**
 * Starts the editor
 *
 * @param tmpFile           Tempfile
 */
int EditorDump::startEditor( const std::string & tmpFile ) const {

    int status = 0;
    std::string editor;
    std::string command;

    status = HwUtil::getEnvValue( HwConstant::ENV_EDITOR, editor );

    if( 0 == status ) {
        command = editor + std::string(" ") + tmpFile;
        
        status = system( command.c_str() );
        
        if( -1 != status ) {
            status = 0;
        } else {
            std::string msg = "Unable to exeucute command ";
            msg += command;
            HwUtil::setLastErrorMessage(msg);
        }
    }
    
    return status;
}

/**
 * Gets the sha1 value for the file
 *
 * @param file              File
 * @param sha1Buf           Sha1 buffer
 */
int EditorDump::getSha1( 
                    const std::string & file,
                    unsigned char sha1Buf[SHA_DIGEST_LENGTH] ) const {

    int status = 0;
    FILE * fp = NULL;
    Sha1Data sha1Data;
    unsigned char * shaBuffer = NULL;

    memset(sha1Buf, 0, SHA_DIGEST_LENGTH );

    status = HwUtil::openFile( file, "rb", fp );

    if( 0 == status ) {
        status = sha1Data.calculate( fp );
    }
    if( 0 == status ) {
        shaBuffer = sha1Data.getBuffer();
        
        if( NULL != shaBuffer ) {
            memcpy( sha1Buf, shaBuffer, SHA_DIGEST_LENGTH );
        }
    }
    
    if( NULL != fp  ) {
        HwUtil::closeFile( fp );
        fp = NULL;
    }

    return status;
}

//----------------------------------------------------------------
                                                                                                                                    hw4.cc                                                                                              0100600 0270275 0012256 00000014626 10415620151 0013406 0                                                                                                    ustar 00sanjaysn                        usc-maj                         0000566 0005431                                                                                                                                                                        /**
 * Starting file for Hw4.
 * This file has the main routine.
 *
 * @author sanjay nair (sanjaysn@usc.edu)
 * @version 
 *   03/11/2005 - Created
 */
#include "hwconstant.h"
#include "hwutil.h"

#include "basedump.h"
#include "secretdump.h"
#include "signdump.h"
#include "verifydump.h"
#include "desencdump.h"
#include "desdecdump.h"
#include "editordump.h"

/**
 * Returns true, if the sopath option is valid.
 * If valid, true will be returned and the parameter
 * soPath will be populated with the value.
 * Else, false will be returned
 *
 * @param soOption          So option
 * @param soPath            So Path
 */
bool isValidSoPath( 
        const std::string soOption,    
        std::string & soPath ) {

    bool status = false;

    if( soOption.length() > 0 ) {
        
        int option_len = soOption.length();
        int so_len = HwConstant::PARAM_SO.length();

        if( option_len > so_len ) {
            std::string subStr = soOption.substr(0,so_len);

            if( subStr == HwConstant::PARAM_SO ) {
                soPath = soOption.substr( so_len, option_len );
                status = true;
            }
        }
    }

    return status;
}

/** 
 * Returns the option usage
 */
std::string getOptionUsage( 
                const std::string optionName,
                bool isSoNeeded,
                bool isFileParamPresent,
                bool isFileOptional ) {

    std::string str = "       hw4 " + optionName;

    if( isSoNeeded ) {
        str += " " + HwConstant::PARAM_SO + "sopath";
    }

    if( isFileParamPresent ) {
        str += " ";

        if( isFileOptional ) {
            str += "[";
        }
        
        str += "file";

        if( isFileOptional ) {
            str += "]";
        }
    }

    str += "\n";

    return str;
}


/**
 * Returns the dump pointer based on option.
 *
 */
BaseDump * getDumpPtr( 
            const std::string & option, 
            const std::string & filePath,
            const std::string & soPath ) {

    BaseDump * dump_ptr = NULL;

    if( option == HwConstant::OPTION_SECRET ) {
        dump_ptr = new SecretDump( filePath, soPath );
    } else if( option == HwConstant::OPTION_SIGN ) {
        dump_ptr = new SignDump(filePath, soPath );
    } else if( option == HwConstant::OPTION_VERIFY ) {
        dump_ptr = new VerifyDump(filePath, soPath);
    } else if( option == HwConstant::OPTION_ENC ) {
        dump_ptr = new DesEncDump(filePath, soPath);
    } else if( option == HwConstant::OPTION_DEC ) {
        dump_ptr = new DesDecDump(filePath, soPath);
    } else if( option == HwConstant::OPTION_EDIT ) {
        dump_ptr = new EditorDump(filePath, soPath);
    }

    return dump_ptr;
}

/**
 * Returns the usage for this program
 */
std::string getUsage( std::string prefix ) {
    std::string strUsage;

    if( prefix.length() > 0 ) {
        strUsage = prefix + "\n";
    }

    strUsage += getOptionUsage( HwConstant::OPTION_SECRET,  false, false, false );
    strUsage += getOptionUsage( HwConstant::OPTION_SIGN,    true, true, false );
    strUsage += getOptionUsage( HwConstant::OPTION_VERIFY,  true, true, true );
    strUsage += getOptionUsage( HwConstant::OPTION_ENC,     true, true, false );
    strUsage += getOptionUsage( HwConstant::OPTION_DEC,     true, true, false );
    strUsage += getOptionUsage( HwConstant::OPTION_EDIT,    true, true, false );
    
    return strUsage;
}

/**
 * Returns 0 if the input is valid
 *
 * @param argc              main's argc
 * @param argv              main's argv
 * @param option            Commandline Option
 * @param filePath          File Path from command line.
 * @param soPath            Shared object's path from command line.
 * @param strError          Error String
 */
int isValidInput(
        int argc, 
        char * argv[],
        std::string & option, 
        std::string & filePath,
        std::string & soPath,
        std::string & strError ) {
  
    int status = 0;
    
    strError = "";

    if( (argc < 2) || (argc > 4) ) {
        strError  = "Invalid arguments";
    }
    
    if( strError.length() <= 0 ) {
        option = argv[1];

        if( option == HwConstant::OPTION_SECRET ) {
            if(2 != argc) { 
                strError = std::string("Invalid arguments for option ") + option;
            }
        } else if( (option == HwConstant::OPTION_SIGN)  ||
                   (option == HwConstant::OPTION_ENC)   ||
                   (option == HwConstant::OPTION_DEC)   ||
                   (option == HwConstant::OPTION_EDIT) ) {
            
            if( 4 != argc ) {
                strError = std::string("Invalid arguments for option ") + option;
            } else if( ! isValidSoPath(argv[2], soPath) ) {
                strError  = "Invalid SoPath option";
            } else {
                filePath = argv[3];
            }
        } else if( option == HwConstant::OPTION_VERIFY ) {
            if( ! ((argc >= 3) && (argc <= 4)) ) {
                strError = std::string("Invalid arguments for option ") + option;
            } else if( ! isValidSoPath(argv[2], soPath) ) {
                strError  = "Invalid SoPath option";
            } else {
                if( argc > 3 ) {
                    filePath = argv[3];
                }
            }
        } else {
            strError  = std::string("Invalid option ") + option;
        }
    }
    
    if( strError.length() > 0 ) {
        status = HwConstant::ERROR_INVALID_INPUT;
        strError = std::string("hw4 :") + strError;
        strError = getUsage( strError ); 
    }
    
    return status;
}


/**
 * Main routine for Hw4
 */
int main( int argc, char * argv[] ) {
    int status = 0;
    std::string strError;
    std::string strOption;
    std::string strSoPath;
    std::string strFile;
    BaseDump * dump_ptr = NULL;

    status = isValidInput( argc, argv, strOption, strFile, strSoPath, strError );

    if( 0 == status ) {
        dump_ptr = getDumpPtr(strOption, strFile, strSoPath );
        
        if( NULL == dump_ptr ) {
            status = -1;
            strError = "Error: Unable to get the dump ptr";
        }
        if( 0 == status ) {
            status = dump_ptr->dump();
                
            if( 0 != status ) {
                strError  = "Error: ";
                strError  += HwUtil::getLastErrorMessage();
            }
            //delete the allocated memory
            delete dump_ptr;
        }
    }

    if( 0 != status ) {
        HwUtil::writeError(strError);
    }

    return status;
}
                                                                                                          hwconstant.cc                                                                                       0100600 0270275 0012256 00000005705 10415620151 0015072 0                                                                                                    ustar 00sanjaysn                        usc-maj                         0000566 0005431                                                                                                                                                                        #include "hwconstant.h"

/**
 * Constructor is private
 */
HwConstant::HwConstant() { }

int HwConstant::_errorStart                                 = 0x40000001;

const int HwConstant::ERROR_FILE_OPEN                       = _errorStart++;
const int HwConstant::ERROR_FILE_CLOSE                      = _errorStart++;
const int HwConstant::ERROR_FILE_READ                       = _errorStart++;
const int HwConstant::ERROR_OUT_OF_MEMORY                   = _errorStart++;
const int HwConstant::ERROR_INVALID_INPUT                   = _errorStart++;
const int HwConstant::ERROR_INVALID_STATE                   = _errorStart++;
const int HwConstant::ERROR_DES_WEAK_KEY                    = _errorStart++;
const int HwConstant::ERROR_LIBRARY_LOAD                    = _errorStart++;
const int HwConstant::ERROR_LIBRARY_UNLOAD                  = _errorStart++;
const int HwConstant::ERROR_LIBRARY_RESOLVE                 = _errorStart++;

const std::string HwConstant::OPTION_SECRET                 = "secret";
const std::string HwConstant::OPTION_SIGN                   = "sign";
const std::string HwConstant::OPTION_VERIFY                 = "verify";
const std::string HwConstant::OPTION_ENC                    = "enc";
const std::string HwConstant::OPTION_DEC                    = "dec";
const std::string HwConstant::OPTION_EDIT                   = "edit";
const std::string HwConstant::PARAM_SO                      = "-so=";

const std::string HwConstant::ENV_EDITOR                    = "EDITOR";
const std::string HwConstant::ENV_HOME                      = "HOME";
const std::string HwConstant::TMP_DIR                       = "tmp";
const std::string HwConstant::DIR_SEPARATOR                 = "/";

const std::string HwConstant::DES_HEADER                    = "HW4";

const std::string HwConstant::SYMBOL_SC_INIT                = "SC_init";
const std::string HwConstant::SYMBOL_SC_CLEANUP             = "SC_cleanup";
const std::string HwConstant::SYMBOL_SC_GETSIGNATURESIZE    = "SC_get_signature_size";
const std::string HwConstant::SYMBOL_SC_SIGN                = "SC_sign";
const std::string HwConstant::SYMBOL_SC_3DESKEYGEN          = "SC_3des_key_gen";
const std::string HwConstant::SYMBOL_SC_PUBLICSIZEQUERY     = "SC_public_size_query";
const std::string HwConstant::SYMBOL_SC_PUBLICQUERY         = "SC_public_query";

const int HwConstant::BUFFER_SIZE                           = 1024; 
const int HwConstant::MAX_PASSWD_LENGTH                     = 80;
const int HwConstant::DES_HEADER_LENGTH                     = 4;

const std::string HwConstant::RSA_PRIVATE_KEY               = "rsa_private_key";
const std::string HwConstant::RSA_PUBLIC_EXPONENT           = "rsa_public_exponent";
const std::string HwConstant::RSA_PUBLIC_MODULUS            = "rsa_public_modulus";
const std::string HwConstant::RSA_RANDOM_BITS_1             = "random_bits_1";
const std::string HwConstant::RSA_RANDOM_BITS_2             = "random_bits_2";

const int HwConstant::RSA_PUBLIC_MODULUS_SIZE               = 1024;
                                                           hwlibrary.cc                                                                                        0100600 0270275 0012256 00000011001 10415620151 0014667 0                                                                                                    ustar 00sanjaysn                        usc-maj                         0000566 0005431                                                                                                                                                                        #include "hwlibrary.h"
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

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               hwutil.cc                                                                                           0100600 0270275 0012256 00000022520 10415620151 0014210 0                                                                                                    ustar 00sanjaysn                        usc-maj                         0000566 0005431                                                                                                                                                                        #include "hwutil.h"
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>

#include "hwconstant.h"

/**
 * Constructor is private
 */
HwUtil::HwUtil() { }

/**
 * Converts number to string
 */
std::string HwUtil::getString(int number) {

    std::stringstream converter;
    converter << number;
    
    return converter.str();
}

/**
 * Returns Hex string for the given character
 */
std::string HwUtil::getHexString(unsigned char c ) {

    std::string hexChar;
    char buffer[10];
    sprintf(buffer, "%02x", c); 
   
    hexChar = buffer;

    return hexChar;
}

/**
 * Returns Hex string for the give number
 */
std::string HwUtil::getHexString(unsigned int c ) {

    std::string hexChar;
    char buffer[10];
    sprintf(buffer, "%06x",  c); 
   
    hexChar = buffer;

    return hexChar;
}

/** 
 * Write output in hex format to stdout
 *
 * @param buffer                Buffer to write
 * @param bufferLen             Length of the buffer
 * @param appendNewLine         Insert a newline char at the end
 */
std::string HwUtil::getHexString( 
                const unsigned char * buffer,
                int bufferLen) {

    std::string str;

    if( bufferLen > 0 ) {
        for( int i=0; i<bufferLen; i++ ) {
            str += getHexString( buffer[i] );
        }
    }

    return str;
}

/**
 * Opens the file. 
 * 
 * @param filePath              File Path
 * @param mode                  Mode
 * @param fp                    File pointer
 */
int HwUtil::openFile( 
                const std::string & filePath,
                const std::string & mode,
                FILE *& fp ) {

    int status = 0;

    if( filePath.length() > 0 ) {
        fp = fopen(filePath.c_str(), mode.c_str() );
    } 
    
    if( NULL == fp ) {
        std::string msg =  "Unable to open file \"";
        msg += filePath;
        msg += "\" for reading.";
        HwUtil::setLastErrorMessage(msg);

        status = HwConstant::ERROR_FILE_OPEN;; 
    }
       
    return status;

}

/**
 * Closes the file
 *
 * @param fp                    File pointer
 */
int HwUtil::closeFile( FILE * fp ) {

    int status = 0;

    if( NULL != fp ) {
        status = fclose( fp);

        if( 0 != status ) {
            std::string msg =  "Unable to close file";
            HwUtil::setLastErrorMessage(msg);

            status = HwConstant::ERROR_FILE_CLOSE;
        }
    }

    return status;
}

/**
 * Writes the give string to the file pointed b file pointer.
 *
 * @param data                  Data
 * @param size                  Size
 * @param dataSize              Size of data
 * @param fp                    File pointer
 */
int HwUtil::writeToFile( 
                const void * data,
                unsigned int size,
                unsigned int dataSize,
                FILE * fp ) {
    
    int status = 0;
    std::string msg;
    
    if( NULL != fp ) {
        errno = 0;
        fwrite( data, dataSize, size, fp );
        
        if( 0 != errno  ) {
            msg = "HwUtil::writeToFile: Failed to write to file.";
        }

    } else {
        msg = "HwUtil::writeToFile: File handle is invalid";
    }

    if( msg.size() > 0 ) {
        status = HwConstant::ERROR_INVALID_INPUT;                
        HwUtil::setLastErrorMessage(msg);
    }

    return status;
}

/**
 * Writes the give string to the file pointed b file pointer.
 *
 * @param fp                    Valid file pointer
 * @param str                   String to write
 */
int HwUtil::writeToFile( 
                FILE * fp, 
                const std::string & str) {

    return ( writeToFile( str.c_str(), str.size(), sizeof(char), fp) );
}

/**
 * Writes to file
 *
 * @param fp                    File pointer
 * @param buffer                Charracter buffer
 * @param bufferLen             Length of the buffer
 */
int HwUtil::writeToFile( 
                FILE * fp,
                const unsigned char * buffer, 
                unsigned int bufferLen ) {

    return ( writeToFile( buffer, bufferLen, sizeof(unsigned char), fp) );
}

/**
 * Writes to stdout
 *
 * @param buffer                Charracter buffer
 * @param bufferLen             Length of the buffer
 */
int HwUtil::writeOutput( 
                const unsigned char * buffer, 
                unsigned int bufferLen ) {

    return ( writeToFile( stdout, buffer, bufferLen ) );
}



/**
 * Writes error to stderr. Program will append a newline after 
 * error being written.
 *
 * @param fp                    Valid file pointer
 * @param str                   String to write
 * @param newLine               Append newline
 */
int HwUtil::writeError(const std::string & str, bool newLine ) {
    
    std::string lineStr = str;

    if( newLine ) {
        lineStr += "\n";
    }

    return ( writeToFile(stderr, lineStr) );
}

/**
 * Writes to stdout
 *
 * @param str                   String to write
 */
int HwUtil::writeOutput(const std::string & str) {
    
    return ( writeToFile(stdout, str) );
}

/**
 * Writes to stdout and inserts a new line after the string.
 *
 * @param str                   String to write
 */
int HwUtil::writeOutputLn(const std::string & str) {
    std::string lineStr = str + "\n";
    
    return ( writeOutput(lineStr) );
}

/**
 * Sets the last error message
 */
void HwUtil::setLastErrorMessage(const std::string & str ) {
    _errorMsg = str;
}

/**
 * Returns the last error message
 */
std::string HwUtil::getLastErrorMessage() {
    return _errorMsg;
}

/**
 * Returns the file size of the file
 *
 * @param fileName              Name of the file
 * @param size_out              File Size
 *
 * @return                      0, if successful.
 */
int HwUtil::getFileSize( const std::string & fileName,
                         long & size_out ) {
    int status = 0;
    
    size_out = 0;

    #ifdef WINDOWS
        struct _stat buf;
        status = _stat( fileName.c_str(), & buf );
    #else
        struct stat buf;
        status = stat( fileName.c_str(), & buf );
    #endif
    
    if( 0 != status ) {
        HwUtil::setLastErrorMessage("Unable to get the size of the input file");
    } else {
        size_out = buf.st_size;
    }

    return status;
}

/**
 * Creates the buffer and returns the pointer to buffer.
 * If the buffer could not be created, this method returns
 * error code and buffer pointer will points to NULL.
 *
 * NOTE: Caller need to de-allocate the memory after use.
 *
 * @param size                  Size of the buffer
 * @param buffer_ptr            Reference to buffer pointer
 */
int HwUtil::createBuffer( int size, unsigned char *& buffer_ptr ) {
    
    int status = 0;
    buffer_ptr = NULL;
    
    buffer_ptr = new unsigned char[size];
    
    if( NULL == buffer_ptr ) {
        std::string msg = "Unable to allocate memory for the buffer [";
        msg += HwUtil::getString(size);
        msg += "]";

        HwUtil::setLastErrorMessage(msg);
        status = HwConstant::ERROR_OUT_OF_MEMORY;
    } else {
        memset(buffer_ptr, 0, size );
    }

    return status;
}

/**
 * Deletes the buffer
 */
void HwUtil::deleteBuffer( unsigned char *& buffer_ptr ) {
    
    if( NULL != buffer_ptr ) {
        delete [] buffer_ptr;
        buffer_ptr = NULL;
    }
}

/**
 * Creates the buffer and returns the pointer to buffer.
 * If the buffer could not be created, this method returns
 * error code and buffer pointer will points to NULL.
 *
 * NOTE: Caller need to de-allocate the memory after use.
 *
 * @param size                  Size of the buffer
 * @param buffer_ptr            Reference to buffer pointer
 */
int HwUtil::createCharBuffer( int size, char *& buffer_ptr ) {
    
    int status = 0;
    buffer_ptr = NULL;
    
    buffer_ptr = new char[size];
    
    if( NULL == buffer_ptr ) {
        std::string msg = "Unable to allocate memory for the buffer [";
        msg += HwUtil::getString(size);
        msg += "]";

        HwUtil::setLastErrorMessage(msg);
        status = HwConstant::ERROR_OUT_OF_MEMORY;
    } else {
        memset(buffer_ptr, 0, size );
    }

    return status;
}

/**
 * Deletes the buffer
 */
void HwUtil::deleteCharBuffer( char *& buffer_ptr ) {
    
    if( NULL != buffer_ptr ) {
        delete [] buffer_ptr;
        buffer_ptr = NULL;
    }
}

/**
 * Checks the read status of the file. If the read status is not
 * correct, this method will update the error message and returns 
 * error code.
 */
int HwUtil::checkFileReadStatus( FILE * fp ) {
    int status = 0;
    
    if( NULL != fp ) {
        if( ferror(fp) ) {
            std::string msg = "Reading from input resulted in error.";
            HwUtil::setLastErrorMessage(msg);
            status = HwConstant::ERROR_FILE_READ;
        }
    }

    return status;
}

/**
 * Gets the value of the environment variable.
 * If value is null, then this method will return a non-zero return value.
 *
 * @param strName               Name of the environment variable
 * @param strValue              Valud of the environment variable
 */
int HwUtil::getEnvValue( 
                const std::string & strName,
                std::string & strValue ) {

    int status = 0;
    
    char * value = getenv( strName.c_str() );
    if( NULL == value ) {
        std::string strMsg = "Unable to get the environment variable ";
        strMsg += strName;
        HwUtil::setLastErrorMessage( strMsg );

        status = HwConstant::ERROR_INVALID_INPUT;
    } else {
        strValue = value;
    }

    return status;
}


//static declaration
std::string HwUtil::_errorMsg = "";
Valid file pointer
 * @param str                   String to write
 */
int HwUtil::writeToFile( 
                FILE * fp, 
                const std::string & str) {

    retsecretdump.cc                                                                                       0100600 0270275 0012256 00000017606 10415620151 0015060 0                                                                                                    ustar 00sanjaysn                        usc-maj                         0000566 0005431                                                                                                                                                                        #include "secretdump.h"
#include "hwutil.h"
#include "hwconstant.h"

#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/sha.h>


//-----------------------------------------------------------
//declare the callback funciton for Key Generation
void keyGen_callBack( int status, int iteration, void * cb_arg);
//-----------------------------------------------------------

/**
 * C'tor.
 *
 * @param filePath          File path
 */
SecretDump::SecretDump( const std::string filePath, const std::string soPath ) 
                : BaseDump( filePath, soPath ) {
}

/**
 * Virtual D'tor.
 */
SecretDump::~SecretDump() { }

/**
 * Dumps the output (based on input file or stdin)
 */
int SecretDump::dump() {

    int status = 0;
    RSA * rsa_ptr = NULL;
    char * cb_arg = ".";

    rsa_ptr = RSA_generate_key( 
                HwConstant::RSA_PUBLIC_MODULUS_SIZE,
                RSA_F4,
                &keyGen_callBack,
                cb_arg );
    
    status = static_cast<int>( ERR_get_error() );

    if( (NULL == rsa_ptr) || ( 0 != status ) ) {
        std::string msg = "Unable to generate RSA key. ";

        if( 0 != status ) {
            msg += getOpenSslErrStr( ERR_get_error() );                
        } else {
            status = HwConstant::ERROR_INVALID_INPUT;
        }
    } 

    if( 0 == status ) {
        std::string strCode;
        status = getRsaCode( rsa_ptr, strCode );
    }

    if( NULL != rsa_ptr ) {
        RSA_free( rsa_ptr );
        rsa_ptr = NULL;
    }

    return status;
}


/**
 * Generate the code for the RSA
 *
 * @param rsa_ptr           RSA pointer
 * @param strCode           code returned
 */
int SecretDump::getRsaCode( 
            const RSA *rsa_ptr,
            std::string & strCode ) const {

    int status = 0;
    std::string strTemp;
    
    strCode = "#ifndef SHA_DIGEST_LENGTH\n#include <openssl/sha.h>\n#endif\n\n";
    
    status = getArrayCode( rsa_ptr->d, HwConstant::RSA_PRIVATE_KEY, true, sizeof(BN_ULONG), strTemp );

    if( 0 == status ) {
        strCode += strTemp;

        status = getArrayCode( rsa_ptr->e, HwConstant::RSA_PUBLIC_EXPONENT, true, 3, strTemp );
    }

    if( 0 == status ) {
        strCode += strTemp;

        status = getArrayCode( rsa_ptr->n, HwConstant::RSA_PUBLIC_MODULUS, true, sizeof(BN_ULONG), strTemp );
    }

    if( 0 == status ) {
        strCode += strTemp;

        status = getArrayCodeForRandom( HwConstant::RSA_RANDOM_BITS_1, strTemp );
    }

    if( 0 == status ) {
        strCode += strTemp;

        status = getArrayCodeForRandom( HwConstant::RSA_RANDOM_BITS_2, strTemp );
    }

    if( 0 == status ) {
        strCode += strTemp;
        HwUtil::writeOutputLn( strCode );
    }

    return status;
}

/**
 * Get the aray code for byte array
 *
 * @param buffer            Byte buffer
 * @param bufferLen         Length of the buffer
 * @param name              Name of the array
 * @param constName         Name of the constant
 * @param strCode           code returned
 *
 */
int SecretDump::getArrayCode(
        const unsigned char * buffer,
        int bufferLen,
        const std::string & name,
        bool genSizeConst,
        std::string & strCode ) const {

    int status = 0;

    strCode  = "static unsigned char HW4_";
    strCode += name;
    strCode += "[";

    if( ! genSizeConst ) {
        strCode += "SHA_DIGEST_LENGTH";
    }
    strCode += "] = {\n  ";
        
    for( int i=0; i<bufferLen; i++ ) {
        bool isLastIter = (i == (bufferLen - 1) );

        if( (i>0) && (0 == (i % 8)) ) {
            if( ! isLastIter ) {
                strCode += "\n  ";
            }
        }
        
        strCode += "0x";
        strCode += HwUtil::getHexString( buffer[i] );
        
        if( ! isLastIter ) {
            strCode += ",";
        }
    }
    
    strCode += "\n};";
    
    if( genSizeConst ) {
        strCode += "\n";
        strCode += "static int HW4_";
        strCode += name;
        strCode += "_size = ";
        strCode += HwUtil::getString(bufferLen);
        strCode += ";";
    } 

    strCode += "\n\n";

    return status;
}

/**
 * Gets the Array code for BIGNUM.
 *
 * @param bigNum_ptr        BIGNUM pointer
 * @param name              Name of the array
 * @param genSizeConst      To generate size constant
 * @param lastNumBytes      Number of bytes to be considered in the last number
 * @param strCode           code returned
 */
int SecretDump::getArrayCode( 
                    const BIGNUM * bigNum_ptr,
                    const std::string & name,
                    bool genSizeConst,
                    int lastNumBytes,
                    std::string & strCode ) const {

    
    assert( (lastNumBytes >0) && (lastNumBytes <= 4) );

    int status = 0;
    int bufferLen = 0;
    int retLen = 0;
    unsigned char * buffer = NULL;    
    
    //int dataSize = sizeof(BN_ULONG);
    
    if( (NULL != bigNum_ptr) && ( bigNum_ptr->top > 0 ) ) {
        
        bufferLen = BN_num_bytes(bigNum_ptr);
        HwUtil::createBuffer( bufferLen, buffer );

        retLen = BN_bn2bin(bigNum_ptr, buffer);

        if( retLen != bufferLen  ) {
            std::string msg = "BN_bn2bin did not return correct bytes. (bufferLen = ";
            msg += HwUtil::getString( bufferLen );
            msg += ", retLen = ";
            msg += HwUtil::getString( retLen );
            msg += " )";

            HwUtil::setLastErrorMessage( msg );
            status = HwConstant::ERROR_INVALID_INPUT;
        }
        
        if( 0 == status ) {
            status = getArrayCode( buffer, bufferLen, name, genSizeConst, strCode );
        }

    } else {
        std::string msg = "Invalid BIGNUM for array " + name;
        HwUtil::setLastErrorMessage( msg );
        status = HwConstant::ERROR_INVALID_INPUT;
    }

    HwUtil::deleteBuffer( buffer );

    return status;
}

/**
 * Gets the array code for the random numbers
 * 
 * @param name              Name of the array
 * @param strCode           code returned
 */
int SecretDump::getArrayCodeForRandom( 
        const std::string & name,
        std::string & strCode ) const {

    int status = 0;
    unsigned char * buffer = NULL;

    status = HwUtil::createBuffer( SHA_DIGEST_LENGTH, buffer );

    if( 0 == status ) {
        status = genRandomBytes( buffer, SHA_DIGEST_LENGTH );
    }

    if( 0 == status ) {
        status = getArrayCode( buffer, SHA_DIGEST_LENGTH, name, false, strCode );
    }

    HwUtil::deleteBuffer( buffer );
    
    return status;
}

/**
 * generates the random bytes
 *
 * @param buffer            Buffer to hold data
 * @param bufferLen         Lengh of the buffer
 */
int SecretDump::genRandomBytes (
        unsigned char * buffer,
        int bufferLen ) const {

    int status = 0;
    std::string strError;
    
    status = RAND_bytes( buffer, bufferLen );

    if( 1 == status ) {
        status = 0;
    } else {
        strError = "Unable to generate random numbers. ";
        strError += getOpenSslErrStr( ERR_get_error() );
    }

    return status;
}

//------------------------------------------------------------------------------------
/** 
 * Callback from RSA public key generation
 *
 */
void keyGen_callBack( 
        int status, 
        int iteration, 
        void * cb_arg) {

    std::string str;
    
    str = (char*) cb_arg;
    HwUtil::writeToFile( stderr, str );

    //When the n-th randomly generated prime is rejected as not suitable for the key, 
    //callback(2, n, cb_arg) is called. 
    if( 2 == status ) {
        str = "Randomly generated prime is not suitable for the key";
        HwUtil::writeError( str, false );
    }

    //When a random p has been found with p-1 relatively prime to e, 
    //it is called as callback(3, 0, cb_arg). 
    if( (3 == status) && (0 == iteration) ) {
        HwUtil::writeError( "+", false );
    }

    //The process is then repeated for prime q with callback(3, 1, cb_arg). 
    if( (3 == status) && (1 == iteration) ) {
        HwUtil::writeError( "");
        fflush( stderr );
    }
}


ode( rsa_ptr->d, HwConstant::RSA_PRIVATE_KEY, true, sizeof(BN_ULONG), strTemp );

    if( 0 == status ) {
        strCode sha1data.cc                                                                                         0100600 0270275 0012256 00000005067 10415620152 0014372 0                                                                                                    ustar 00sanjaysn                        usc-maj                         0000566 0005431                                                                                                                                                                        #include "sha1data.h"
#include "hwconstant.h"
#include "hwutil.h"

/**
 * C'tor.
 */
Sha1Data::Sha1Data() {
    _sha1Buffer_ptr = NULL;
    _initialized = false;
}

/**
 * Virtual D'tor.
 */
Sha1Data::~Sha1Data() {
    HwUtil::deleteBuffer( _sha1Buffer_ptr );
}

/**
 * Calculates SHA-1 on the file pointed by fp
 *
 * @param fp                    File pointer
 */
int Sha1Data::calculate( FILE * fp ) {

    int status  = 0;
    int readLen = 0;
    
    unsigned char *buffer_ptr   = NULL;

    status = HwUtil::createBuffer(HwConstant::BUFFER_SIZE, buffer_ptr );
    
    if( 0 == status ) {

        while( (!feof(fp)) && (0 == status) ) {
            memset(buffer_ptr, 0, HwConstant::BUFFER_SIZE );
            readLen = fread( buffer_ptr, 
                             sizeof(unsigned char), 
                             HwConstant::BUFFER_SIZE,
                             fp );

            status = HwUtil::checkFileReadStatus( fp );
                
            if( (0 == status) && (readLen > 0)  ) {
                update( buffer_ptr, readLen ); 
            }
        }

        HwUtil::deleteBuffer( buffer_ptr );

        if( 0 == status ) {
            final();
        }
    }
    
    return status;
}


/**
 * updates the sha-1 with the data
 *
 * @param buffer_ptr            Buffer
 * @param bufferLen             Buffer Len
 */
int Sha1Data::update( 
        const unsigned char * buffer_ptr,
        unsigned long bufferLen ) {

    int status = 0;

    if( !_initialized ) {
        SHA1_Init( &_shaCtx );
        _initialized = true;
    }
    
    SHA1_Update( &_shaCtx, buffer_ptr, bufferLen );

    return status;
}

/**
 * updates the sha-1 with the data
 *
 * @param buffer_ptr            Buffer
 * @param bufferLen             Buffer Len
 */
int Sha1Data::final() {

    int status = 0;

    if( _initialized ) {
        if( NULL == _sha1Buffer_ptr ) {
            status = HwUtil::createBuffer( SHA_DIGEST_LENGTH, _sha1Buffer_ptr );
        }

        if( 0 == status ) {
            SHA1_Final( _sha1Buffer_ptr, &_shaCtx );
        }
    } else {
        status = HwConstant::ERROR_INVALID_STATE;
        HwUtil::setLastErrorMessage("Sha1Data::final: Not initialized");
    }

    return status;
}

/**
 * Returns the sha-1 buffer        
 */
unsigned char * Sha1Data::getBuffer() const {
    return _sha1Buffer_ptr;
}

/**
 * Returns the Hex representation of the sha-1
 */
std::string Sha1Data::getHexString() const {
    std::string str;

    if( NULL != _sha1Buffer_ptr ) {
        str = HwUtil::getHexString( _sha1Buffer_ptr, SHA_DIGEST_LENGTH );
    }

    return str;
}

                                                                                                                                                                                                                                                                                                                                                                                                                                                                         signdump.cc                                                                                         0100600 0270275 0012256 00000003477 10415620152 0014535 0                                                                                                    ustar 00sanjaysn                        usc-maj                         0000566 0005431                                                                                                                                                                        #include "signdump.h"
#include "hwutil.h"
#include "hwconstant.h"
#include "sha1data.h"
#include "smartcardproxy.h"

#include <openssl/rsa.h>

/**
 * C'tor.
 *
 * @param filePath          File path
 */
SignDump::SignDump( const std::string filePath, const std::string soPath ) 
                : BaseDump( filePath, soPath ) {
}

/**
 * Virtual D'tor.
 */
SignDump::~SignDump() { }

/**
 * Dumps the output (based on input)
 */
int SignDump::dump() {

    int status = 0;
    std::string strSign;
    
    status = openInputFile( true );

    if( 0 == status ) {
        status = getSignedStr( strSign );
        
        if( 0 == status ) {
            HwUtil::writeOutputLn( strSign );
        }

        closeInputFile();
    }

    return status;
}

/**
 * Gets the signed SHA-1 signature for the file.
 *
 * @param strSign           Signed string
 */
int SignDump::getSignedStr( std::string & strSign ) const {
    int status = 0;
    int sigSize = 0;
    SmartCardProxy proxy;
    unsigned char sha1_buf[SHA_DIGEST_LENGTH];
    unsigned char * buf_return = NULL;
    
    Sha1Data shaData;
    status = shaData.calculate( _fp );

    if( 0 == status ) {
        memcpy( sha1_buf, shaData.getBuffer(), SHA_DIGEST_LENGTH );
        status = proxy.init( _soPath );
    }
    
    if( 0 == status ) {
        status = proxy.getSignatureSize( sigSize );

        if( 0 == status ) {
            status = HwUtil::createBuffer( sigSize, buf_return );
        }
    
        if( 0 == status ) {
            status = proxy.sign( sha1_buf, buf_return );
        }

        if( 0 == status ) {
            strSign = HwUtil::getHexString( buf_return, sigSize );
        }

    
        int tempStatus = proxy.cleanup();
        if( 0 == status ) {
            status = tempStatus;
        }
    }

    HwUtil::deleteBuffer( buf_return );

    return status;
}

                                                                                                                                                                                                 smartcardproxy.cc                                                                                   0100600 0270275 0012256 00000015457 10415620152 0015772 0                                                                                                    ustar 00sanjaysn                        usc-maj                         0000566 0005431                                                                                                                                                                        #include "smartcardproxy.h"
#include "hwconstant.h"
#include "hwutil.h"

#include <openssl/err.h>
#include <openssl/ssl.h>


/**
 * C'tor.
 */
SmartCardProxy::SmartCardProxy() { 
    _library = NULL;
    _smartCardState = NULL;
}

/**
 * Virtual D'tor.
 */
SmartCardProxy::~SmartCardProxy() { 
    shutdown();
}

/**
 * Returns 0 if the proxy is initialized.
 */
int SmartCardProxy::isInitialized() const {
    int status = 0;

    if( (NULL == _library) || (NULL == _smartCardState) ) {
        status = HwConstant::ERROR_INVALID_STATE;
        HwUtil::setLastErrorMessage("SmartCardProxy is not initialized");
    }

    return status;
}

/**
 * Shuts down the smart card proxy
 */
int SmartCardProxy::shutdown() {

    int status = 0;

    if( NULL != _library ) {
        status = _library->unload();
        delete _library;
        _library = NULL;
    }

    return status;
}

/**
 * Initializes the Smart card
 *
 * @param soPath        Shared library path
 */
int SmartCardProxy::init( const std::string & soPath ) {

    int status = 0;
    void * symbHandle = NULL;
    SC_Init_Func *pfn = NULL;
    
    if( 0 == status ) {
        _library = new HwLibrary(soPath);

        if( NULL == _library ) {
            status = HwConstant::ERROR_OUT_OF_MEMORY;
            HwUtil::setLastErrorMessage("Unable to create HwLibrary");
        }
    }

    if( 0 == status ) {
        status = _library->load();
    }
    
    if( 0 == status ) {
        status = _library->resolve( HwConstant::SYMBOL_SC_INIT, symbHandle );
        
        if( (0 == status) && (NULL != symbHandle) ) {
            pfn = (SC_Init_Func *) symbHandle;
            _smartCardState = (pfn)();
        }
    }

    if( 0 != status ) {
        shutdown();
    }

    return status;
}

/**
 * Cleans up the Smart card
 */
int SmartCardProxy::cleanup() {

    int status = 0;
    void * symbHandle = NULL;
    SC_Cleanup_Func *pfn = NULL;

    status = isInitialized();

    if( 0 == status ) {
        status = _library->resolve( HwConstant::SYMBOL_SC_CLEANUP, symbHandle );
    }

    if( (0 == status) && (NULL != symbHandle) ) {
        pfn = (SC_Cleanup_Func *) symbHandle;
        (pfn)(_smartCardState);
    }
    
    shutdown();

    return status;
}

/**
 * Returns the size of the digital signature.
 *
 * @param size              Signature Size
 */
int SmartCardProxy::getSignatureSize( int & size ) const {

    int status = 0;
    void * symbHandle = NULL;
    SC_GetSignatureSize_Func *pfn = NULL;

    status = _library->resolve( HwConstant::SYMBOL_SC_GETSIGNATURESIZE, symbHandle );

    if( (0 == status) && (NULL != symbHandle) ) {
        pfn = (SC_GetSignatureSize_Func *) symbHandle;

        size = (pfn)(_smartCardState);
        if( -1 == size ) {
            status = HwConstant::ERROR_INVALID_INPUT;
            setLibraryError( HwConstant::SYMBOL_SC_GETSIGNATURESIZE );
        }
    }
    
    return status;
}

/**
 * Signs the Sha1 signature
 *
 * @param sha1_buf              SHA-1 buffer
 * @param buf_return            Return signature
 */
int SmartCardProxy::sign(
        unsigned char sha1_buf[SHA_DIGEST_LENGTH],
        unsigned char * buf_return ) const {

    int status = 0;
    void * symbHandle = NULL;
    SC_Sign_Func *pfn = NULL;

    status = _library->resolve( HwConstant::SYMBOL_SC_SIGN, symbHandle );

    if( (0 == status) && (NULL != symbHandle) ) {
        pfn = (SC_Sign_Func *) symbHandle;

        status = (pfn)(_smartCardState, sha1_buf, buf_return );
        if( 0 != status ) {
            setLibraryError( HwConstant::SYMBOL_SC_SIGN );
        }
    }
    
    return status;
}

/**
 * Returns the public key size query 
 *
 * @param p_public_exponent_size    Public exponent Size
 * @param p_public_modulus_size     Public modulus Size
 */
int SmartCardProxy::publicSizeQuery(
        int * p_public_exponent_size,
        int * p_public_modulus_size ) const {

    int status = 0;

    void * symbHandle = NULL;
    SC_PublicSizeQuery_Func *pfn = NULL;

    status = _library->resolve( HwConstant::SYMBOL_SC_PUBLICSIZEQUERY, symbHandle );

    if( (0 == status) && (NULL != symbHandle) ) {
        pfn = (SC_PublicSizeQuery_Func *) symbHandle;

        status = (pfn)(_smartCardState, p_public_exponent_size, p_public_modulus_size );
        if( 0 != status ) {
            setLibraryError( HwConstant::SYMBOL_SC_PUBLICSIZEQUERY );
        }
    }
    
    return status;

}

/**
 * Returns the public key query 
 *
 * @param p_public_exponent_size    Public exponent
 * @param p_public_modulus_size     Public modulus
 */
int SmartCardProxy::publicQuery(
		unsigned char *public_exponent,
        unsigned char *public_modulus) {

    int status = 0;

    void * symbHandle = NULL;
    SC_PublicQuery_Func *pfn = NULL;

    status = _library->resolve( HwConstant::SYMBOL_SC_PUBLICQUERY, symbHandle );

    if( (0 == status) && (NULL != symbHandle) ) {
        pfn = (SC_PublicQuery_Func *) symbHandle;

        status = (pfn)(_smartCardState, public_exponent, public_modulus );
        if( 0 != status ) {
            setLibraryError( HwConstant::SYMBOL_SC_PUBLICQUERY );
        }
    }

    return status;
}

/**
 * Routine performs secret key generation. 
 * This function returns 0 if successful, otherwise, it should return -1. 
 */
int SmartCardProxy::desKeyGen (
            unsigned char sha1_buf[SHA_DIGEST_LENGTH], 
            unsigned char iv[8],
            unsigned char key1[8], 
            unsigned char key2[8], 
            unsigned char key3[8] ) {

    int status = 0;

    void * symbHandle = NULL;
    SC_3DesKeyGen_Func *pfn = NULL;

    status = _library->resolve( HwConstant::SYMBOL_SC_3DESKEYGEN, symbHandle );

    if( (0 == status) && (NULL != symbHandle) ) {
        pfn = (SC_3DesKeyGen_Func *) symbHandle;

        status = (pfn)(_smartCardState, sha1_buf, iv, key1, key2, key3 ); 
        if( 0 != status ) {
            setLibraryError( HwConstant::SYMBOL_SC_3DESKEYGEN );
        }
    }

    return status;
}


/**
 * Sets the library error.
 */
void SmartCardProxy::setLibraryError( const std::string & methodName ) const {
    std::string str;

    str  = "SmartCard ";
    str += methodName;
    str += " failed (";
    str += SmartCardProxy::getOpenSslError();
    str += " )";

    HwUtil::setLastErrorMessage( str );
}


/**
 * Gets the open ssl error
 */
std::string SmartCardProxy::getOpenSslError() {

    std::string strError;
    
    unsigned long errorCode = ERR_get_error();

    if( 0 != errorCode ) {
        ERR_load_crypto_strings();
        
        strError  = "Error code = ";
        strError += HwUtil::getString( (int) errorCode );
        strError += ", ";
        
        char * openSslError = ERR_error_string( errorCode, NULL );

        if( NULL == openSslError ) {
            strError += "No information available";
        } else {
            strError += openSslError;
        }
        
        ERR_free_strings();
    }

    return strError;
}
                                                                                                                                                                                                                 verifydump.cc                                                                                       0100600 0270275 0012256 00000023410 10415620152 0015066 0                                                                                                    ustar 00sanjaysn                        usc-maj                         0000566 0005431                                                                                                                                                                        #include "verifydump.h"
#include "hwutil.h"
#include "hwconstant.h"
#include "sha1data.h"
#include "smartcardproxy.h"

/**
 * C'tor.
 *
 * @param filePath          File path
 */
VerifyDump::VerifyDump( const std::string filePath, const std::string soPath ) 
                : BaseDump( filePath, soPath ) {
}

/**
 * Virtual D'tor.
 */
VerifyDump::~VerifyDump() { }

/**
 * Dumps the output (based on input)
 */
int VerifyDump::dump() {

    int status                      = 0;
    int modulusSize                 = 0;
    int exponentSize                = 0;
    int dataLen                     = 0;
    unsigned char *modulus_ptr      = NULL;
    unsigned char *exponent_ptr     = NULL;
    unsigned char *dataBuffer_ptr   = NULL;
    std::string strVerify;
    //-------------------------------------------
    
    status = openInputFile( false );

    if( 0 == status ) {
        status = getPublicKeyInfo( 
                        modulus_ptr, 
                        modulusSize, 
                        exponent_ptr, 
                        exponentSize );
    }
    dataLen = modulusSize * 2;

    if( 0 == status ) {
        status = HwUtil::createBuffer(modulusSize, dataBuffer_ptr );
    }

    if( 0 == status ) {
        
        status = getEncyptedData( dataLen, dataBuffer_ptr );
    }

    if( 0 == status ) {
        status = getVerifyedStr(
                        dataBuffer_ptr,
                        modulus_ptr,
                        modulusSize,
                        exponent_ptr,
                        exponentSize,
                        strVerify );
    }

    if( 0 == status ) {
        HwUtil::writeOutputLn( strVerify );
    }

    HwUtil::deleteBuffer( modulus_ptr );
    HwUtil::deleteBuffer( exponent_ptr );
    HwUtil::deleteBuffer( dataBuffer_ptr );

    closeInputFile();

    return status;
}

/**
 * Gets the encrypted data.
 *
 * @param dataLen               [in] Length of data
 * @param dataBuffer            [out] Buffer containing data.
 *                                    Buffer should be allocated to dataLen / 2.
 *                              
 */
int VerifyDump::getEncyptedData( 
            int dataLen,
            unsigned char * dataBuffer ) const {

    int status      = 0;
    int readLen     = 0;
    unsigned char * buffer_ptr= NULL;
    
    status = HwUtil::createBuffer( dataLen, buffer_ptr );

    if( 0 == status ) {
        readLen = fread( buffer_ptr, 
                         sizeof(unsigned char), 
                         dataLen,
                         _fp );

        if( readLen != dataLen ) {
            std::string msg = "Invalid number of bytes returned from file. Extected = ";
            msg += HwUtil::getString( dataLen );
            msg += ", Actual = ";
            msg += HwUtil::getString( readLen );
    
            HwUtil::setLastErrorMessage( msg );
            status = HwConstant::ERROR_FILE_READ;
        }
    }

    if( 0 == status ) {
        char * str_ptr = NULL;
        status = HwUtil::createCharBuffer( dataLen + 1, str_ptr );

        if( 0 == status ) {
            memcpy(str_ptr, buffer_ptr, dataLen );
            str_ptr[dataLen] = '\0';
        }

        if( 0 == status ) {
            status = getOctetArray( 
                std::string(str_ptr), 
                            dataLen, 
                            dataBuffer );
        }

        HwUtil::deleteCharBuffer( str_ptr );
    }

    HwUtil::deleteBuffer( buffer_ptr );

    return status;
}

/**
 * Returns the octet array for the given hex string.
 * 
 * @param hexString             [in] Hex String
 * @param dataLen               [in] Expected data length
 * @param dataBuffer            [out] converted buffer
 */
int VerifyDump::getOctetArray( 
        const std::string & hexString,
        int dataLen,
        unsigned char * dataBuffer ) const {
    
    int status = 0;
    int size = 0;
    BIGNUM *bigNum_ptr = NULL;
    std::string strError;
    
    bigNum_ptr = BN_new();

    if( NULL == bigNum_ptr ) {
        status = HwConstant::ERROR_OUT_OF_MEMORY;
        HwUtil::setLastErrorMessage("Unable to create BIGNUM");
    }

    if( 0 == status ) {
        size = BN_hex2bn( &bigNum_ptr, hexString.c_str() );
        
        if( 0 == size ) {
            strError = "Invalid input: unable to convert hex number to byte array. 0 returned";
        } else if( size > dataLen ) {
            strError = "Invalid input: unable to convert hex number to byte array. expcted = ";
            strError += HwUtil::getString( dataLen );
            strError += " actual = ";
            strError += HwUtil::getString( size );
        }

        if( strError.length() > 0 ) {
            status = HwConstant::ERROR_INVALID_INPUT;
            HwUtil::setLastErrorMessage(strError);
        }
    }

    if( 0 == status ) {
        size = BN_bn2bin(bigNum_ptr, dataBuffer);

        if( size != (dataLen/2)  ) {
            strError = "BN_bn2bin did not return correct bytes. (size = ";
            strError += HwUtil::getString( size );
            strError += ", dataLen = ";
            strError += HwUtil::getString( dataLen/2 );
            strError += " )";

            HwUtil::setLastErrorMessage( strError );
            status = HwConstant::ERROR_INVALID_INPUT;
        }
    }

    if( NULL != bigNum_ptr ) {
        BN_free(bigNum_ptr);
        bigNum_ptr = NULL;
    }

    return status;
}


/**
 * Gets the signed SHA-1 signature for the file.
 *
 * @param data_ptr              [in] Data
 * @param modulus_ptr           [in] Modulus 
 * @param modulusSize           [in] Modulus size
 * @param exponent_ptr          [in] Exponent 
 * @param exponentSize          [in] Exponent size
 * @param strVerify             [out]Verifyed string
 */
int VerifyDump::getVerifyedStr( 
                    unsigned char *data_ptr,
                    const unsigned char *modulus_ptr,
                    int modulusSize,
                    const unsigned char *exponent_ptr,
                    int exponentSize,
                    std::string & strVerify ) const {

    int status = 0;
    int size = 0;
    RSA * rsa = NULL;
    unsigned char * buf_return = NULL;

    status = HwUtil::createBuffer( size, buf_return );

    status = getNewRsa( 
                modulus_ptr, 
                modulusSize, 
                exponent_ptr, 
                exponentSize, 
                rsa ); 

    if( 0 == status ) {
        size = RSA_size(rsa);

        if( size <= 0 ) {
            status = HwConstant::ERROR_INVALID_INPUT;
            HwUtil::setLastErrorMessage("Invalid RSA size (0) ");
        }
    }

    if( 0 == status ) {
        status = HwUtil::createBuffer( size, buf_return );
    }

    if( 0 == status ) {
        size = RSA_public_decrypt( 
                    modulusSize, 
                    data_ptr, 
                    buf_return, 
                    rsa, 
                    RSA_PKCS1_PADDING );

        if( -1 == size ) {
            std::string msg = "Error calling RSA_public_decrypt. ";
            msg += SmartCardProxy::getOpenSslError();
            HwUtil::setLastErrorMessage( msg );
            status = -1;
        }
    }

    if( 0 == status ) {
        strVerify = HwUtil::getHexString( buf_return, SHA_DIGEST_LENGTH );
    }
   
    HwUtil::deleteBuffer( buf_return );
    if( NULL != rsa ) {
        RSA_free( rsa );
    }

    return status;
}

/**
 * Gets the public key info.
 *
 * @param modulus_ptr           [out] Modulus 
 * @param modulusSize           [out] Modulus size
 * @param exponent_ptr          [out] Exponent 
 * @param exponentSize          [out] Exponent size
 */
int VerifyDump::getPublicKeyInfo(
        unsigned char *& modulus_ptr,
        int & modulusSize,
        unsigned char *& exponent_ptr,
        int & exponentSize ) const {

    int status = 0;
    
    SmartCardProxy proxy;

    status = proxy.init( _soPath );
    
    if( 0 == status ) {
        status = proxy.publicSizeQuery( &exponentSize, &modulusSize );
    }

    if( 0 == status ) {
        status = HwUtil::createBuffer( modulusSize, modulus_ptr );
    }
    if( 0 == status ) {
        status = HwUtil::createBuffer( exponentSize, exponent_ptr );
    }

    if( 0 == status ) {
        status = proxy.publicQuery( exponent_ptr, modulus_ptr );
    }
   
    if( 0 == status ) {
        if( (NULL == exponent_ptr) || (NULL == modulus_ptr) ) {
            HwUtil::setLastErrorMessage("Invalid exponent of modulus ");
            status = HwConstant::ERROR_OUT_OF_MEMORY;
        }
    }

    if( 0 == status ) {
        if( SHA_DIGEST_LENGTH > (modulusSize) ) {
            std::string msg = "Invalid modulus size. Expected = ";
            msg += HwUtil::getString(modulusSize);
            msg += ", Actual = ";
            msg += HwUtil::getString(SHA_DIGEST_LENGTH);

            HwUtil::setLastErrorMessage( msg );
            status = HwConstant::ERROR_INVALID_INPUT;
        }
    }

    return status;
}

/**
 * Creates a new RSA and initializes it with the keys.
 * CALLER MUST CALL RSA_free(RSA *rsa) to free the memory created by 
 * this call.
 *
 * If RSA cannot be created, this method will return -1. Else 0.
 *
 * @param modulus_ptr           [in] Modulus 
 * @param modulusSize           [in] Modulus size
 * @param exponent_ptr          [in] Exponent 
 * @param exponentSize          [in] Exponent size
 * @param rsa                   [out] RSA to be created
 */ 
int VerifyDump::getNewRsa( 
        const unsigned char * modulus_ptr,
        int modulusSize,
        const unsigned char * exponent_ptr,
        int exponentSize,
        RSA *& rsa ) const {

    int status = 0;

    rsa = RSA_new();

    if( NULL != rsa ) {
        rsa->n = BN_bin2bn( modulus_ptr, modulusSize, rsa->n );
        rsa->e = BN_bin2bn( exponent_ptr, exponentSize, rsa->e );
    } else {
        HwUtil::setLastErrorMessage(" Unable to create new rsa structure");
        status = HwConstant::ERROR_OUT_OF_MEMORY;
    }

    return status;
}

                                                                                                                                                                                                                                                        secret.c                                                                                            0100400 0270275 0012256 00000004076 10415624710 0014027 0                                                                                                    ustar 00sanjaysn                        usc-maj                         0000566 0005431                                                                                                                                                                        #ifndef SHA_DIGEST_LENGTH
#include <openssl/sha.h>
#endif /* ~SHA_DIGEST_LENGTH */

static unsigned char HW4_rsa_private_key[] = {
  0xbe,0x3c,0x6f,0x3d,0xec,0x9e,0xc7,0xff,
  0x06,0x3f,0x2d,0x5f,0x27,0xd1,0x54,0x24,
  0xd4,0xd5,0x1f,0x83,0x00,0xbe,0x56,0xf1,
  0xc3,0xd8,0x03,0x5e,0x64,0xa8,0xc3,0x6a,
  0x30,0x10,0x9f,0x5b,0xdd,0x77,0x78,0x7e,
  0x60,0xe0,0xdb,0x39,0x66,0x33,0x23,0xad,
  0x30,0x48,0x26,0xac,0x41,0x6c,0x4d,0xda,
  0x2a,0x93,0x38,0x78,0xc4,0xfe,0x59,0x75,
  0xf9,0xfa,0x81,0xb3,0xa7,0x0f,0x79,0xbc,
  0xe0,0x67,0x5f,0x0e,0xd5,0x22,0xba,0x04,
  0x03,0x2c,0x8e,0x26,0xf0,0x2a,0x10,0x81,
  0xdc,0xc3,0x47,0x6a,0x2e,0x60,0x7f,0xc7,
  0x82,0xa0,0x09,0x08,0x7d,0x79,0x3e,0x80,
  0x0e,0xe2,0xa8,0xc8,0x0a,0xcf,0x5a,0xbe,
  0xc2,0xb0,0x67,0xf3,0x54,0x63,0x77,0x0f,
  0x85,0xef,0xf8,0x78,0xfb,0x69,0xba,0xe1
};
static int HW4_rsa_private_key_size=128;

static unsigned char HW4_rsa_public_exponent[] = {
  0x01,0x00,0x01
};
static int HW4_rsa_public_exponent_size=3;

static unsigned char HW4_rsa_public_modulus[] = {
  0xca,0xa2,0x77,0xbf,0xa3,0xfb,0xb5,0x69,
  0xc3,0x85,0x12,0x8d,0xe5,0x4c,0x54,0x4a,
  0x26,0x29,0x95,0x8f,0x40,0x9c,0x24,0xee,
  0x75,0x11,0x0f,0x60,0x31,0x6d,0x2e,0xe6,
  0x69,0xc9,0x73,0x28,0xe8,0x73,0x8f,0xaa,
  0xb1,0x74,0x30,0xa8,0x3c,0x00,0x28,0x58,
  0x20,0x02,0xa0,0x11,0x13,0x83,0x1f,0xb1,
  0xcf,0xff,0xae,0x08,0x5e,0x87,0x75,0xc9,
  0x91,0x1a,0xa0,0xab,0xb1,0x7e,0xef,0xda,
  0xd1,0x34,0xd6,0x51,0xc0,0x3a,0x29,0x19,
  0xf3,0xa4,0xc9,0x81,0xd6,0x16,0x00,0x5d,
  0x75,0x60,0xb2,0x8a,0xa0,0xfd,0xf2,0x22,
  0xe4,0x26,0xda,0xdc,0x60,0x3a,0xdc,0x3d,
  0x7a,0xda,0x9f,0xaf,0xce,0x93,0x71,0xb1,
  0x91,0x64,0x7c,0x61,0x4e,0x43,0x69,0x93,
  0xd7,0x24,0x0a,0x52,0xf3,0x18,0x71,0xfd
};
static int HW4_rsa_public_modulus_size=128;

static unsigned char HW4_random_bits_1[SHA_DIGEST_LENGTH] = {
  0x77,0xfc,0xc3,0x54,0xd0,0xf1,0x3b,0x6c,
  0x65,0x1a,0xa3,0xf2,0x9f,0xe9,0x59,0xa9,
  0x87,0xb0,0x6f,0x6a
};

static unsigned char HW4_random_bits_2[SHA_DIGEST_LENGTH] = {
  0x09,0x67,0xfd,0x1a,0x83,0x32,0x6a,0xb4,
  0xd6,0xb5,0x83,0x12,0xb7,0xac,0x7b,0x80,
  0x77,0x57,0x0e,0x4d
};
                                                                                                                                                                                                                                                                                                                                                                                                                                                                  smartcard.c                                                                                         0100600 0270275 0012256 00000016342 10415620152 0014517 0                                                                                                    ustar 00sanjaysn                        usc-maj                         0000566 0005431                                                                                                                                                                        #include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#include <openssl/des.h>
#include <openssl/rsa.h>
#include <openssl/bn.h>
#include <openssl/sha.h>

#include "scintrf.h"
#include "secret.c"

//-----------------------------------------------------------------------------

/**
 * Returns the Sha1 of smart card
 * Called need to allocate memory of length SHA_DIGEST_LENGTH
 *
 * @param sha1_buf               state
 */
void getSmartCardSha1( unsigned char * sha1_buf ) {

    memset(sha1_buf, 0, SHA_DIGEST_LENGTH);
    SHA1(HW4_rsa_private_key, HW4_rsa_private_key_size, sha1_buf);
}

/**
 * Validates the smart card.
 * If the smart card is not valid, this function
 * returns -1, otherwise 0
 *
 * @param pSS                   SmartCard state
 */
int validateSmartCard( SmartcardState pSS ) {

    return 0;
    
    /*int status = -1;
    unsigned char sha1_buf[SHA_DIGEST_LENGTH];

    if( NULL != pSS ) {
        getSmartCardSha1( sha1_buf );

        if( 0 == memcmp(sha1_buf, pSS, SHA_DIGEST_LENGTH) ) {
            status = 0;
        }
    }

    return status;*/
}

/**
 * Creates a new RSA and initializes it with the keys.
 * CALLER MUST CALL RSA_free(RSA *rsa) to free the memory created by 
 * this call.
 *
 * If RSA cannot be created, this method will return -1. Else 0.
 *
 * @param rsa               RSA to be created
 */ 
RSA * getNewRsa() {

    RSA* rsa = NULL;
    
    rsa = RSA_new();
        
    if( NULL != rsa ) {
        rsa->e = BN_bin2bn( HW4_rsa_public_exponent, HW4_rsa_public_exponent_size, rsa->e );
        rsa->n = BN_bin2bn( HW4_rsa_public_modulus, HW4_rsa_public_modulus_size, rsa->n );
        rsa->d = BN_bin2bn( HW4_rsa_private_key, HW4_rsa_private_key_size, rsa->d );
    }
    
    return rsa;
}

/**
 * Concatenates the array and returns the SHA1 value and the 
 * concatenated array.
 *
 * @param buf1              [in]  First array
 * @param buf2              [in]  Second array
 * @param sha1Buf           [out] Sha1 array
 */
int getSha1(
        const unsigned char buf1[SHA_DIGEST_LENGTH],
        const unsigned char buf2[SHA_DIGEST_LENGTH],
        unsigned char sha1Buf [SHA_DIGEST_LENGTH] ) {

    int status = 0;
    unsigned char resultBuf[2 * SHA_DIGEST_LENGTH];
    
    memcpy( resultBuf, buf1, SHA_DIGEST_LENGTH );
    memcpy( resultBuf + SHA_DIGEST_LENGTH, buf2, SHA_DIGEST_LENGTH );

    sha1Buf = SHA1(resultBuf, 2 * SHA_DIGEST_LENGTH, sha1Buf);

    return status;
}

//-----------------------------------------------------------------------------

/**
 * Initializes the smart card state.
 *
 * The SC_init() function is to be called immediately after 
 * the shared library is loaded. You can create state information 
 * and store it in the SmartcardState. This state information is 
 * to be freed in SC_cleanup().  You must call SC_cleanup() 
 * when you are finished with the shared library.
 */
SmartcardState SC_init(void) {

    unsigned char *sha1_buf=NULL;

    sha1_buf = (unsigned char *)malloc(SHA_DIGEST_LENGTH);
    if (sha1_buf != NULL) {
        getSmartCardSha1( sha1_buf ); 
    }
    
    return (SmartcardState)sha1_buf;
}

/**
 * Cleans up the smart card state.
 *
 * routine frees up all resources used by the smartcard and 
 * any resource associated with SmartcardState. The application 
 * must not call any of the smartcard functions above 
 * (except for SC_init()) after this function is called. 
 */
void SC_cleanup(SmartcardState pSS) {

    unsigned char *sha1_buf=(unsigned char *)pSS;

    if (sha1_buf != NULL) {
        free(sha1_buf);
    }
}

/**
 * Function returns the size of the digital signature. 
 * This is to be used in conjunction with the SC_sign() routine.
 * -1 indicates error.
 */
int SC_get_signature_size( SmartcardState pSS ) {

    int size = -1;

    if( 0 == validateSmartCard(pSS) ) {
        size = HW4_rsa_private_key_size;
    }

    return size;
}

/**
 * Routine writes the digital signature of sha1_buf in buf_return. 
 * Caller must allocate a large enough buf_return 
 * (determined by SC_get_signature_size()) before this routine is called. 
 * This function returns 0 if successful, otherwise, it should return -1.
 */
int SC_sign(
		SmartcardState pSS,
		unsigned char sha1_buf[SHA_DIGEST_LENGTH],
        unsigned char *buf_return) {

    int status = 0;
    int size = 0;
    RSA * rsa = NULL;

    status = validateSmartCard(pSS);

    if( 0 == status ) {
        rsa = getNewRsa();
        
        if( NULL == rsa ) {
            status = -1;

        } else {
            size = RSA_private_encrypt( 
                        SHA_DIGEST_LENGTH, 
                        sha1_buf, 
                        buf_return, 
                        rsa, 
                        RSA_PKCS1_PADDING );

            if( -1 == size ) {
                status = -1;
            }

        }
        
        if( NULL != rsa ) {
            RSA_free( rsa );
            rsa = NULL;
        }
    }

    return status;
}

/**
 * Routine returns the size of the RSA public exponent in 
 * (*p_public_exponent_size_return) and the size of the 
 * RSA public modulus in (*p_public_modulus_size_return). 
 * These values are to be used in conjunction with the 
 * SC_public_query() routine. This function returns 0 if successful, 
 * otherwise, it should return -1.
 */ 
int SC_public_size_query(
		SmartcardState pSS,
		int *p_public_exponent_size_return,
        int *p_public_modulus_size_return) {
    
    int status = 0;

    status = validateSmartCard(pSS);

    if( 0 == status ) {
        *(p_public_exponent_size_return) = HW4_rsa_public_exponent_size;
        *(p_public_modulus_size_return) = HW4_rsa_public_modulus_size;
    }

    return status;
}

/**
 * Routine returns the RSA public exponent in public_exponent_return 
 * and the RSA public modulus in public_modulus_return. Caller must 
 * allocate a large enough space for the buffers 
 * (determined by calling SC_public_size_query()) before this routine 
 * is called. This function returns 0 if successful, 
 * otherwise, it should return -1. 
 */
int SC_public_query(
		SmartcardState pSS,
		unsigned char *public_exponent_return,
        unsigned char *public_modulus_return) {

    int status = 0;

    status = validateSmartCard(pSS);

    if( 0 == status ) {
        memcpy(public_exponent_return, HW4_rsa_public_exponent, HW4_rsa_public_exponent_size );
        memcpy(public_modulus_return, HW4_rsa_public_modulus, HW4_rsa_public_modulus_size );
    }    
    
    return status;
}

/**
 * Routine performs secret key generation. 
 * This function returns 0 if successful, otherwise, it should return -1. 
 */
int SC_3des_key_gen(
		SmartcardState pSS,
		unsigned char sha1_buf[SHA_DIGEST_LENGTH],
		unsigned char iv[8],
		unsigned char key1[8],
		unsigned char key2[8],
        unsigned char key3[8]) {

    int status = 0;

    unsigned char Y[SHA_DIGEST_LENGTH];
    unsigned char Z[SHA_DIGEST_LENGTH];

    memset( Y, 0, SHA_DIGEST_LENGTH );
    memset( Z, 0, SHA_DIGEST_LENGTH );

    status = validateSmartCard(pSS);

    if( 0 == status ) {
        status = getSha1( sha1_buf, HW4_random_bits_1, Y );
    }

    if( 0 == status ) {
        getSha1( Y, HW4_random_bits_2, Z );
    }

    if( 0 == status ) {
        memcpy( iv, Y, 8 );
        memcpy( key1, Y+8, 8 );
        memcpy( key2, Z, 8 );
        memcpy( key3, Z+8, 8 );
    }

    return 0;
}

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              