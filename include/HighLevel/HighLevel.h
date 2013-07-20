#ifndef HIGHLEVEL_H_INCLUDED
#define HIGHLEVEL_H_INCLUDED

vector<ReferentialMatchEntry> _loadsRMEs(ConfigFileParser &cfp, string comprPath)
{

    //cout<<"LOADING:"<<comprPath<<"\n";
    AbstractSerializer *serializer=SerializerFactory::getSerializer(cfp.entries["SERIALIZERTYPE"]);
    vector<ReferentialMatchEntry> v;
    serializer->unserialize(comprPath,v);
    //vector<ReferentialMatchEntry> v(l.begin(), l.end());
    delete serializer;



    return v;
}



#include "HLReferenceRewriting.h"
#include "HLReferenceSelection.h"


#include "CompressedSequence.h"
#include "CompressedSequenceLoader.h"


class HighLevel
{
public:


    static vector<ReferentialMatchEntry> loadsRMEs(ConfigFileParser &cfp, string comprPath)
    {
        return _loadsRMEs(cfp, comprPath);
    }


    static string partiallyDecompress(vector<ReferentialMatchEntry> &rmes, long start, long length, char* ref, long ref_len)
    {
        long rmei=0;
        long curpos=0;

        while (curpos+rmes[rmei].getTotalLength()<start)
        {
            curpos+=rmes[rmei].getTotalLength();
            rmei++;

        }
        //long innerpos=start-curpos;


        string result="";

        long toAdd=length;

        long innerposstart=start-curpos;
        //cout<<innerposstart<<"@"<<rmei<<"/"<<rmes.size()<< " .... "<<rmes[rmei].toString()<<"\n";
        while (rmei<(long)rmes.size())
        {
            if (innerposstart<rmes[rmei].length)
            {
                for (long innerpos=innerposstart;innerpos<rmes[rmei].length;innerpos++)
                {
                    result+=ref[rmes[rmei].pos+innerpos];
                    toAdd--;
                    if (toAdd==0) return result;
                    if (innerposstart>0) innerposstart--;
                }
            }

            for (long t=innerposstart;t<rmes[rmei].mismatchcount;t++)
            {
                result+=rmes[rmei].mismatchchar;
                toAdd--;
                if (toAdd==0) return result;

            }

            rmei++;
            innerposstart=0;
        }

        return result;
    }

    /*char* getDecompressedMaxOverlap(vector<ReferentialMatchEntry> &rmes, long rmepos, long innerstart)
    {
        string overlap;

        for (long innerpos=innerstart;innerpos<rmes[rmepos].length;innerpos++)
        {
            overlap+=index->refcontent[rmes[rmepos].pos+innerpos];
        }

        for (long t=0;t<rmes[rmepos].mismatchcount;t++)
            overlap+=rmes[rmepos].mismatchchar;

        overlap+=tempDecompLeftover(rmes, rmepos+1, k-1);

        char* result=new char[overlap.length()+1];
        strcpy(result, overlap.c_str());
        return result;
    }*/


static void compressFile(AbstractIndex *i, AbstractCompressor* c, AbstractSerializer *s, string inputRawPath, string comprPath)
{

    //load to-be-compressed sequence
    StopWatch sw;
    AbstractSequence* myseqin=new RawSequence();
    myseqin->load(inputRawPath);

    //preform compression
    vector<ReferentialMatchEntry> rmes;

    StopWatch sw2;
    c->compress(myseqin,i,rmes);
    sw2.stop(false);
    sw.stop(false);

    cout<<"Compression speed: "<<((double)myseqin->data.length()/(1024*1024)/(sw.getDiff()/1000))<<" MB/s\n";
    cout<<"Compression speed (no IO/): "<<((double)myseqin->data.length()/(1024*1024)/(sw2.getDiff()/1000))<<" MB/s\n";

    //Serialize referential compression
    s->serialize(rmes, comprPath);
    delete myseqin;
}

static void compressFileRAW(string refpath, string inPath, string comprPath)
{
    AbstractIndex *index=new KMERIndex(24);
    index->initialize(refpath);

    AbstractSerializer *serializer=SerializerFactory::getSerializer("CompactSerializer");

    AbstractCompressor *compressor=CompressorFactory::getCompressor("SimpleCompressor_LO_MD");

    compressFile(index, compressor, serializer, inPath, comprPath);

    //AbstractSerializer *serializerplain=SerializerFactory::getSerializer("PlainSerializer");
    //compressFile(index, compressor, serializerplain, inPath, comprPath+".plain");


    delete compressor;
    delete index;
    delete serializer;
}


static void compressFileRAW2RMEs(AbstractIndex *index, string in, vector<ReferentialMatchEntry> &rmes)
{
    AbstractCompressor *compressor=CompressorFactory::getCompressor("SimpleCompressor_LO_MD");
    RawSequence* myseqin=new RawSequence();
    myseqin->loadFromString(in);
    compressor->compress(myseqin,index,rmes);
    delete compressor;
    delete index;
    delete myseqin;
}

static void decompressFile(AbstractIndex *i, AbstractCompressor* c, AbstractSerializer *s, string comprPath, string decomprPath)
{
    if (!SystemTools::existsFile(comprPath))
    {
        cout<<"HighLevel: File does not exist: "<<comprPath<<". Exit!\n";
        exit(0);
    }

    vector<ReferentialMatchEntry> rmes2;
    //unserialize referential match entries
    //cout<<"Unserializing ...\n";
    s->unserialize(comprPath, rmes2);
    AbstractSequence* myseqout=new RawSequence();

    //cout<<"Decompressing ...\n";
    StopWatch sw;
    StopWatch sw2;
    //decompress each referential match entry
    c->decompress(rmes2,i, myseqout);
    sw2.stop(false);

    //save decompressed sequence
    myseqout->save(decomprPath);
    sw.stop(false);

    cout<<"Decompression speed: "<<((double)myseqout->data.length()/(1024*1024)/(sw.getDiff()/1000))<<" MB/s\n";
    cout<<"Decompression speed (no I/O): "<<((double)myseqout->data.length()/(1024*1024)/(sw2.getDiff()/1000))<<" MB/s\n";

    delete myseqout;
}

static void decompressFileRAW(string refpath, string comprPath, string decomprPath)
{
    AbstractIndex *index=new NoIndex();
    index->initialize(refpath);

    AbstractSerializer *serializer=SerializerFactory::getSerializer("CompactSerializer");

    AbstractCompressor *compressor=CompressorFactory::getCompressor("SimpleCompressor_LO_MD");

    decompressFile(index, compressor, serializer, comprPath, decomprPath);

    delete compressor;
    delete index;
    delete serializer;
}

static string decompressFileIntoString(string refpath, string comprPath)
{
    AbstractIndex *index=new NoIndex();
    index->initialize(refpath);

    AbstractSerializer *serializer=SerializerFactory::getSerializer("CompactSerializer");

    AbstractCompressor *compressor=CompressorFactory::getCompressor("SimpleCompressor_LO_MD");

    if (!SystemTools::existsFile(comprPath))
    {
        cout<<"HighLevel: File does not exist: "<<comprPath<<". Exit!\n";
        exit(0);
    }

    vector<ReferentialMatchEntry> rmes2;
    //unserialize referential match entries
    //cout<<"Unserializing ...\n";
    serializer->unserialize(comprPath, rmes2);
    AbstractSequence* myseqout=new RawSequence();

    //cout<<"Decompressing ...\n";
    StopWatch sw;
    StopWatch sw2;
    //decompress each referential match entry
    compressor->decompress(rmes2,index, myseqout);
    sw2.stop(false);

    string result=myseqout->data;

    delete compressor;
    delete index;
    delete serializer;
    delete myseqout;


    return result;
}

static string decompressFileIntoString(AbstractIndex *index, string comprPath)
{
    AbstractSerializer *serializer=SerializerFactory::getSerializer("CompactSerializer");

    AbstractCompressor *compressor=CompressorFactory::getCompressor("SimpleCompressor_LO_MD");

    if (!SystemTools::existsFile(comprPath))
    {
        cout<<"HighLevel: File does not exist: "<<comprPath<<". Exit!\n";
        exit(0);
    }

    vector<ReferentialMatchEntry> rmes2;
    //unserialize referential match entries
    //cout<<"Unserializing ...\n";
    serializer->unserialize(comprPath, rmes2);
    AbstractSequence* myseqout=new RawSequence();

    //cout<<"Decompressing ...\n";
    StopWatch sw;
    StopWatch sw2;
    //decompress each referential match entry
    compressor->decompress(rmes2,index, myseqout);
    sw2.stop(false);

    string result=myseqout->data;

    delete compressor;
    delete index;
    delete serializer;
    delete myseqout;


    return result;
}

static void convertCompression(string config, string inputPath, string oldindexRawPath, string indexRawPath, string outPath)
{
    string command="COMPRESSCOMPRESSEDDIR";
    ConfigFileParser cfp(config);

    AbstractCompressor *compressor=CompressorFactory::getCompressor(cfp.entries["COMPRESSORTYPE"]);
    AbstractSerializer *serializer=SerializerFactory::getSerializer(cfp.entries["SERIALIZERTYPE"]);


    cout<<"Indexing ...\n";
    AbstractIndex *index=new KMERIndex(string2long(cfp.entries["KMERLENGTH"]));
    index->initialize(indexRawPath);

    cout<<"Indexing ...\n";
    AbstractIndex *oldindex=new KMERIndex(13);
    oldindex->initialize(oldindexRawPath);



    StopWatch sw;
    std::vector<string> myfiles;
    getAllFilesInDirectory(inputPath, myfiles);

    long l=0;

    BOOST_FOREACH(string s, myfiles)
    {
        cout<<l<<": "<<s<<"\n";
        vector<ReferentialMatchEntry> rmes2;
        serializer->unserialize(s, rmes2);
        AbstractSequence* myseqin=new RawSequence();

        StopWatch sw;
        compressor->decompress(rmes2,oldindex, myseqin);
        sw.stop(false);

        string fname=getFileName(s);
        string fdot=getFirstDotElement(fname);

        vector<ReferentialMatchEntry> rmes;

        compressor->compress(myseqin,index,rmes);
        serializer->serialize(rmes, outPath+fdot+".fresco");
        delete myseqin;
        l++;
    }

    sw.stop(false);

    cout<<"Processed "<<myfiles.size()<<" in "<<sw.getDiff()<<" ms.\n";

    delete index;
    delete oldindex;
    delete serializer;
    delete compressor;
}

static void convertCompression(ConfigFileParser &cfp, vector<string> inputPaths, string oldindexRawPath, string indexRawPath, string outpath)
{
    //SystemTools::renewDir(outpath);

    AbstractCompressor *compressor=CompressorFactory::getCompressor(cfp.entries["COMPRESSORTYPE"]);
    AbstractSerializer *serializer=SerializerFactory::getSerializer(cfp.entries["SERIALIZERTYPE"]);


    cout<<"Indexing ...\n";
    AbstractIndex *index=new KMERIndex();
    index->initialize(indexRawPath);

    cout<<"Indexing ...\n";
    //AbstractIndex *oldindex=new KMERIndex(13);
    //oldindex->initialize(oldindexRawPath);

    AbstractIndex *oldindex=new SequenceOnlyIndex();
    oldindex->initialize(oldindexRawPath);



    StopWatch sw;

    long l=0;

    BOOST_FOREACH(string s, inputPaths)
    {
        StopWatch sw1;
        cout<<l<<": "<<s<<"\n";
        vector<ReferentialMatchEntry> rmes2;
        serializer->unserialize(s, rmes2);
        AbstractSequence* myseqin=new RawSequence();

        StopWatch sw2;
        compressor->decompress(rmes2,oldindex, myseqin);
        sw.stop(false);

        string fname=getFileName(s);
        string fdot=getFirstDotElement(fname);

        vector<ReferentialMatchEntry> rmes;

        compressor->compress(myseqin,index,rmes);

        sw2.stop(false);

        string fn=SystemTools::getFileName(s);
        serializer->serialize(rmes, outpath+fn);

        sw1.stop(false);

        l++;

        cout<<"Recompression speed (with I/O): "<<((double)myseqin->data.length()/(1024*1024)/(sw1.getDiff()/1000))<<" MB/s\n";
        cout<<"Recompression speed (no I/O): "<<((double)myseqin->data.length()/(1024*1024)/(sw2.getDiff()/1000))<<" MB/s\n";

        delete myseqin;

    }

    sw.stop(false);

    cout<<"Processed "<<inputPaths.size()<<" in "<<sw.getDiff()<<" ms.\n";

    delete index;
    delete oldindex;
    delete serializer;
    delete compressor;
}




};

#endif // HIGHLEVEL_H_INCLUDED
