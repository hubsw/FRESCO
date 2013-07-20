FRESCO
======

A framework for referential sequence compression


1) About

Sequence compression is considered a key technology in the future of bioinformatics. Similarity between biological sequences can be exploited using so-called referential compression schemes, which encode the differences of an input sequence with respect to a reference sequence.
FRESCO, a Framework for REferential Sequence COmpression, is designed in a modular
way, which makes it easy to replace parts of the compression algorithm, e.g. index structures, with different implementations. As a community-maintained, open source referential compression framework, FRESCO tries to support the ongoing trend of using compression in bioinfomatics.

2) How to compile/install

In order to compile FESCO you need to run make. The makefile is provided in the /build directory of FRESCO. An example session of compiling and linking FRESCO is as follows:

cd /build

make clean

make

The second command will remove previous intermediate files (if existing). The third command will create the FRESCO executable in the FRESCO main directory. This executable is standalone. If you want to you FRESCO in other directories than the FRESCO root directory, then you need to set the PATH envorinment entryaccordingly (please refer to the manual of your shell/linux system).

You need the following external libraries in order to compile FRESCO:

boost_system (1.51 or later)

boost_filesystem

boost_iostreams

boost_thread-mt

libz

3) How to configure FRESCO

All configuration parameters are stored in a configuration file, whose path is passed as a parameter to the FESCO executable. The configuration file adopts the structure of ini-files from windows, i.e. each line is of the shape "key=value". The order of lines is not important.
At the moment, three keys are supported for FRESCO:

1) NUMBEROFADSOREFS: 
- fixed numebr of additional references for second-order compression.

2) COMPRESSORTYPE:  
- SimpleCompressor: A naive referential compression algorithm.
- SimpleCompressor_LO: SimpleCompressor plus local optimization (looking for local matches, in order to avoid costly index access)
- SimpleCompressor_LO_MD: SimpleCompressor_LO plus match distance heuristics, which prefers close short matches of far away longer matches

3) SERIALIZERTYPE:
- PlainSerializer: Serializes referential match entries in plain text
- PlainDeltaSerializer: Serializes referential match entries in plain text with positions delta encoded to the previous referential match entry
- CompactSerializer: binary encoding of referential match entries (most compact representation right now) 


4) How to run

The usage of FRESCO is as follows:


FRESCO config task inputpath outputpath

where:
- config is a path to the config file, e.g. /home/test/config.ini
- task is either COMPRESS, DECOMPRESS, REFSEL, REFREW, SOCOMPRESS, or SODECOMPRESS
- input is 
  - a path to the to-be-compressed file (in case of COMPRESS)
  - a path to the referentially compressed files (in case of DECOMPRESS, REFSEL, REFREW, and SOCOMPRESS)
  - a path to the SO compressed files (in case of SODECOMPRESS)
- output is
  - a path to the referentially compressed output files (in case of COMPRESS, REFSEL, and REFREW)
  - a path to the SO compressed output files (in case of SOCOMPRESS)
  - a path to the uncompressed outputfile (in case of DECOMPRESS and SODECOMPRESS)
  
Examples:

FRESCO /home/test/config.ini COMRESS /bio/uncompressed/ /bio/compressed/ 
FRESCO /home/test/config.ini DECOMRESS /bio/compressed/ /bio/decompressed/ 

Please note that when you execute task COMPRESS, the inputpath needs to contain a file "list.txt" which contains one line for each file you want to compress. See dataset ToyFRESCO for a simple example with for files.

5) How to change the code

In order to create a new instance of a referential compression algorithm, you need to create instantiations of the following classes:

AbstractIndex
AbstractSequence
AbstractCompressor
AbstractSerializer

In the following, we describe each of these interfaces and their standard implementations in FRESCO in detail.

The AbstractSequence interface defines two functions: one for loading a sequence from a file and and another one for writing a sequence to a file. FRESCO provides implementations for handling raw-files (one byte per symbol) and FASTA files.

An AbstractIndex is used for looking up matches of the to-be-compressed sequence with respect to the reference. The index is initialized from a given reference sequence, e.g. loaded from a FASTA file. The interface declares a function for looking up the longest prefix match of an input string with respect to the indexed reference. In FRESCO, we provide a standard implementation based on a k-mer hash index, KMERINdex, i.e. for each k-mer we store all occurrences in the reference sequence. Once a match for a partial sequence is needed, the k-mer prefix of the partial sequence is used to find the longest match in the reference sequence. Other implementations could use suffix trees.

The AbstractCompressor defines two functions: one for compressing a sequence into a list of referential match entries and another one for decompressing referential match entries back to a sequence. The compression interface makes use of the index implementation to find matches. FRESCO provides three compression algorithms: 1) a greedy (SimpeCompressor), which finds always the longest possible match, 2) an optimization for finding local matches without expensive index lookups (SimpeCompressor_LO), and 3) an optimization which prefers short, but local matches over longer matches far away from the previous match (SimpeCompressor_LO_MD).

The AbstractSerializer (un)serializes a list of referential matches to/from a file. FRESCO has three standard implementations: 1) plain ASCII format (PlainSerializer), 2) plain encoding with positions relatively encoded to previous matches (PlainDeltaSerializer), and 3) compact binary encoding (CompactSerializer).


