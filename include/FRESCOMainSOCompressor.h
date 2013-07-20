#ifndef FRESCOMAINSOCOMPRESSOR_H_INCLUDED
#define FRESCOMAINSOCOMPRESSOR_H_INCLUDED

// second order referential match entry (including an id for a reference sequence)
class SORME
{
public:

    SORME(long _refseqid, long _pos, long _length, char _mismatchchar, long _mismatchcount)
    {
        refseqid=_refseqid;
        pos=_pos;
        length=_length;
        mismatchchar=_mismatchchar;
        mismatchcount=_mismatchcount;
    }

    string toString()
    {
        return ""+boost::lexical_cast<string>(refseqid)+"|"+boost::lexical_cast<string>(pos)+"|"+boost::lexical_cast<string>(length)+"|"+boost::lexical_cast<string>(mismatchchar)+"|"+boost::lexical_cast<string>(mismatchcount)+"";
    }

    long refseqid;
    long pos;
    long length;
    char mismatchchar;
    long mismatchcount;
};


//class for creating a compressed representation of a set of SORME
class SORMESSerializer
{
public:

    SORMESSerializer()
    {
        zippedout=new boost::iostreams::filtering_ostream();

    }

    ~SORMESSerializer()
    {
        delete zippedout;
    }

    void serialize(vector<SORME> &sormes, string outputPath)
    {
        ofstream myFile;
        myFile.open (outputPath.c_str(), ios::out | ios::binary);

        zippedout->push(boost::iostreams::gzip_compressor(boost::iostreams::zlib::huffman_only,1000000));
        zippedout->push(myFile);

        unsigned char sepbyte=255;
        expectedrefpos=0;

        long l=0;
        while (l<(long)sormes.size())
        {
            while (l<(long)sormes.size() and sormes[l].refseqid<0)
            {
                serializeRealRME(sormes[l]);
                l++;
            }

            zippedout->write((const char*)&sepbyte, 1);

            while (l<(long)sormes.size() and sormes[l].refseqid>=0)
            {
                serializeRealSORME(sormes[l]);
                l++;
            }
            zippedout->write((const char*)&sepbyte, 1);
        }


        zippedout->flush();
        zippedout->reset();
        myFile.close();

    }

    char getLeftOverChar(char mismatchchar)
    {
        char leftoverchar=0;

        if (mismatchchar==0)
        {
            leftoverchar=0;
        }else{
            if (mismatchchar=='A') leftoverchar=1;
            else if (mismatchchar=='C') leftoverchar=2;
            else if (mismatchchar=='G') leftoverchar=3;
            else if (mismatchchar=='T') leftoverchar=4;
            else if (mismatchchar=='N') leftoverchar=5;
            else {
                    leftoverchar=-1;
                }
        }

        return leftoverchar;
    }


    //serialize a SORME that represents a RME
    void serializeRealRME(SORME &sorme)
    {
        long refposdiff=sorme.pos-expectedrefpos;
        char leftoverchar=getLeftOverChar(sorme.mismatchchar);

        if(refposdiff==0 and sorme.length<32 and leftoverchar>=1 and leftoverchar<=4 and sorme.mismatchcount==1)
        {
            unsigned char bytes[4];
            bytes[0]=0+(sorme.length<<2);
            bytes[0]+=(leftoverchar-1);

            zippedout->write((const char*)bytes, 1);

        }else if (refposdiff==0 and sorme.length<4090 and leftoverchar>=1 and leftoverchar<=4 and sorme.mismatchcount==1)
        {
            unsigned char bytes[2];
            bytes[0]=128+(sorme.length>>6);
            bytes[1]=(sorme.length%64)<<2;
            bytes[1]+=(leftoverchar-1);

            zippedout->write((const char*)bytes, 2);

        }else if (abs(refposdiff)<127 and sorme.length<65000 and leftoverchar>=1 and leftoverchar<=4 and sorme.mismatchcount==1)
        {
            unsigned char bytes[4];
            bytes[0]=128+64+(leftoverchar-1);
            bytes[1]=(unsigned char)(refposdiff);
            bytes[2]=(sorme.length>>8)%256;
            bytes[3]=(sorme.length%256);

            zippedout->write((const char*)bytes, 4);

        }else if (sorme.length<256 and leftoverchar>=1 and leftoverchar<=4 and sorme.mismatchcount==1)
        {
            unsigned char bytes[6];
            bytes[0]=128+64+32+(leftoverchar-1);
            bytes[1]=sorme.pos>>24;
            bytes[2]=(sorme.pos>>16)%256;
            bytes[3]=(sorme.pos>>8)%256;
            bytes[4]=sorme.pos%256;
            bytes[5]=sorme.length;

            zippedout->write((const char*)bytes, 6);

        }else if (sorme.length<65000 and leftoverchar>=1 and leftoverchar<=4 and sorme.mismatchcount==1)
        {
            unsigned char bytes[7];
            bytes[0]=128+64+32+16+(leftoverchar-1);
            bytes[1]=sorme.pos>>24;
            bytes[2]=(sorme.pos>>16)%256;
            bytes[3]=(sorme.pos>>8)%256;
            bytes[4]=sorme.pos%256;
            bytes[5]=(sorme.length>>8)%256;
            bytes[6]=sorme.length%256;

            zippedout->write((const char*)bytes, 7);
        }else
        {
            unsigned char bytes[12];
            unsigned char c=128+64+32+16+8;
            zippedout->write((const char*) &c, 1);
            zippedout->write((const char*) &sorme.mismatchchar, 1);


            bytes[0]=(sorme.pos>>24);
            bytes[1]=(sorme.pos>>16)%256;
            bytes[2]=(sorme.pos>>8)%256;
            bytes[3]=sorme.pos%256;

            bytes[4]=(sorme.length>>24);
            bytes[5]=(sorme.length>>16)%256;
            bytes[6]=(sorme.length>>8)%256;
            bytes[7]=sorme.length%256;

            bytes[8]=(sorme.mismatchcount>>24);
            bytes[9]=(sorme.mismatchcount>>16)%256;
            bytes[10]=(sorme.mismatchcount>>8)%256;
            bytes[11]=sorme.mismatchcount%256;

            zippedout->write((const char *)&bytes, 12);
        }

        expectedrefpos=sorme.pos+sorme.length+sorme.mismatchcount;
    }

    //serialize a real SORME (not RME)
    void serializeRealSORME(SORME &sorme)
    {
        if (sorme.refseqid<128 and sorme.pos<65500 and sorme.length<256)
        {
            unsigned char bytes[4];
            bytes[0]=sorme.refseqid;
            bytes[1]=(sorme.pos>>8)%256;
            bytes[2]=sorme.pos%256;
            bytes[3]=sorme.length;
            zippedout->write((const char *)&bytes, 4);
        }else{
            unsigned char bytes[10];
            bytes[0]=128;
            bytes[1]=sorme.refseqid;
            bytes[2]=(sorme.pos>>24);
            bytes[3]=(sorme.pos>>16)%256;
            bytes[4]=(sorme.pos>>8)%256;
            bytes[5]=sorme.pos%256;
            bytes[6]=(sorme.length>>24);
            bytes[7]=(sorme.length>>16)%256;
            bytes[8]=(sorme.length>>8)%256;
            bytes[9]=sorme.length%256;

            zippedout->write((const char *)&bytes, 10);
        }

    }



    long expectedrefpos;
    boost::iostreams::filtering_ostream *zippedout;

};

long getNextPrime(long inp)
{
    long cur=inp+1;
    bool prime=false;
    while (!prime)
    {
        prime=true;
        for (long l=2; l<sqrt(inp)+1;l++)
        {
            if (cur%l==0)
            {
                prime=false;
                break;
            }
        }

        if (!prime) cur++;
    }
    return cur;
}

long hashforcs1(ReferentialMatchEntry &rme, long mod)
{
    long result=0;
    result=result+rme.pos;
    result=result*(rme.length+11);
    result=result+rme.mismatchchar*40;

    return result%mod;
}

class RMEHashEntry
{
public:
    RMEHashEntry(ReferentialMatchEntry* _element)
    {
        element=_element;
    }

    ReferentialMatchEntry *element;
    int32_t occurrence;
};




class FOReference
{
public:
    FOReference(ConfigFileParser &cfp, short _id, string _path)
    {
        id=_id;
        path=_path;

        cout<<"Creating FORef "<<path<<"\n";

        rcdata=_loadsRMEs(cfp, path);

        long size=rcdata.size();
        hashtablesize=getNextPrime(size*5);
        for (long l=0;l<hashtablesize;l++)
        {
            hashtable.push_back(NULL);
        }

        for (long l=0;l<size;l++)
        {
            long h1=abs(hashforcs1(rcdata[l], hashtablesize));
            long h2=abs((((rcdata[l].length+100)%(hashtablesize-4))+1)%hashtablesize);

            if (h2==0) h2=7; //avoid a delta of 0 in double hashing

            long pos=abs(h1%hashtablesize);

            while (hashtable[pos]!=NULL)
            {
                pos=abs((pos+h2)%hashtablesize);
            }

            hashtable[pos]=new RMEHashEntry(&rcdata[l]);
            hashtable[pos]->occurrence=l;
        }
    }

    void getLongestInfixMatch(vector<ReferentialMatchEntry> &inputrcdata, long inputposstart, long &pos, long &length)
    {
        long h1=abs(hashforcs1(inputrcdata[inputposstart], hashtablesize));
        long h2=abs((((inputrcdata[inputposstart].length+100)%(hashtablesize-4))+1)%hashtablesize);

        if (h2==0) h2=7;

        long curpos=abs(h1%hashtablesize);
        bool found=false;

        long counter=0;

        while (hashtable[curpos]!=NULL and !found)
        {
            RMEHashEntry* cur=hashtable[curpos];

            if (*cur->element==inputrcdata[inputposstart])
            {

                long l=cur->occurrence;
                {
                    long matchlen=getRealMatchLength(inputrcdata, inputposstart, l);
                    if (matchlen>length)
                    {
                        length=matchlen;
                        pos=l;
                    }
                }

                found=true;
            }
            curpos=abs((curpos+h2)%hashtablesize);
            counter++;
        }
    }


    short id;
    string path;

    vector<RMEHashEntry*>hashtable;
    long hashtablesize;
    vector<ReferentialMatchEntry> rcdata;

private:

    long getRealMatchLength(vector<ReferentialMatchEntry> &inputrcdata, long inputposstart, long refpos)
    {
        long refposold=refpos;

        while (inputposstart<(long)inputrcdata.size() and refpos<(long)rcdata.size() and (inputrcdata[inputposstart])==(rcdata[refpos]))
        {
            inputposstart++; refpos++;
        }

        return refpos-refposold;
    }
};

class FRESCOMainSOCompressor
{
public:

    void compress(ConfigFileParser &cfp, string inpath, string outpath, long numberofrefs)
    {
        SystemTools::renewDir(outpath);

        ifstream inf(inpath+"compressedlist.txt");

        vector<string> files;

        string refpathfull;
        getline(inf,refpathfull);

        string refpathfullfn=SystemTools::getFileName(refpathfull);
        system((((string)"cp ")+refpathfull+" "+outpath+refpathfullfn).c_str());


        string s;
        while (getline(inf,s))
        {
            if (s.length()>1)
            {
                cout<<s<<"\n";
                files.push_back(s);
            }
        }

        vector<FOReference*> forefs;
        set<string> forefpaths;

        ofstream ofs(outpath+"compressedlist.txt");
        ofs<<outpath+refpathfullfn<<"\n";

        ofs<<numberofrefs<<"\n";

        if (numberofrefs>=(long)files.size())
        {
            cout<<"SO: equal or more references than files. Stop!\n";
            exit(0);
        }


        for (long l=0; l<numberofrefs; l++)
        {
            string curref=files[rand()%files.size()];
            while (curref.compare(refpathfull)==0 or forefpaths.find(curref)!=forefpaths.end())
            {
                curref=files[abs(rand()%files.size())];
            }

            FOReference *newforef=new FOReference(cfp, l, curref);
            forefs.push_back(newforef);
            forefpaths.insert(curref);

            string curreffn=SystemTools::getFileName(curref);
            system((((string)"cp ")+curref+" "+outpath+curreffn).c_str());
            ofs<<outpath+curreffn<<"\n";
        }

        cout<<"We have "<<forefs.size()<<" references\n";

        BOOST_FOREACH(string p, files)
        {
            cout<<p<<":";
            string fn=SystemTools::getFileName(p);
            if (forefpaths.find(p)!=forefpaths.end())
            {
                cout<<"1O-ref\n";
            }else
            {
                cout<<"to-be-compressed\n";
                ofs<<outpath+fn+".sofresco"<<"\n";

                vector<ReferentialMatchEntry> inputrcdata=_loadsRMEs(cfp, p);

                long inputlength=inputrcdata.size();
                long inputpos=0;

                long lookupcounter=0;
                long sormecounter=0;

                long ONEBYONECOMPRESSION=true;
                long RANDOMSTRINGLENGTH=3;

                vector<SORME> sormes;

                while (inputpos<inputlength)
                {
                    long newrefseqid=0, newrefpos=0, newreflength=0;
                    newreflength=0;
                    lookupcounter++;

                    StopWatch swt;

                    step(forefs, inputrcdata, inputpos, newrefseqid, newrefpos, newreflength);

                    swt.stop(false);

                    if (ONEBYONECOMPRESSION and newreflength<RANDOMSTRINGLENGTH)
                    {
                        newrefpos=0;
                        newreflength=0;
                    }

                    if (newreflength>0)
                    {
                        SORME sorme(newrefseqid,newrefpos,newreflength,0,0);
                        sormes.push_back(sorme);

                        inputpos+=newreflength;
                    }else{
                        SORME sorme(-1,inputrcdata[inputpos+newreflength].pos,inputrcdata[inputpos+newreflength].length,inputrcdata[inputpos+newreflength].mismatchchar,inputrcdata[inputpos+newreflength].mismatchcount);
                        sormes.push_back(sorme);
                        inputpos+=1;
                    }

                    sormecounter++;
                }

                SORMESSerializer ser;
                ser.serialize(sormes, outpath+fn+".sofresco");
            }
        }

        ofs.close();

        BOOST_FOREACH(FOReference *foref, forefs)
        {
            delete foref;
        }
    }


private:
    void step(vector<FOReference*> &forefs, vector<ReferentialMatchEntry> &inputrcdata, long inputpos, long &newrefseqid, long &newrefpos, long &newreflength)
    {
        for (long refseqid=0;refseqid<(long)forefs.size();refseqid++)
        {

            long locnewrefpos=0, locnewreflength=0;
            forefs[refseqid]->getLongestInfixMatch(inputrcdata, inputpos, locnewrefpos, locnewreflength);

            if (locnewreflength>newreflength)
            {
                newreflength=locnewreflength;
                newrefpos=locnewrefpos;
                newrefseqid=refseqid;
            }
        }
        if ((inputpos+newreflength) == (long)inputrcdata.size())
            newreflength--;
    }
};

#endif // FRESCOMAINSOCOMPRESSOR_H_INCLUDED
