# mini-miner

```bash
mkdir build && cd build && cmake .. && make -j8
```

```
mini-miner 0.0.1

DESCRIPTION
  

USAGE
  mini-miner -i/--input <FILE> [-o/--output <FILE>] [-k/--kmer-size <INT>] [-m/--minim-size <INT>] 
             [-t/--threads <INT>] [--canonical-kmer] [--canonical-minimizer] [--dump] 
             [-h/--help] [-v/--verbose] [-d/--debug] [--version] 

OPTIONS
  [global] - global parameters
    -i --input               - input file 
    -o --output              - output file {minimizers.txt}
    -k --kmer-size           - k-mer size {32}
    -m --minim-size          - minimizer size {10}
       --canonical-kmer      -  [⚑]
       --canonical-minimizer -  [⚑]
       --dump                - dump all minimizer at --output [⚑]
    -t --threads             - nb of threads {8}

  [common]
    -h --help    - Show this message and exit. [⚑]
    -v --verbose - Verbose mode. [⚑]
    -d --debug   - Debug mode. [⚑]
       --version - Show version and exit. [⚑]
```