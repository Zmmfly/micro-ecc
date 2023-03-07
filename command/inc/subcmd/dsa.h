#ifndef __UECC_SUBCMD_DSA_H__
#define __UECC_SUBCMD_DSA_H__

#include "CLI/CLI.hpp"
#include "uecc/mio.hpp"
#include "uECC.h"
#include <fstream>

namespace uecc::subcmd {

class dsa
{
public:
    dsa(CLI::App_p root);

private:
    void run();
    void init();

private:
    bool chkCurve();
    bool chkFileAndMap();
    bool chkPubkey();
    bool chkPrvkey();
    bool chkSignIO();
    bool fileHash();
    bool signVerifyOut();

private:
    CLI::App_p m_root;
    CLI::App* m_cmd;

    bool        m_isVerify      = false;
    bool        m_signATEnd     = false;
    bool        m_pIsCompressed = false;
    size_t      m_length        = 0;
    size_t      m_offset        = 0;
    std::string m_name          = "secp256k1";
    std::string m_hashName      = "sha256";
    std::string m_signPath      = {};
    std::string m_pubValPath    = {};
    std::string m_prvPath       = {};
    std::string m_fpath         = {};
    std::vector<uint8_t> m_pubkey;
    std::vector<uint8_t> m_prvkey;
    std::vector<uint8_t> m_sign;
    std::vector<uint8_t> m_hash;
    mio::mmap_source m_imap;
    std::fstream m_ifs, m_sfs;
    uECC_Curve m_curve = nullptr;
};

}

#endif /* __UECC_SUBCMD_DSA_H__ */
