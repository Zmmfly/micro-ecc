#ifndef __UECC_UTILS_H__
#define __UECC_UTILS_H__

#include "uECC.h"
#include <string>
#include <vector>

namespace uecc{

uECC_Curve curve4name(std::string name);
bool hexTobytes(std::string str, std::vector<uint8_t> &bytes);
bool isHexStr(std::string &str);

}

#endif /* __UECC_UTILS_H__ */
