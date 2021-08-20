#pragma once
#include <string>
#include <sstream>

namespace mini {

struct mini_parameters
{
  std::string input;
  std::string output;
  std::size_t kmer_size;
  std::size_t minim_size;
  std::size_t threads;
  bool dump_minim {false};
  bool canonical_kmer {false};
  bool canonical_minim {false};
  bool verbose {false};
  bool debug {false};

  std::string to_str()
  {
    std::stringstream ss;
    ss << "input=" << input << ",";
    ss << "output=" << output << ",";
    ss << "kmer_size=" << kmer_size << ",";
    ss << "minim_size=" << minim_size << ",";
    ss << "dump_minim=" << dump_minim << ",";
    ss << "canonical_kmer=" << canonical_kmer << ",";
    ss << "canonical_minimizer=" << canonical_minim << ",";
    ss << "threads=" << threads << ",";
    ss << "verbose=" << verbose << ",";
    ss << "debug=" << debug;
    return ss.str();
  }
};

using param_t = struct mini_parameters;

}