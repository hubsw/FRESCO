#ifndef FRESCOMAINCOMPRESSOR_H_INCLUDED
#define FRESCOMAINCOMPRESSOR_H_INCLUDED

class FRESCOMainCompressor
{
public:
    void compress(ConfigFileParser &cfp, string inpath, string outpath)
    {
        SystemTools::renewDir(outpath);

        string infilepath=inpath+"list.txt";

        //load to-be-compressed files
        FileLoader fl(infilepath);

        string fn=SystemTools::getFileName(fl.reference);

        system(("cp "+fl.reference+" "+outpath+fn).c_str());

        //cout<<"Reference="<<fl.reference<<"\n";
        AbstractIndex *index=new KMERIndex();
        index->initialize(fl.reference);

        AbstractCompressor *compressor=CompressorFactory::getCompressor(cfp.entries["COMPRESSORTYPE"]);
        AbstractSerializer *serializer=SerializerFactory::getSerializer(cfp.entries["SERIALIZERTYPE"]);

        //create file for listing compressed files
        ofstream ofs(outpath+"compressedlist.txt");
        ofs<<(outpath+fn)<<"\n";

        //compress each file
        BOOST_FOREACH(string inputPath, fl.allpaths)
        {
            cout<<" -"<<inputPath<<"\n";

            StopWatch sw1;
            AbstractSequence* myseqin=new RawSequence();
            myseqin->load(inputPath);

            vector<ReferentialMatchEntry> rmes;
            StopWatch sw2;
            compressor->compress(myseqin,index,rmes);
            sw2.stop(false);
            sw1.stop(false);

            //pront statistics and ann comrpessed file to output list
            string filename=SystemTools::getFileName(inputPath);
            cout<<"Compression speed (with I/O): "<<((double)myseqin->data.length()/(1024*1024)/(sw1.getDiff()/1000))<<" MB/s\n";
            cout<<"Compression speed (no I/O): "<<((double)myseqin->data.length()/(1024*1024)/(sw2.getDiff()/1000))<<" MB/s\n";
            serializer->serialize(rmes, outpath+filename+".fresco");
            ofs<<outpath<<filename<<".fresco"<<"\n";

            delete myseqin;
        }

        delete serializer;
        delete compressor;
        delete index;
        ofs.close();

    }

};

#endif // FRESCOMAINCOMPRESSOR_H_INCLUDED
