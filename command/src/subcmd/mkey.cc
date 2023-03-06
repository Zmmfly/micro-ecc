#include "subcmd/mkey.h"
#include "uecc/utils.h"
#include "fmt/format.h"
#include <fstream>

namespace uecc::subcmd {

mkey::mkey(CLI::App_p root) {
    m_root = root;
    m_cmd = root->add_subcommand("mkey", "Make key pair");
    init();
}

void mkey::init(){
    m_cmd->add_option("-n", m_name, "curve name, like [secp160r1 | secp192r1 | scp256k1 ...]")
        ->required();
    m_cmd->add_option("--prv", m_prvp, "private key save path, printf directly if not set");
    m_cmd->add_option("--pub", m_pubp, "pubkey key save path");
    m_cmd->add_flag("-c", m_pubcompress, "using compressed pubkey or not");
    m_cmd->callback(std::bind(&mkey::run, this));
}

void mkey::run() {
    auto curve = curve4name(m_name);
    if (curve == nullptr) {
        printf("curve not support: %s\n", m_name.c_str());
        return;
    }

    std::vector<uint8_t> pub(uECC_curve_public_key_size(curve));
    std::vector<uint8_t> prv(uECC_curve_private_key_size(curve));
    std::vector<uint8_t> pubcompr(uECC_curve_private_key_size(curve) +1);
    std::fstream pufs, prfs;

    if (uECC_make_key(pub.data(), prv.data(), curve) == 0) {
        printf("make key fail\n");
        return;
    }

    /* printf name and public key */
    printf("%13s: %s\n", "Curve name", m_name.c_str());
    printf("%13s: " "\033[1;36m" "%s" "\033[0m" "\n", "Public key", 
        fmt::format("{:02x}", fmt::join(pub, "")).c_str());

    /* compress public key if flag set */
    if (m_pubcompress) {
        uECC_compress(pub.data(), pubcompr.data(), curve);
        printf("%13s: " "\033[1;35m" "%s" "\033[0m" " (compressed)\n", "Public key", 
            fmt::format("{:02x}", fmt::join(pubcompr, "")).c_str() );
    }

    /* printf private key if path empty */
    if (m_prvp.empty()) {
        printf("%13s: " "\033[1;33m" "%s" "\033[0m" "\n", "Private key", 
            fmt::format("{:02x}", fmt::join(prv, "")).c_str());
    } else {
        prfs.open(m_prvp, std::ios::binary|std::ios::out|std::ios::trunc);
        if ( !prfs.good() ) {
            printf("open private %s fail\n", m_prvp.c_str());
            return;
        }
        prfs.write((char*)prv.data(), prv.size());
    }

    /* write to file if path valid */
    if (!m_pubp.empty()) {
        pufs.open(m_pubp, std::ios::binary|std::ios::out|std::ios::trunc);
        if (!pufs.good()) {
            printf("open public %s fail\n", m_pubp.c_str());
            return;
        }
        pufs.write( (char*)(m_pubcompress ? pubcompr.data() : pub.data()), 
            m_pubcompress ? pubcompr.size() : pub.size());
    }
}

}