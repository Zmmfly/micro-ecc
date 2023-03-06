#ifndef __UECC_SUBCMD_MKEY_H__
#define __UECC_SUBCMD_MKEY_H__

#include "CLI/CLI.hpp"
#include <vector>
#include <string>

namespace uecc::subcmd {

class mkey
{
public:
    mkey(CLI::App_p root);

private:
    void run();
    void init();

private:
    CLI::App_p m_root;
    CLI::App* m_cmd;
    std::string m_name;
    bool m_pubcompress = false;
    std::string m_prvp, m_pubp;

};

}

#endif /* __UECC_SUBCMD_MKEY_H__ */
