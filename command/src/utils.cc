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

}
