#include <tchar.h>

#include <iomanip>

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include "common.h"

#include <openssl/evp.h>
#pragma comment(lib, "libcrypto.lib")

using std::ifstream, std::ofstream, std::ios, std::cout, std::endl;

bool openssl_sha(const std::string& unhashed, std::string& hashed)
{
    bool success = false;

    EVP_MD_CTX* context = EVP_MD_CTX_new();

    if (context != NULL)
    {
        if (EVP_DigestInit_ex(context, EVP_sha256(), NULL))
        {
            if (EVP_DigestUpdate(context, unhashed.c_str(), unhashed.length()))
            {
                unsigned char hash[EVP_MAX_MD_SIZE];
                unsigned int lengthOfHash = 0;

                if (EVP_DigestFinal_ex(context, hash, &lengthOfHash))
                {
                    std::stringstream ss;
                    for (unsigned int i = 0; i < lengthOfHash; ++i)
                    {
                        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
                    }

                    hashed = ss.str();
                    success = true;
                }
            }
        }

        EVP_MD_CTX_free(context);
    }

    return success;
}

void openssl_sha_test(int argc, TCHAR* argv[])
{
    string infile = utf8_encode(argv[1]);

    ifstream ifs{ infile, ios::binary | ios::in };
    if (!ifs)
    {
        cout << "couldn't open " << infile << " for reading" << endl;
        return;
    }


    ifs.seekg(0, ifs.end);
    int length = ifs.tellg();
    ifs.seekg(0, ifs.beg);

    string unhashed, hashed;
    unhashed.resize(length);
    
    std::cout << "Reading " << length << " characters... ";
    // read data as a block:
    ifs.read(&unhashed[0], length);

    ifs.close();

    openssl_sha(unhashed, hashed);

    std::cout << "hash: " << hashed << std::endl;
}