#include <iostream>
#include <filesystem>

#include <spdlog/spdlog.h>

#include <kmercpp/loop_executor.hpp>
#include <minilib/config.hpp>
#include <minilib/threadpool.hpp>
#include <minilib/fastx_parser.hpp>
#include <minilib/sequence.hpp>

#include "cli.hpp"

#if __cplusplus >= 201703L
    #define throw(...)
    #include <hyperloglog.hpp>
    #undef throw
#endif

namespace fs = std::filesystem;

namespace mini {

template<size_t K>
struct entry_point
{
  void operator()(const param_t& params)
  {
    std::vector<std::string> inputs;

    auto [is_fastx, _] = bc::check::seems_fastx("", params.input);
    if (params.input[0] == '@')
    {
      std::ifstream in(params.input, std::ios::in);
      for (std::string line; std::getline(in, line);)
      {
        auto [is_fastx2, _] = bc::check::seems_fastx("", line);
        if (!is_fastx2)
        {
          spdlog::error("Invalid input: {}.", line);
          std::exit(EXIT_FAILURE);
        }
        inputs.push_back(line);
      }
    }
    else if (is_fastx)
    {
      inputs.push_back(params.input);
    }
    else
    {
      spdlog::error("Invalid input: {}.", params.input);
      std::exit(EXIT_FAILURE);
    }

    ThreadPool pool(params.threads);
    BlockingQueue<std::string> in_queue(10000, 1);
    fastx_parser parser(inputs);
    hll::HyperLogLog kestimate(10);
    std::mutex mutex;
    std::set<uint32_t> minim;
    std::size_t nk = 0;
    for (size_t i=0; i<params.threads; i++)
    {
      auto worker = [&in_queue, &mutex, &minim, &params, &nk, &kestimate](int i)
      {
        std::string s;
        while (in_queue.pop(s))
        {
          process_seq<K>(s, nk, params.kmer_size, params.minim_size, params.canonical_kmer, params.canonical_minim, mutex, minim, kestimate);
        }
      };
      pool.add_task(worker);
    }

    parser.populate(in_queue);
    pool.join_all();

    std::cerr << "Total k-mers = " << nk << std::endl;
    std::cerr << "Unique k-mers (estimate) = " << kestimate.estimate() << std::endl;
    std::cerr << "Max minim = " << std::to_string(std::llround(std::pow(4, params.minim_size))) << std::endl;
    std::cerr << "Minim seen = " << minim.size() << std::endl;

    if (params.dump_minim)
    {
      std::ofstream out(params.output, std::ios::out);
      for (auto& m : minim)
      {
        out << minim_to_str(m, params.minim_size) << "\n";
      }
    }
  }
};

}

int main(int argc, char* argv[])
{
  using namespace mini;

  minimizer_cli cli(PROJECT_NAME, PROJECT_DESC, PROJECT_VER);

  param_t parameters;

  try
  {
    parameters = cli.parse(argc, argv);
  }
  catch (const bc::ex::BCliError& e)
  {
    bc::utils::exit_bcli(e);
    exit(EXIT_FAILURE);
  }

  if (parameters.debug)
    spdlog::set_level(spdlog::level::debug);
  else if (parameters.verbose)
    spdlog::set_level(spdlog::level::info);
  else
    spdlog::set_level(spdlog::level::warn);

  spdlog::debug("Parameters: {}", parameters.to_str());

  kmercpp::const_loop_executor<256, 32>::exec<mini::entry_point>(parameters.kmer_size, parameters);

  return 0;
}