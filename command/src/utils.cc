#include "uecc/utils.h"

namespace uecc {

uECC_Curve curve4name(std::string name)
{
    if (name == "secp160r1") return uECC_secp160r1();
    if (name == "secp192r1") return uECC_secp192r1();
    if (name == "secp224r1") return uECC_secp224r1();
    if (name == "secp256r1") return uECC_secp256r1();
    if (name == "secp256k1") return uECC_secp256k1();
    return {};
}

/**
 * @brief Converter the hex string to bytes
 * 
 * @param[in] str 
 * @param[out] bytes
 * @return true 
 * @return false 
 */
bool hexTobytes(std::string str, std::vector<uint8_t> &bytes)
{
    if (str.empty() && str.length() % 2) return false;
    uint8_t ch = 0;
    bytes.clear();
    for (size_t i=0; i<str.length(); i++) {
        auto c = str[i];
        ch <<= 4;
        if ( 'a' <= c && c <= 'f') ch |= ( c - 'a' + 10 );
        else if ( 'A' <= c && c <= 'F') ch |= ( c - 'A' + 10 );
        else if ( '0' <= c && c <= '9') ch |= ( c - '0' );
        else return false;
        if ( ((i+1)%2 == 0) || (i+1) == str.length() ) bytes.push_back(ch);
    }
    return true;
}

bool isHexStr(std::string &str)
{
    if (str.empty()) return false;
    for (auto it:str) {
        if ('a'<= it && it <= 'f') continue;
        if ('A'<= it && it <= 'F') continue;
        if ('0'<= it && it <= '9') continue;
        return false;
    }
    return true;
}

}
