#ifndef FRESCOMAINCODE_H_INCLUDED
#define FRESCOMAINCODE_H_INCLUDED

void printUsage()
{
    cout<<"Usage:\n\n";
    cout<<"FRESCO task config input index output\n\n";
    cout<<"where:\n";
    cout<<"- task is either COMPRESS or DECOMPRESS\n";
    cout<<"- config is a path to the config file\n";
    cout<<"- input is \n";
    cout<<"  - a path to the to-be-compressed file (in case of COMPRESS)\n";
    cout<<"  - a path to the compressed file (in case of DECOMPRESS)\n";
    cout<<"- index is a path to the reference file\n";
    cout<<"- output is\n";
    cout<<"  - a path to the compressed output file (in case of COMPRESS)\n";
    cout<<"  - a path to the uncompressed outputfile (in case of DECOMPRESS)\n";

    exit(0);
}



void frescoNormal(int argc, char *argv[])
{
    if (argc!=6) printUsage();
    string command=argv[1];
    string config=argv[2];
    string inputPath=argv[3];
    string indexRawPath=argv[4];
    string outPath=argv[5];


    ConfigFileParser cfp(config);

    //Initiliaze compressor
    AbstractCompressor *compressor=CompressorFactory::getCompressor(cfp.entries["COMPRESSORTYPE"]);

    //Referential compression serializer
    AbstractSerializer *serializer=SerializerFactory::getSerializer(cfp.entries["SERIALIZERTYPE"]);
    //AbstractSerializer *serializer=SerializerFactory::getSerializer("PlainSerializer");




    if (command=="COMPRESS")
    {
        cout<<"Indexing ...\n";
        AbstractIndex *index=new KMERIndex(string2long(cfp.entries["KMERLENGTH"]));
        index->initialize(indexRawPath);

        AbstractSequence* myseqin=new RawSequence();
        myseqin->load(inputPath);

        vector<ReferentialMatchEntry> rmes;
        StopWatch sw2;
        compressor->compress(myseqin,index,rmes);
        sw2.stop(false);

        cout<<"Compression speed (no I/O): "<<((double)myseqin->data.length()/(1024*1024)/(sw2.getDiff()/1000))<<" MB/s\n";
        serializer->serialize(rmes, outPath);

        delete index;
    }/*else if (command=="COMPRESSDIR")
    {
        //Actual compression
        string s;
        cout<<"Indexing ...\n";
        AbstractIndex *index=new KMERIndex(string2long(cfp.entries["KMERLENGTH"]));
        index->initialize(indexRawPath);

        MultiCompressionSystem mcs(index, compressor, serializer);

        long counter=1;
        long totalsize=0;

        std::vector<string> myfiles;
        getAllFilesInDirectory(inputPath, myfiles);
        BOOST_FOREACH(string s, myfiles)
        {
            //cout<<s<<"...\n";
            string fname=getFileName(s);
            mcs.addFile(s, outPath+fname+".fresco");
            totalsize+=getFileSize(s);
            counter++;
        }
        mcs.performCompression();

        delete index;


    }*/else if (command=="DECOMPRESS")
    {
        AbstractIndex *index=new SequenceOnlyIndex();
        index->initialize(indexRawPath);

        //Actual decompression
        HighLevel::decompressFile(index, compressor, serializer, inputPath, outPath);

        delete index;

    }else
    {
        cout<<"Unkown command: "<<command<<"\n";
        cout<<"Possible commands are: COMPRESS and DECOMPRESS\n";
        exit(0);
    }

    delete serializer;
    delete compressor;

}

#endif // FRESCOMAINCODE_H_INCLUDED
