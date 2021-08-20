#include <thread>

#include <bcli/bcli.hpp>

#include "param.hpp"

namespace mini {

using cli_t = std::shared_ptr<bc::Parser<0>>;

class minimizer_cli
{
public:
  minimizer_cli(const std::string& name, const std::string& desc, const std::string& version)
  {
    m_cli = std::make_shared<bc::Parser<0>>(bc::Parser<0>(name, desc, version, std::string{}));
    m_cli->add_param("-i/--input", "input file")->meta("FILE")->setter(m_param.input)->checker(bc::check::is_file);
    m_cli->add_param("-o/--output", "output file")->meta("FILE")->def("minimizers.txt")->setter(m_param.output);
    m_cli->add_param("-k/--kmer-size", "k-mer size")->meta("INT")->def("32")->setter(m_param.kmer_size)->checker(bc::check::is_number);
    m_cli->add_param("-m/--minim-size", "minimizer size")->meta("INT")->def("10")->setter(m_param.minim_size)->checker(bc::check::is_number);
    m_cli->add_param("--canonical-kmer", "")->as_flag()->setter(m_param.canonical_kmer);
    m_cli->add_param("--canonical-minimizer", "")->as_flag()->setter(m_param.canonical_minim);
    m_cli->add_param("--dump", "dump all minimizer at --output")->as_flag()->setter(m_param.dump_minim);
    m_cli->add_param("-t/--threads", "nb of threads")->meta("INT")->def(std::to_string(std::thread::hardware_concurrency()))->setter(m_param.threads);

    m_cli->add_common("common");
    m_cli->getp("verbose")->setter(m_param.verbose);
    m_cli->getp("debug")->setter(m_param.debug);
  }

  param_t parse(int argc, char** argv)
  {
    m_cli->parse(argc, argv);
    return m_param;
  }

private:
  cli_t m_cli;
  param_t m_param;
};

}
