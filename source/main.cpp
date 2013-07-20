#include "../include/BaseIncludes.h"
#include "../include/SystemTools.h"
#include "../include/Sequence/AbstractSequence.h"
#include "../include/Sequence/RawSequence.h"
#include "../include/Index/AbstractIndex.h"
#include "../include/Index/KMERIndex.h"
#include "../include/Index/NoIndex.h"
#include "../include/Index/SequenceOnlyIndex.h"
#include "../include/Compressor/AbstractCompressor.h"
#include "../include/Compressor/SimpleCompressor_LO_MD.h"
#include "../include/Compressor/SimpleCompressor_LO.h"
#include "../include/Compressor/SimpleCompressor.h"
#include "../include/Compressor/CompressorFactory.h"
#include "../include/Serializer/AbstractSerializer.h"
#include "../include/Serializer/PlainSerializer.h"
#include "../include/Serializer/PlainDeltaSerializer.h"
#include "../include/Serializer/CompactSerializer.h"
#include "../include/Serializer/SerializerFactory.h"
#include "../include/Config/ConfigFileParser.h"
#include "../include/HighLevel/HighLevel.h"
#include "../include/FileLoader.h"
#include "../include/FRESCOMainCompressor.h"
#include "../include/FRESCOMainDecompressor.h"
#include "../include/FRESCOMainRefSelection.h"
#include "../include/FRESCOMainRefRewriting.h"
#include "../include/FRESCOMainSOCompressor.h"
#include "../include/FRESCOMainSODecompressor.h"

void compress(ConfigFileParser &cfp, string inpath, string outpath)
{

    FRESCOMainCompressor mc;
    mc.compress(cfp, inpath, outpath);
}

void decompress(ConfigFileParser &cfp, string inpath, string outpath)
{
    FRESCOMainDecompressor md;
    md.decompress(cfp, inpath, outpath);
}

void refRewriting(ConfigFileParser &cfp, string inpath, string outpath)
{
    FRESCOMainRefRewriting mrr;
    mrr.rewrite(cfp, inpath, outpath);
}

void refSelection(ConfigFileParser &cfp, string inpath, string outpath)
{
    FRESCOMainRefSelection mrs;
    mrs.select(cfp, inpath, outpath);
}

void socompress(ConfigFileParser &cfp, string inpath, string outpath, long numberofaddref)
{
    FRESCOMainSOCompressor socomp;
    socomp.compress(cfp,inpath,outpath,numberofaddref);
}

void sodecompress(ConfigFileParser &cfp,string inpath, string outpath)
{
    FRESCOMainSODecompressor sodecomp;
    sodecomp.decompress(cfp, inpath, outpath);
}

void printUsage()
{
    cout<<"Usage:\n\n";
    cout<<"FRESCO config task inputpath outputpath\n\n";
    cout<<"where:\n";
    cout<<"- config is a path to the config file, e.g. /home/test/config.ini\n";
    cout<<"- task is either COMPRESS, DECOMPRESS, REFSEL, REFREW, SOCOMPRESS, or SODECOMPRESS\n";
    cout<<"- input is \n";
    cout<<"  - a path to the to-be-compressed file (in case of COMPRESS)\n";
    cout<<"  - a path to the referentially compressed files (in case of DECOMPRESS, REFSEL, REFREW, and SOCOMPRESS)\n";
    cout<<"  - a path to the SO compressed files (in case of SODECOMPRESS)\n";
    cout<<"- output is\n";
    cout<<"  - a path to the referentially compressed output files (in case of COMPRESS, REFSEL, and REFREW)\n";
    cout<<"  - a path to the SO compressed output files (in case of SOCOMPRESS)\n";
    cout<<"  - a path to the uncompressed outputfile (in case of DECOMPRESS and SODECOMPRESS)\n";
    cout<<"  \n";
    cout<<"Examples:\n\n";
    cout<<"FRESCO /home/test/config.ini COMRESS /bio/uncompressed/ /bio/compressed/ \n";
    cout<<"FRESCO /home/test/config.ini DECOMRESS /bio/compressed/ /bio/decompressed/ \n";
    cout<<"  \n";
    cout<<"For further information please see README.txt!\n";
    exit(0);
}

int main(int argc, char *argv[])
{
    //check if the number of arguments is correct
    if (argc!=5)
    {
        printUsage();
        exit(0);
    }
    string configpath=argv[1];
    string command=argv[2];
    string inpath=argv[3];
    string outpath=argv[4];

    //append slash at the end of paths, if it does not exist
    if (inpath[inpath.size()-1]!='/') inpath=inpath+"/";
    if (outpath[outpath.size()-1]!='/') outpath=outpath+"/";

    //read configuration entries
    ConfigFileParser cfp(configpath);

    if (command=="COMPRESS")
    {
        compress(cfp, inpath, outpath);
    }else if (command=="DECOMPRESS")
    {
        decompress(cfp, inpath, outpath);
    }else if (command=="REFSEL")
    {
        refSelection(cfp, inpath, outpath);
    }else if (command=="REFREW")
    {
        refRewriting(cfp, inpath, outpath);
    }else if (command=="SOCOMPRESS")
    {
        socompress(cfp, inpath, outpath, boost::lexical_cast<long>(cfp.entries["NUMBEROFADSOREFS"]));
    }else if (command=="SODECOMPRESS")
    {
        sodecompress(cfp, inpath, outpath);
    }else
    {
        cout<<"Unkown command found: "<<command<<"\n";
        printUsage();
        exit(0);
    }

    return 0;
}


