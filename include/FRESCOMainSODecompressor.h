#ifndef FRESCOMAINSODECOMPRESSOR_H_INCLUDED
#define FRESCOMAINSODECOMPRESSOR_H_INCLUDED

class SORMESDeserializer
{
public:


    void deserialize(string path, vector<SORME> &v)
    {
        ifstream myFile(path.c_str());
        boost::iostreams::filtering_istream unzippedin;
        unzippedin.push(boost::iostreams::gzip_decompressor());
        unzippedin.push(myFile);

        v.clear();

        unsigned char b=255;


        unzippedin.read((char *)&b, 1);

        long rmecount=0;
        long sormecount=0;

        long expectedrefpos=0;

        while (((b>>7)==0) or !unzippedin.eof())
        {
            while (((b>>7)==0) or (b!=255 and !unzippedin.eof()))
            {
                long length=0;
                long pos=0;
                char loc=0;
                unsigned char mmatch;

                long mismatchcount=1;

                if ((b>>7)==0)
                {
                    pos=expectedrefpos;

                    length=(b>>2)%32;
                    loc=(b%4)+1;
                    mmatch=convertLeftOverChar(loc);
                }else if ((b>>6)==2)
                {
                    unsigned char b2;
                    unzippedin.read((char*)&b2, 1);

                    pos=expectedrefpos;
                    length=(((long)(b%64))<<6)+(b2>>2);
                    loc=(b2%4)+1;
                    mmatch=convertLeftOverChar(loc);

                }else if ((b>>5) ==6)
                {
                    unsigned char b2, b3, b4;
                    unzippedin.read((char*)&b2, 1);
                    unzippedin.read((char*)&b3, 1);
                    unzippedin.read((char*)&b4, 1);

                    pos=expectedrefpos+(char)b2;
                    length=((long)b3)*256+((long)b4);
                    loc=(b%4)+1;
                    mmatch=convertLeftOverChar(loc);

                }else if ((b>>4) ==14)
                {
                    unsigned char b2, b3, b4, b5, b6;
                    unzippedin.read((char*)&b2, 1);
                    unzippedin.read((char*)&b3, 1);
                    unzippedin.read((char*)&b4, 1);
                    unzippedin.read((char*)&b5, 1);
                    unzippedin.read((char*)&b6, 1);

                    pos=((long)b2)*256*256*256+((long)b3)*256*256+((long)b4)*256+((long)b5);
                    length=b6;
                    loc=(b%4)+1;
                    mmatch=convertLeftOverChar(loc);

                }else if ((b>>3) ==30)
                {
                    unsigned char b2, b3, b4, b5, b6, b7;
                    unzippedin.read((char*)&b2, 1);
                    unzippedin.read((char*)&b3, 1);
                    unzippedin.read((char*)&b4, 1);
                    unzippedin.read((char*)&b5, 1);
                    unzippedin.read((char*)&b6, 1);
                    unzippedin.read((char*)&b7, 1);

                    pos=((long)b2)*256*256*256+((long)b3)*256*256+((long)b4)*256+((long)b5);
                    length=((long)b6)*256+((long)b7);
                    loc=(b%4)+1;
                    mmatch=convertLeftOverChar(loc);


                }else if ((b>>3) ==31)
                {
                    unsigned char b2, b3, b4, b5, b6, b7, b8, b9,b10,b11,b12,b13;


                    unzippedin.read((char*)&mmatch, 1);

                    unzippedin.read((char*)&b2, 1);
                    unzippedin.read((char*)&b3, 1);
                    unzippedin.read((char*)&b4, 1);
                    unzippedin.read((char*)&b5, 1);
                    unzippedin.read((char*)&b6, 1);
                    unzippedin.read((char*)&b7, 1);
                    unzippedin.read((char*)&b8, 1);
                    unzippedin.read((char*)&b9, 1);
                    unzippedin.read((char*)&b10, 1);
                    unzippedin.read((char*)&b11, 1);
                    unzippedin.read((char*)&b12, 1);
                    unzippedin.read((char*)&b13, 1);


                    pos=((long)b2)*256*256*256+((long)b3)*256*256+((long)b4)*256+((long)b5);
                    length=((long)b6)*256*256*256+((long)b7)*256*256+((long)b8)*256+((long)b9);
                    mismatchcount=((long)b10)*256*256*256+((long)b11)*256*256+((long)b12)*256+((long)b13);

                    loc=b%8;
                }
                SORME sorme(-1, pos, length, mmatch, mismatchcount);
                v.push_back(sorme);

                expectedrefpos=pos+length+mismatchcount;

                rmecount++;
                if (!unzippedin.eof())
                    unzippedin.read((char *)&b, 1);
            }

            b=255;
            if (!unzippedin.eof())
                unzippedin.read((char *)&b, 1);

            while (b!=255 and !unzippedin.eof())
            {
                if (b<128)
                {
                    unsigned char b2, b3, b4;

                    unzippedin.read((char*)&b2, 1);
                    unzippedin.read((char*)&b3, 1);
                    unzippedin.read((char*)&b4, 1);


                    SORME sorme(b, ((long)b2)*256+b3, b4, 0,0);
                    v.push_back(sorme);

                }else
                {
                    unsigned char bX, b2, b3, b4, b5, b6, b7, b8, b9;
                    unzippedin.read((char*)&bX, 1);
                    unzippedin.read((char*)&b2, 1);
                    unzippedin.read((char*)&b3, 1);
                    unzippedin.read((char*)&b4, 1);
                    unzippedin.read((char*)&b5, 1);
                    unzippedin.read((char*)&b6, 1);
                    unzippedin.read((char*)&b7, 1);
                    unzippedin.read((char*)&b8, 1);
                    unzippedin.read((char*)&b9, 1);


                    SORME sorme(bX, ((long)b2)*256*256*256+((long)b3)*256*256+((long)b4)*256+b5, ((long)b6)*256*256*256+((long)b7)*256*256+((long)b8)*256+b9, 0,0);
                    v.push_back(sorme);

                }

                sormecount++;

                if (!unzippedin.eof())
                    unzippedin.read((char *)&b, 1);
            }

            if (!unzippedin.eof())
                unzippedin.read((char *)&b, 1);
        }

    }



    char convertLeftOverChar(char c)
    {
        if (c==0) return 0;
        else if (c==1) return 'A';
        else if (c==2) return 'C';
        else if (c==3) return 'G';
        else if (c==4) return 'T';
        else if (c==5) return 'N';
        else
        {
            cout<<"Unknown converter char ...\n";
            exit(-1);
        }
    }
};

class FRESCOMainSODecompressor
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


        string snumberofaddrefs;
        getline(inf,snumberofaddrefs);
        long numberofaddrefs=boost::lexical_cast<long>(snumberofaddrefs);

        cout<<"Additional references="<<numberofaddrefs<<"\n";

        AbstractIndex *index=new SequenceOnlyIndex(reference);

        AbstractCompressor *compressor=CompressorFactory::getCompressor(cfp.entries["COMPRESSORTYPE"]);
        AbstractSerializer *serializer=SerializerFactory::getSerializer(cfp.entries["SERIALIZERTYPE"]);

        vector<vector<ReferentialMatchEntry>> forefs;

        for (long l=0;l<numberofaddrefs;l++)
        {
            string line;
            getline(inf,line);

            cout<<" - "<<line<<"\n";

            string fn=SystemTools::getFileName(line);
            fn=fn.substr(0,fn.size()-7); // Remove .fresco
            HighLevel::decompressFile(index, compressor, serializer, line, outpath+fn);

            forefs.push_back(_loadsRMEs(cfp, line));

        }

        string s;
        while (getline(inf,s))
        {
            if (s.length()>1)
            {

                string fn=SystemTools::getFileName(s);
                fn=fn.substr(0,fn.size()-16); // Remove .fresco.sofresco

                decompressSOFile(forefs,index, compressor, serializer, s, outpath+fn);
            }
        }

        delete index;
        delete serializer;
        delete compressor;
    }


    void decompressSOFile(vector<vector<ReferentialMatchEntry>> &refs, AbstractIndex *index, AbstractCompressor *compressor, AbstractSerializer *serializer, string path, string outpath)
    {
        cout<<"SODECOMP:"<<outpath<<"\n";

        SORMESDeserializer des;
        vector<SORME> sormes;
        des.deserialize(path, sormes);

        vector<ReferentialMatchEntry> recoveredrmes;

        BOOST_FOREACH(SORME s, sormes)
        {
            if (s.refseqid<0)
            {
                ReferentialMatchEntry rme(s.pos, s.length, s.mismatchchar, s.mismatchcount);
                recoveredrmes.push_back(rme);
            }else{
                for (long l=0;l<s.length;l++)
                {
                    recoveredrmes.push_back((refs[s.refseqid])[s.pos+l]);
                }
            }
        }

        AbstractSequence* myseqout=new RawSequence();

        compressor->decompress(recoveredrmes,index, myseqout);
        string result=myseqout->data;
        delete myseqout;

        ofstream ofs(outpath);
        ofs<<result;
        ofs.close();
    }

};
#endif // FRESCOMAINSODECOMPRESSOR_H_INCLUDED
