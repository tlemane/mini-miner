#include <kmercpp/kmer.hpp>
#include <set>
#include <mutex>

#if __cplusplus >= 201703L
    #define throw(...)
    #include <hyperloglog.hpp>
    #undef throw
#endif

namespace mini {

template<std::size_t K>
uint32_t get_minimizer(const kmercpp::Kmer<K>& kmer, std::size_t size, bool canonical_minimizer)
{
  uint32_t m = std::numeric_limits<uint32_t>::max();
  for (size_t i=0; i<kmer.m_kmer_size-size+1; i++)
  {
    uint32_t v = 0;
    for (size_t j=i; j<i+size; j++)
    {
      v <<= 2; v |= kmer.byte_at(j);
    }
    uint32_t rev = 0;
    if (canonical_minimizer)
    {
      uint32_t tmp = v;
      for (int j=size-1; j>=0; j--)
      {
        rev <<= 2; rev |= kmercpp::revB[tmp & 3];
      }

      if (rev < v) m = rev;
      else m = v;
    }
    else
    {
      m = v;
    }
  }
  return m;
}

std::string minim_to_str(uint32_t m, std::size_t size)
{
  int i;
  uint32_t tmp = m;
  char seq[17];
  for (i=size-1; i>=0; i--)
  {
      seq[i] = kmercpp::bToN[tmp & 3];
      tmp = tmp >> 2;
  }
  seq[size]='\0';
  return seq;

}

template<std::size_t K>
void process_seq(const std::string& s, std::size_t& nkmer, std::size_t kmer_size, std::size_t m_size, bool canok, bool canom, std::mutex& mutex, std::set<uint32_t>& m_set, hll::HyperLogLog& loglog)
{
  for (std::size_t i=0; i<s.size()-kmer_size+1; i++)
  {
    kmercpp::Kmer<K> k; k.set_from_str(s.substr(i, kmer_size));
    if (canok)
    {
      kmercpp::Kmer<K> kr = k.rev_comp();
      if (kr < k)
      {
        k = std::move(kr);
      }
    }

    uint32_t minim = get_minimizer(k, m_size, canom);
    {
      std::unique_lock<std::mutex> lock(mutex);
      nkmer++;
      m_set.insert(minim);
      loglog.add(reinterpret_cast<const char*>(k.get_data8()), k.get_size_bits() / 8);
    }
  }
}

}