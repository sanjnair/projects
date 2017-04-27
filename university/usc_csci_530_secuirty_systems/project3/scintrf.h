#ifndef __SMARTCARD_H__
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


