
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>

#include <string>
#include <iostream>
#include <fstream>

#include "common.h"

#include <openssl/evp.h>
#include <openssl/aes.h>

#pragma comment(lib, "libcrypto.lib")

using std::ifstream, std::ofstream, std::ios, std::cout, std::endl;

/**
 * Create a 256 bit key and IV using the supplied key_data. salt can be added for taste.
 * Fills in the encryption and decryption ctx objects and returns 0 on success
 **/
int aes_init(unsigned char* key_data, int key_data_len, unsigned char* salt, EVP_CIPHER_CTX* e_ctx,
    EVP_CIPHER_CTX* d_ctx)
{
    int i, nrounds = 5;
    unsigned char key[32], iv[32];

    /*
     * Gen key & IV for AES 256 CBC mode. A SHA1 digest is used to hash the supplied key material.
     * nrounds is the number of times the we hash the material. More rounds are more secure but
     * slower.
     */
    i = EVP_BytesToKey(EVP_aes_256_cbc(), EVP_sha1(), salt, key_data, key_data_len, nrounds, key, iv);
    if (i != 32) {
        printf("Key size is %d bits - should be 256 bits\n", i);
        return -1;
    }

    EVP_CIPHER_CTX_init(e_ctx);
    EVP_EncryptInit_ex(e_ctx, EVP_aes_256_cbc(), NULL, key, iv);
    EVP_CIPHER_CTX_init(d_ctx);
    EVP_DecryptInit_ex(d_ctx, EVP_aes_256_cbc(), NULL, key, iv);

    return 0;
}

/*
 * Encrypt *len bytes of data
 * All data going in & out is considered binary (unsigned char[])
 */
unsigned char* aes_encrypt(EVP_CIPHER_CTX* e, unsigned char* plaintext, int* len)
{
    /* max ciphertext len for a n bytes of plaintext is n + AES_BLOCK_SIZE -1 bytes */
    int c_len = *len + AES_BLOCK_SIZE, f_len = 0;
    unsigned char* ciphertext = static_cast<unsigned char*>(malloc(c_len));

    /* allows reusing of 'e' for multiple encryption cycles */
    EVP_EncryptInit_ex(e, NULL, NULL, NULL, NULL);

    /* update ciphertext, c_len is filled with the length of ciphertext generated,
      *len is the size of plaintext in bytes */
    EVP_EncryptUpdate(e, ciphertext, &c_len, plaintext, *len);

    /* update ciphertext with the final remaining bytes */
    EVP_EncryptFinal_ex(e, ciphertext + c_len, &f_len);

    *len = c_len + f_len;
    return ciphertext;
}

/*
 * Decrypt *len bytes of ciphertext
 */
unsigned char* aes_decrypt(EVP_CIPHER_CTX* e, unsigned char* ciphertext, int* len)
{
    /* plaintext will always be equal to or lesser than length of ciphertext*/
    int p_len = *len, f_len = 0;
    unsigned char* plaintext = static_cast<unsigned char*>(malloc(p_len));

    EVP_DecryptInit_ex(e, NULL, NULL, NULL, NULL);
    EVP_DecryptUpdate(e, plaintext, &p_len, ciphertext, *len);
    EVP_DecryptFinal_ex(e, plaintext + p_len, &f_len);

    *len = p_len + f_len;
    return plaintext;
}

void openssl_aes_test(int argc, TCHAR* argv[])
{

    string infile = utf8_encode(argv[1]);
    string outfile = utf8_encode(argv[2]);
    string outfile2 = utf8_encode(argv[3]);

    ifstream ifs{ infile, ios::binary | ios::in };
    if (!ifs)
    {
        cout << "couldn't open " << infile << " for reading" << endl;
        return;
    }

    ofstream ofs{ outfile, ios::binary | ios::out | ios::trunc };
    if (!ofs)
    {
        cout << "couldn't open " << outfile << " for writing" << endl;
        return;
    }

    ofstream ofs2{ outfile2, ios::binary | ios::out | ios::trunc };
    if (!ofs2)
    {
        cout << "couldn't open " << outfile2 << " for writing" << endl;
        return;
    }


    /* "opaque" encryption, decryption ctx structures that libcrypto uses to record
       status of enc/dec operations */
    EVP_CIPHER_CTX *en = EVP_CIPHER_CTX_new(),
        *de = EVP_CIPHER_CTX_new();


    /* 8 bytes to salt the key_data during key generation. This is an example of
       compiled in salt. We just read the bit pattern created by these two 4 byte
       integers on the stack as 64 bits of contigous salt material -
       of course this only works if sizeof(int) >= 4 */
    unsigned int salt[] = { 12345, 54321 };
    unsigned char key_data[] = "this is a test";
    int key_data_len;

    /* the key_data is read from the argument list */
    key_data_len = sizeof(key_data);

    /* gen key and iv. init the cipher ctx object */
    if (aes_init(key_data, key_data_len, (unsigned char*)&salt, en, de)) {
        printf("Couldn't initialize AES cipher\n");
        return ;
    }

    /* encrypt and decrypt each input string and compare with the original */
    char* plaintext;
    unsigned char* ciphertext;
    int olen, len;

    /* The enc/dec functions deal with binary data and not C strings. strlen() will
        return length of the string without counting the '\0' string marker. We always
        pass in the marker byte to the encrypt/decrypt functions so that after decryption
        we end up with a legal C string
        */

        // get length of file:
    ifs.seekg(0, ifs.end);
    int length = ifs.tellg();
    ifs.seekg(0, ifs.beg);

    char* buffer = new char[length];

    std::cout << "Reading " << length << " characters... ";
    // read data as a block:
    ifs.read(buffer, length);

    ifs.close();

    olen = len = length;

    ciphertext = aes_encrypt(en, (unsigned char*)buffer, &len);

    ofs.write(reinterpret_cast<const char*>(ciphertext), len);
    ofs.close();

    plaintext = (char*)aes_decrypt(de, ciphertext, &len);

    ofs2.write(reinterpret_cast<const char*>(plaintext), len);
    ofs2.close();

    if (strncmp(plaintext, buffer, olen))
        printf("FAIL: enc/dec failed for \"%s\"\n", buffer);
    else
        printf("OK: enc/dec ok for \"%s\"\n", plaintext);

    free(ciphertext);
    free(plaintext);
    delete[] buffer;

    EVP_CIPHER_CTX_cleanup(en);
    EVP_CIPHER_CTX_cleanup(de);

    EVP_CIPHER_CTX_free(en);
    EVP_CIPHER_CTX_free(de);
}
