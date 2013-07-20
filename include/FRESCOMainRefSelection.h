#ifndef FRESCOMAINREFSELECTION_H_INCLUDED
#define FRESCOMAINREFSELECTION_H_INCLUDED

class FRESCOMainRefSelection
{
public:


    void select(ConfigFileParser &cfp, string inpath, string  outpath)
    {
        ifstream inf(inpath+"compressedlist.txt");

        vector<string> compressedStringsPaths;

        string refpath;

        //read compressed files list
        getline(inf,refpath);
        cout<<"REF="<<refpath<<"\n";
        string s;

        while (getline(inf,s))
        {
            if (s.length()>1)
            {
                compressedStringsPaths.push_back(s);
            }
        }

        SystemTools::renewDir(outpath);

        ReferenceSelection rs;
        string result=rs.runOnPath(cfp, inpath+"compressedlist.txt");


        AbstractIndex *index=new NoIndex();
        index->initialize(refpath);

        AbstractSerializer *serializer=SerializerFactory::getSerializer(cfp.entries["SERIALIZERTYPE"]);
        AbstractCompressor *compressor=CompressorFactory::getCompressor(cfp.entries["COMPRESSORTYPE"]);

        HighLevel::decompressFile(index, compressor, serializer, result, outpath+"newref.txt");

        delete compressor;
        delete index;
        delete serializer;

        cout<<"REFSEL result:"<<result<<"\n";

        //recompress the compressed files w.r.t. the new reference
        HighLevel::convertCompression(cfp, compressedStringsPaths, refpath,outpath+"newref.txt", outpath);

        //create new list of compressed files
        ofstream ofs(outpath+"compressedlist.txt");
        ofs<<outpath+"newref.txt"<<"\n";
        BOOST_FOREACH(string s, compressedStringsPaths)
        {
            string fn=SystemTools::getFileName(s);
            ofs<<outpath<<fn<<"\n";
        }
        ofs.close();

    }
};

#endif // FRESCOMAINREFSELECTION_H_INCLUDED
