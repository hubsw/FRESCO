#ifndef COMPRESSEDSEQUENCE_H_INCLUDED
#define COMPRESSEDSEQUENCE_H_INCLUDED

class RCSMMI
{
public:

    RCSMMI(vector<ReferentialMatchEntry> _rmes, bool decompress, AbstractIndex *refindex=NULL)
    {
        rmes.insert(rmes.end(), _rmes.begin(), _rmes.end());
        createRMEPositions();

        if (decompress and refindex!=NULL)
        {
            AbstractCompressor *compressor=CompressorFactory::getCompressor("SimpleCompressor_LO_MD");
            AbstractSequence* myseqout=new RawSequence();
            compressor->decompress(rmes,refindex,myseqout);
            raw=myseqout->data;
            delete myseqout;
            delete compressor;
        }
    }

    void createRMEPositions()
    {
        long pos=0;
        for (long i=0;i<(long)rmes.size();i++)
        {
            rmepositions[i]=pos;
            pos+=rmes[i].getTotalLength();
        }
    }

    vector<ReferentialMatchEntry> rmes;
    string raw;
    map<int, long> rmepositions;
};


class ReferentiallyCompressedSequence
{
public:

    typedef int size_type;

    ReferentiallyCompressedSequence(string path)
    {
        if (path.length()>0)
        {
            compressedLength=SystemTools::getGZipFileSize(path);

            ifstream myFile(path.c_str());
            boost::iostreams::filtering_istream unzippedin;
            unzippedin.push(boost::iostreams::gzip_decompressor());
            unzippedin.push(myFile);

            data=new unsigned char[compressedLength+1];
            unzippedin.read((char*)data, compressedLength);
        }

    }

    RCSMMI* getRCSMMI(bool decompress, AbstractIndex *_refindex=NULL)
    {
        vector<ReferentialMatchEntry> content;

        AbstractSerializer *serializer=SerializerFactory::getSerializer("CompactSerializer");
        serializer->unserializeFromMainMemory((unsigned char*)data, compressedLength,content);
        delete serializer;

        return new RCSMMI(content, decompress, _refindex);

    }

    long compressedLength;
    unsigned char* data;
};

class ReferentiallyCompressedSequenceWithRMEs : ReferentiallyCompressedSequence
{
public:

    typedef int size_type;

    ReferentiallyCompressedSequenceWithRMEs(string path): ReferentiallyCompressedSequence(path)
    {
    }

    ReferentiallyCompressedSequenceWithRMEs(vector<ReferentialMatchEntry> _rcdata): ReferentiallyCompressedSequence("")
    {
        rcdata=_rcdata;
    }

    void prepareRMes(ConfigFileParser &cfp)
    {
        AbstractSerializer *serializer=SerializerFactory::getSerializer(cfp.entries["SERIALIZERTYPE"]);
        serializer->unserializeFromMainMemory((unsigned char*)data, compressedLength,rcdata);
        delete serializer;
    }


    vector<ReferentialMatchEntry> rcdata;
};



#endif // COMPRESSEDSEQUENCE_H_INCLUDED
