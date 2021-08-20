#include <string>
#include <kseq++/seqio.hpp>

#include <minilib/blocking_queue.hpp>

namespace mini {

class fastx_parser
{
public:
  fastx_parser(const std::vector<std::string>& paths) : m_paths(paths) {}

  void populate(BlockingQueue<std::string>& queue)
  {
    for (auto& p : m_paths)
    {
      klibpp::KSeq record;
      klibpp::SeqStreamIn iss(p.c_str());
      while (iss >> record)
      {
        queue.push(std::move(record.seq));
      }
    }
    queue.end_signal();
  }

private:
  std::vector<std::string> m_paths;
};

}