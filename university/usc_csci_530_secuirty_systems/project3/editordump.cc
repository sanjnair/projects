#include "editordump.h"
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
