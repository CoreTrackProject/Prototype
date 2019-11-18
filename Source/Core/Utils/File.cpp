#include "File.h"

#include <nfd.h>
#include <boost/log/trivial.hpp>

#include <xxhash.h>
#include <fstream>

bool File::PickSingleFile(std::string &out_filePath) {

    nfdchar_t  *filePath = nullptr;
    nfdresult_t result = NFD_OpenDialog(nullptr, nullptr, &filePath );

    if (result == NFD_OKAY) {
        BOOST_LOG_TRIVIAL(info) << "File selected: " << filePath;

        out_filePath.assign(filePath);
        return true;
        //free(filePath);

    } else if (result == NFD_CANCEL) {
        BOOST_LOG_TRIVIAL(info) << "File selection has been canceled";

    } else {
        BOOST_LOG_TRIVIAL(info) << "NFD Error: " << NFD_GetError();
    }

    return false;

}

std::string File::CalcFileHash(std::string filePath) {

    /* create a hash state */
    XXH64_state_t* const state = XXH64_createState();
    if (state==nullptr) {abort();}


    /* Initialize state with selected seed */
    XXH64_hash_t const seed = 0;   /* or any other value */
    if (XXH64_reset(state, seed) == XXH_ERROR) { abort(); }



    std::ifstream fileStream;
    fileStream.open(filePath, std::ios::binary);
    char buff[1000];
    fileStream.seekg (0, std::ios::beg);



    // read 1000 byte chunks and feed that to xxhash
    while(!fileStream.eof()) {
        fileStream.read(buff, 1000);
        if (XXH64_update(state, buff, 1000) == XXH_ERROR)  { abort(); }
    }
    fileStream.close();


    /* Get the hash */
    XXH64_hash_t const hash = XXH64_digest(state);
    XXH64_freeState(state);

    return std::to_string(hash);
}
