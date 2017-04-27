#include <stdio.h>
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

