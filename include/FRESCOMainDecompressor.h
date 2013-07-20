#ifndef FRESCOMAINDECOMPRESSOR_H_INCLUDED
#define FRESCOMAINDECOMPRESSOR_H_INCLUDED



//check equality of two sequences
bool isEqual(string original, string decompressed)
{
    long result=false;
    char *t1=SystemTools::loadFromText(original);
    char *t2=SystemTools::loadFromText(decompressed);
    long pos=0;

    while (t1[pos]==t2[pos] and t1[pos]!=0 and t2[pos]!=0)
    {
        pos++;
    }

    if (t1[pos]==t2[pos])
    {
        result=true;
    }else
    {
        cout<<"problem@"<<pos<<"\n";
    }

    delete t1;
    delete t2;

    return result;
}

//check equality of original path and decompressed path
void checkEquality(string origpath, string decomppath)
{
    std::vector<string> files;
    getAllFilesInDirectory(decomppath, files);

    cout<<"Checking equality ...\n";

    BOOST_FOREACH(string path, files)
    {
        string filename=SystemTools::getFileName(path);


        if (isEqual(origpath+filename, path))
        {
            cout<<"FN:"<<filename<<"\n";
            cout<<" -> Equal!\n";
        }else
        {
            cout<<"FN:"<<filename<<"\n";
            cout<<" -> Original and decompressed file are not Equal! Exit!\n";
            exit(0);
        }
    }
}


class FRESCOMainDecompressor
{
public:

    void decompress(ConfigFileParser &cfp, string comprpath, string  outpath)
    {
        SystemTools::renewDir(outpath);

        string comprfilepath=comprpath+"compressedlist.txt";

        ifstream inf(comprfilepath.c_str());

        string refpathfull;
        getline(inf,refpathfull);

        cout<<"REF="<<refpathfull<<"\n";

        long reflen=0;
        char* ref=SystemTools::loadFromText(refpathfull, reflen);
        string reference=ref;
        delete []ref;


        AbstractIndex *index=new SequenceOnlyIndex(reference);

        //Initiliaze compressor
        AbstractCompressor *compressor=CompressorFactory::getCompressor(cfp.entries["COMPRESSORTYPE"]);

        //Referential compression serializer
        AbstractSerializer *serializer=SerializerFactory::getSerializer(cfp.entries["SERIALIZERTYPE"]);
        string s;
        while (getline(inf,s))
        {
            if (s.length()>1)
            {
                cout<<s<<"\n";
                string fn=SystemTools::getFileName(s);
                fn=fn.substr(0,fn.size()-7); // Remove .fresco
                HighLevel::decompressFile(index, compressor, serializer, s, outpath+fn);
            }
        }

        delete index;
        delete serializer;
        delete compressor;

    }

};



#endif // FRESCOMAINDECOMPRESSOR_H_INCLUDED
