#include "CLI/CLI.hpp"
#include "subcmd/mkey.h"
#include "subcmd/dsa.h"
#include "subcmd/dh.h"

using namespace uecc;

int main(int argc, char **argv)
{
    auto a = std::make_shared<CLI::App>("uECC toolbox");

    subcmd::mkey mkkey(a);

    a->require_subcommand(1);

    CLI11_PARSE(*a, argc, argv);
    return 0;
}
