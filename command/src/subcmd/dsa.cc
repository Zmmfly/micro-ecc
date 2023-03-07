#include "subcmd/dsa.h"
#include "uecc/mio.hpp"
#include "uecc/utils.h"
#include "spdlog/spdlog.h"
#include "cryptopp/sha.h"
#include <algorithm>
#include <filesystem>
#include <streambuf>

namespace fs = std::filesystem;

namespace uecc::subcmd {

dsa::dsa(CLI::App_p root)
{
    m_root = root;
    m_cmd = root->add_subcommand("dsa", "ECDSA sign or verify with uecc");
    init();
}

void dsa::run() {
    if (!chkCurve()) {
        spdlog::error("chk curve fail");
        return;
    }
    if (!chkFileAndMap()) {
        spdlog::error("chk file and map fail");
        return;
    }
    if (!chkPubkey()) {
        spdlog::error("chk pubkey fail");
        return;
    }
    if (!chkPrvkey()) {
        spdlog::error("chk prvkey fail");
        return;
    }
    if (!chkSignIO()) {
        spdlog::error("chk sign io fail");
        return;
    }
    if (!fileHash()) {
        spdlog::error("file hash fail");
        return;
    }
    if (!signVerifyOut()) {
        spdlog::error("sign verify or out fail");
        return;
    }
}

bool dsa::chkCurve()
{
    m_curve = curve4name(m_name);
    return m_curve ? true : false;
}

bool dsa::chkFileAndMap()
{
    std::error_code ec;
    m_imap.map(m_fpath, ec);

    if (ec) {
        spdlog::error("map file to memory fail, {}, {}", ec.value(), ec.message());
        return false;
    }

    /* check offset */
    if (m_offset > m_imap.size()) {
        spdlog::error("offset overflow, offset {}, size {}", m_offset, m_imap.size());
        return false;
    }

    /* check length */
    if (m_length == 0) {
        size_t signsize = uECC_curve_public_key_size(m_curve);
        size_t need = m_offset + (m_signATEnd ? signsize : 0) + 1;
        if (need > m_imap.size()) {
            spdlog::error("The length is not specified, the file size({}) is not enough for need({})",
                m_imap.size(), need);
        }
        m_length = m_imap.size() - m_offset - (m_signATEnd ? signsize : 0);

    } else {
        if (m_offset + m_length > m_imap.size()) 
        {
            spdlog::info("offset({}) + length({}) overflow({})", 
                m_offset, m_length, m_imap.size());
            return false;
        }
    }
    return true;
}

bool dsa::chkPubkey()
{
    if (!m_isVerify) return true;
    std::vector<uint8_t> key;
    /* check pub val */
    if (isHexStr(m_pubValPath)) {
        if (m_pIsCompressed && (m_pubValPath.length()/2 != (uECC_curve_private_key_size(m_curve)+1))) {
            spdlog::error("special the pubkey compressed, but size not");
            return false;
        }
        if (!hexTobytes(m_pubValPath, key)) {
            spdlog::error("convert pubkey from hex string fail");
            return false;
        }
    } else {
        mio::mmap_source pmap;
        std::error_code ec;
        pmap.map(m_pubValPath, ec);
        if (ec) {
            spdlog::error("map pubkey fail, {}, {}", ec.value(), ec.message());
            return false;
        }
        if (m_pIsCompressed && (pmap.size()/2 != (uECC_curve_private_key_size(m_curve)+1))) {
            spdlog::error("special the pubkey compressed, but size not");
            return false;
        }

        key.insert(key.begin(), pmap.begin(), pmap.end());
    }

    if (!m_pIsCompressed) {
        m_pubkey = key;
        return true;
    }
    m_pubkey.resize(uECC_curve_public_key_size(m_curve));
    uECC_decompress(key.data(), m_pubkey.data(), m_curve);
    return true;
}

bool dsa::chkPrvkey() {
    if (m_isVerify) return true;
    auto path = fs::path(m_prvPath);
    if (!fs::exists(path) || !fs::is_regular_file(path)) {
        spdlog::error("private key not exist or not regular file");
        return false;
    }

    size_t fsize = fs::file_size(path);
    size_t prv_size = uECC_curve_private_key_size(m_curve);
    if (fsize != fsize) {
        spdlog::error("private key size({}) not match({})", fsize, prv_size);
        return false;
    }

    std::fstream pfs(path, std::ios::binary|std::ios::in);
    if (!pfs.is_open()) {
        spdlog::error("private key open fail");
        return false;
    }

    m_prvkey = std::vector<uint8_t>( (std::istreambuf_iterator<char>(pfs)), (std::istreambuf_iterator<char>()) );
    if (m_prvkey.size() != prv_size) {
        spdlog::error("private key read fail");
        return false;
    }
    return true;
}

bool dsa::chkSignIO() {
    if (!m_isVerify) return true;
    size_t sign_size = uECC_curve_public_key_size(m_curve);
    if (m_signATEnd) {
        /* check range */
        if (m_offset + m_length + sign_size > m_imap.size()) {
            spdlog::error("file size not enough for reading signature");
            return false;
        }

        /* Get signature */
        m_sign.clear();
        m_sign.insert(m_sign.end(), m_imap.begin()+m_offset+m_length, 
            m_imap.begin()+m_offset+m_length + sign_size);
    } else {
        auto path = fs::path(m_signPath);

        /* check exist */
        if (!fs::exists(path) || !fs::is_regular_file(path)) {
            spdlog::error("signature not exist or not regular file");
            return false;
        }

        /* check size */
        size_t fsize = fs::file_size(path);
        if (fsize != sign_size) {
            spdlog::error("signature size({}) not match({})", fsize, sign_size);
            return false;
        }

        /* read file */
        std::fstream sfs(path, std::ios::binary|std::ios::in);
        if (!sfs.is_open()) {
            spdlog::error("open signature file fail");
            return false;
        }
        m_sign = std::vector<uint8_t>( (std::istreambuf_iterator<char>(sfs)), (std::istreambuf_iterator<char>()) );
        sfs.close();
        if (m_sign.size() != sign_size) {
            spdlog::error("read signature fail, size({}) not match({})", m_sign.size(), sign_size);
            return false;
        }
    }
    return true;
}

bool dsa::fileHash() {
    m_hash.resize(CryptoPP::SHA256().DigestSize());
    CryptoPP::SHA256().CalculateDigest(m_hash.data(), (uint8_t*)m_imap.data()+m_offset, m_length);
    spdlog::info("file hash with offset({}), length({}): {:02x}",
        m_offset, m_length, fmt::join(m_hash, ""));
    return true;
}

bool dsa::signVerifyOut() {
    size_t sign_size = uECC_curve_public_key_size(m_curve);

    if (m_isVerify) {
        if (m_sign.size() != sign_size) {
            spdlog::error("verify of signature size({}) not match({})", m_sign.size(), sign_size);
        }

        int ret = uECC_verify(m_pubkey.data(), m_hash.data(), m_hash.size(), m_sign.data(), m_curve);
        spdlog::info("signature verify: {}", ret == 0 ? "fail" : "pass");

    } else {
        m_sign.resize(sign_size);

        /* check private */
        if (m_prvkey.empty()) {
            spdlog::error("private key empty");
            return false;
        }

        /* sign */
        int ret = uECC_sign(m_prvkey.data(), m_hash.data(), m_hash.size(), m_sign.data(), m_curve);
        if (ret == 0) {
            spdlog::error("sign fail, {}", ret);
            return false;
        }
        spdlog::info("signature: {:02x}", fmt::join(m_sign, ""));

        /* write signature at end */
        if (m_signATEnd) {
            m_imap.unmap();
            std::fstream ofs(m_fpath, std::ios::binary|std::ios::out|std::ios::app);
            if (!ofs.is_open()) {
                spdlog::error("sign at end enabled, buf file open fail");
                return false;
            }
            ofs.seekp(m_offset+m_length, std::ios::beg);
            ofs.write((char*)m_sign.data(), m_sign.size());
            ofs.close();
        }

        /* write signature to file */
        if (!m_sign.empty()) {
            std::fstream ofs(m_signPath, std::ios::binary|std::ios::out);
            if (!ofs.is_open()) {
                spdlog::error("open signature output file fail");
                return false;
            }
            ofs.write((char*)m_sign.data(), m_sign.size());
            ofs.close();
        }
    }
    return true;
}

void dsa::init()
{
    m_cmd->add_flag("-v,--verify",      m_isVerify,         "verify signature");
    m_cmd->add_flag("-e,--end",         m_signATEnd,        "sign write to end, or read sign from end");
    m_cmd->add_flag("-c,--compress",    m_pIsCompressed,    "public key is compressed");
    m_cmd->add_option("-l,--length",    m_length,           "data length for sign or verify, if not set, will use all of data");
    m_cmd->add_option("-o,--offset",    m_offset,           "data offset to sign or verify");
    m_cmd->add_option("--hash",         m_hashName,         "the hash name for using, fixed to sha256 for currently");
    m_cmd->add_option("-s,--sign",      m_signPath,         "the sign stored path, or output path");
    m_cmd->add_option("-p,--pub",       m_pubValPath,       "pubkey hex string or path, check hex first");
    m_cmd->add_option("--prv",          m_prvPath,          "prvkey path for sign");
    m_cmd->add_option("file",           m_fpath,            "the file for sign or verify")
        ->required()->check(CLI::ExistingFile);
    m_cmd->callback(std::bind(&dsa::run, this));
}

}