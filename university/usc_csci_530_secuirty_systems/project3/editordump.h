/**
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

