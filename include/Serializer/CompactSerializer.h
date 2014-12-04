#ifndef COMPACTSERIALIZER_H_INCLUDED
#define COMPACTSERIALIZER_H_INCLUDED

class CompactSerializer: public AbstractSerializer
{

public:

    void serialize(vector<ReferentialMatchEntry> &l, string outputPath)
    {
        ofstream myFile;
        myFile.open (outputPath.c_str(), ios::out | ios::binary);
        zippedout=new boost::iostreams::filtering_ostream();

        zippedout->push(boost::iostreams::gzip_compressor(boost::iostreams::zlib::huffman_only,1000000));
        zippedout->push(myFile);

        lastsavedrefpos=0;
        lastsavedlength=-1;

        long mysize=l.size();
        long pos=0;

        std::vector<ReferentialMatchEntry> v(l.begin(), l.end());

        //cout<<"Number of entries="<<mysize<<"\n";
        //exit(-1);
        while (pos<mysize)
        {
            pos+=addRef(pos, &v);
        }


        zippedout->flush();
        zippedout->reset();
        delete zippedout;
        myFile.close();

        //myFile.close();
    }

    virtual void unserializeFromMainMemory(unsigned char *data, long comprLength, vector<ReferentialMatchEntry> &l)
    {
        l.clear();
        unsigned char b=0;
        long lastsavedrefpos=0;

        typedef boost::iostreams::stream<boost::iostreams::array_source> array_stream;
        array_stream unzippedin((char*)data, comprLength);

        unzippedin.read((char *)&b, 1);
        long line=0;

        //cout<<inputPath<<"\n";
        while (!unzippedin.eof())
        {
            long length=0;
            long pos=0;
            char loc=0;


         if (b==255)
            {
                //unsigned char b2, b3, b4;
                unsigned char sumlen;


                unzippedin.read((char*)&sumlen, 1);
                long bytecount=((sumlen-1)/4)+1;

                unsigned char sumbytes[bytecount];
                unzippedin.read((char*)&sumbytes, bytecount);

                reproduceSumBytes(sumbytes, bytecount, l,sumlen);


                pos=lastsavedrefpos;
                length=sumlen-1;
            }else if ((b>>7)==0)
            {
                pos=lastsavedrefpos;

                length=(b>>2)%32;
                loc=(b%4)+1;

                ReferentialMatchEntry rme(pos, length, convertLeftOverChar(loc),1);
                l.push_back(rme);
                //cout<<rme.pos<<","<<rme.length<<","<<rme.mismatchchar<<","<<rme.mismatchcount<<"\n";
            }else if ((b>>6)==2)
            {
                unsigned char b2;
                unzippedin.read((char*)&b2, 1);

                pos=lastsavedrefpos;
                length=(((long)(b%64))<<6)+(b2>>2);
                loc=(b2%4)+1;

                ReferentialMatchEntry rme(pos, length, convertLeftOverChar(loc),1);
                l.push_back(rme);
                //cout<<rme.pos<<","<<rme.length<<","<<rme.mismatchchar<<","<<rme.mismatchcount<<"\n";

            }else if ((b>>5) ==6)
            {
                unsigned char b2, b3, b4;
                unzippedin.read((char*)&b2, 1);
                unzippedin.read((char*)&b3, 1);
                unzippedin.read((char*)&b4, 1);

                pos=lastsavedrefpos+(char)b2;
                length=((long)b3)*256+((long)b4);
                loc=(b%4)+1;

                ReferentialMatchEntry rme(pos, length, convertLeftOverChar(loc),1);
                l.push_back(rme);
                //cout<<rme.pos<<","<<rme.length<<","<<rme.mismatchchar<<","<<rme.mismatchcount<<"\n";

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

                ReferentialMatchEntry rme(pos, length, convertLeftOverChar(loc),1);
                l.push_back(rme);
                //cout<<rme.pos<<","<<rme.length<<","<<rme.mismatchchar<<","<<rme.mismatchcount<<"\n";

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

                ReferentialMatchEntry rme(pos, length, convertLeftOverChar(loc),1);
                l.push_back(rme);
                //cout<<rme.pos<<","<<rme.length<<","<<rme.mismatchchar<<","<<rme.mismatchcount<<"\n";

            }else if ((b>>3) ==31)
            {
                unsigned char b2, b3, b4, b5, b6, b7, b8, b9,b10,b11,b12,b13;

                unsigned char mmatch;
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
                long mismatchcount=((long)b10)*256*256*256+((long)b11)*256*256+((long)b12)*256+((long)b13);

                loc=b%8;

                ReferentialMatchEntry rme(pos, length, mmatch,mismatchcount);
                l.push_back(rme);
                //cout<<rme.pos<<","<<rme.length<<","<<rme.mismatchchar<<","<<rme.mismatchcount<<"\n";
            }else{
                cout<<"Unknown command found -> exit\n";
                exit(-1);
            }

            if (pos<0)
            {
                //long a=1;
                cout<<"NEGPOSHERE("<<line<<"):";
                cout<<((long)b)<<"\n";
                exit(-1);
            }
            if (pos==4294967295)
            {
                cout<<"HERE("<<line<<"):";
                cout<<((long)b)<<"\n";
                exit(-1);
            }

            lastsavedrefpos=pos+length+1;


            unzippedin.read((char*)&b, 1);
            line++;
        }

    }

    void unserializeFromStream(ifstream *mystream, vector<ReferentialMatchEntry> &l)
    {
        l.clear();

        unsigned char b=0;

        long lastsavedrefpos=0;


        boost::iostreams::filtering_istream unzippedin;
        unzippedin.push(boost::iostreams::gzip_decompressor());
        unzippedin.push(*mystream);

        unzippedin.read((char *)&b, 1);
        long line=0;

        //cout<<inputPath<<"\n";
        while (!unzippedin.eof())
        {
            long length=0;
            long pos=0;
            char loc=0;


         if (b==255)
            {
                //unsigned char b2, b3, b4;
                unsigned char sumlen;


                unzippedin.read((char*)&sumlen, 1);
                long bytecount=((sumlen-1)/4)+1;

                unsigned char sumbytes[bytecount];
                unzippedin.read((char*)&sumbytes, bytecount);

                reproduceSumBytes(sumbytes, bytecount, l,sumlen);


                pos=0;
                length=sumlen-1;
            }else if ((b>>7)==0)
            {
                pos=lastsavedrefpos;

                length=(b>>2)%32;
                loc=(b%4)+1;

                ReferentialMatchEntry rme(pos, length, convertLeftOverChar(loc),1);
                l.push_back(rme);
                //cout<<rme.pos<<","<<rme.length<<","<<rme.mismatchchar<<","<<rme.mismatchcount<<"\n";
            }else if ((b>>6)==2)
            {
                unsigned char b2;
                unzippedin.read((char*)&b2, 1);

                pos=lastsavedrefpos;
                length=(((long)(b%64))<<6)+(b2>>2);
                loc=(b2%4)+1;

                ReferentialMatchEntry rme(pos, length, convertLeftOverChar(loc),1);
                l.push_back(rme);
                //cout<<rme.pos<<","<<rme.length<<","<<rme.mismatchchar<<","<<rme.mismatchcount<<"\n";

            }else if ((b>>5) ==6)
            {
                unsigned char b2, b3, b4;
                unzippedin.read((char*)&b2, 1);
                unzippedin.read((char*)&b3, 1);
                unzippedin.read((char*)&b4, 1);

                pos=lastsavedrefpos+(char)b2;
                length=((long)b3)*256+((long)b4);
                loc=(b%4)+1;

                ReferentialMatchEntry rme(pos, length, convertLeftOverChar(loc),1);
                l.push_back(rme);
                //cout<<rme.pos<<","<<rme.length<<","<<rme.mismatchchar<<","<<rme.mismatchcount<<"\n";

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

                ReferentialMatchEntry rme(pos, length, convertLeftOverChar(loc),1);
                l.push_back(rme);
                //cout<<rme.pos<<","<<rme.length<<","<<rme.mismatchchar<<","<<rme.mismatchcount<<"\n";

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

                ReferentialMatchEntry rme(pos, length, convertLeftOverChar(loc),1);
                l.push_back(rme);
                //cout<<rme.pos<<","<<rme.length<<","<<rme.mismatchchar<<","<<rme.mismatchcount<<"\n";

            }else if ((b>>3) ==31)
            {
                unsigned char b2, b3, b4, b5, b6, b7, b8, b9,b10,b11,b12,b13;
                unsigned char mmatch;
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
                long mismatchcount=((long)b10)*256*256*256+((long)b11)*256*256+((long)b12)*256+((long)b13);

                loc=b%8;

                ReferentialMatchEntry rme(pos, length, mmatch,mismatchcount);
                l.push_back(rme);
                //cout<<rme.pos<<","<<rme.length<<","<<rme.mismatchchar<<","<<rme.mismatchcount<<"\n";
            }else{
                cout<<"Unknown command found -> exit\n";
                exit(-1);
            }

            if (pos<0)
            {
                //long a=1;
                cout<<"NEGPOSHERE("<<line<<"):";
                cout<<((long)b)<<"\n";
                exit(-1);
            }
            if (pos==4294967295)
            {
                cout<<"HERE("<<line<<"):";
                cout<<((long)b)<<"\n";
                exit(-1);
            }

            lastsavedrefpos=pos+length+1;


            unzippedin.read((char*)&b, 1);
            line++;
        }

    }

    void unserialize(string inputPath, vector<ReferentialMatchEntry> &l)
    {
        ifstream myFile(inputPath.c_str());
        unserializeFromStream(&myFile,l);
        myFile.close();
    }



private:

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
                //cout<<"Unknown character in input:"<<mismatchchar<<":("<<((long)mismatchchar)<<")\n,"; exit(-1);
                    leftoverchar=-1;
                }
        }

        return leftoverchar;
    }

    bool summingUpApplicable(long indexpos, vector<ReferentialMatchEntry>* rcdata, long &sumlen)
    {
        long maxsumlen=sumlen;
        if (indexpos+maxsumlen>=(long)rcdata->size())
            return false;

        sumlen=0;
        for (long t=0;t<maxsumlen;t++)
        {
            if ((*rcdata)[indexpos+t].pos!=0) break;
            if ((*rcdata)[indexpos+t].length!=0) break;

            char leftoverchar=getLeftOverChar((*rcdata)[indexpos+t].mismatchchar);

            if (!(leftoverchar>=1 and leftoverchar<=4)) break;
            if ((*rcdata)[indexpos+t].mismatchcount>1) break;

            sumlen++;
        }

        return (sumlen>0);

    }


    long addRef(long indexpos, vector<ReferentialMatchEntry>* rcdata)
    {
        long posstep=1;


        long pos=(*rcdata)[indexpos].pos;

        if (pos==4294967295)
        {
            cout<<"NEGPROBLEM ....\n";
            exit(0);
        }

        long length=(*rcdata)[indexpos].length;
        long mismatchchar=(*rcdata)[indexpos].mismatchchar;
        long mismatchcount=(*rcdata)[indexpos].mismatchcount;
        //cout<<indexpos<<"\n";
        char leftoverchar=getLeftOverChar(mismatchchar);

        //if (pos-(lastsavedrefpos+lastsavedlength+1)==0 and summingUpApplicable(indexpos, rcdata, 12))
        long sumlen=254;
        if (summingUpApplicable(indexpos, rcdata, sumlen))
        {
            //cout<<"SU\n";
            unsigned char c=128+64+32+16+8+4+2+1;
            zippedout->write((const char*) &c, 1);
            zippedout->write((const char*) &sumlen, 1);

            long bytecount=((sumlen-1)/4)+1;
            unsigned char sumbytes[bytecount];

            fillSumBytes(sumbytes, bytecount, indexpos, rcdata, sumlen);

            zippedout->write((const char*)sumbytes, bytecount);

            //bytes[0]=((getLeftOverChar((*rcdata)[indexpos].mismatchchar)-1)<<6)+((getLeftOverChar((*rcdata)[indexpos+1].mismatchchar)-1)<<4)+((getLeftOverChar((*rcdata)[indexpos+2].mismatchchar)-1)<<2)+((getLeftOverChar((*rcdata)[indexpos+3].mismatchchar)-1));
            //bytes[1]=((getLeftOverChar((*rcdata)[indexpos+4].mismatchchar)-1)<<6)+((getLeftOverChar((*rcdata)[indexpos+5].mismatchchar)-1)<<4)+((getLeftOverChar((*rcdata)[indexpos+6].mismatchchar)-1)<<2)+((getLeftOverChar((*rcdata)[indexpos+7].mismatchchar)-1));
            //bytes[2]=((getLeftOverChar((*rcdata)[indexpos+8].mismatchchar)-1)<<6)+((getLeftOverChar((*rcdata)[indexpos+9].mismatchchar)-1)<<4)+((getLeftOverChar((*rcdata)[indexpos+10].mismatchchar)-1)<<2)+((getLeftOverChar((*rcdata)[indexpos+11].mismatchchar)-1));



            //pos=lastsavedrefpos+12;
            length=sumlen-1;
            posstep=sumlen;
        }
        else if (pos-(lastsavedrefpos+lastsavedlength+1)==0 and length<32 and leftoverchar>=1 and leftoverchar<=4 and mismatchcount==1)
        {
            //cout<<"2\n";
            bytes[0]=0+(length<<2);
            bytes[0]+=(leftoverchar-1);

            zippedout->write((const char*)bytes, 1);

        }else if (pos-(lastsavedrefpos+lastsavedlength+1)==0 and length<4090 and leftoverchar>=1 and leftoverchar<=4 and mismatchcount==1)
        {
            //cout<<"IP="<<indexpos<<"3\n";
            bytes[0]=128+(length>>6);
            bytes[1]=(length%64)<<2;
            bytes[1]+=(leftoverchar-1);

            zippedout->write((const char*)bytes, 2);

        }else if (abs(pos-(lastsavedrefpos+lastsavedlength+1))<127 and length<65000 and leftoverchar>=1 and leftoverchar<=4 and mismatchcount==1)
        {
            //cout<<"4\n";
            bytes[0]=128+64+(leftoverchar-1);
            bytes[1]=(unsigned char)(pos-(lastsavedrefpos+lastsavedlength+1));
            bytes[2]=(length>>8)%256;
            bytes[3]=(length%256);

            zippedout->write((const char*)bytes, 4);

        }else if (length<256 and leftoverchar>=1 and leftoverchar<=4 and mismatchcount==1)
        {
            //cout<<"6\n";
            bytes[0]=128+64+32+(leftoverchar-1);
            bytes[1]=pos>>24;
            bytes[2]=(pos>>16)%256;
            bytes[3]=(pos>>8)%256;
            bytes[4]=pos%256;
            bytes[5]=length;

            zippedout->write((const char*)bytes, 6);

        }else if (length<65000 and leftoverchar>=1 and leftoverchar<=4 and mismatchcount==1)
        {
            //cout<<"7\n";
            bytes[0]=128+64+32+16+(leftoverchar-1);
            bytes[1]=pos>>24;
            bytes[2]=(pos>>16)%256;
            bytes[3]=(pos>>8)%256;
            bytes[4]=pos%256;
            bytes[5]=(length>>8)%256;
            bytes[6]=length%256;

            zippedout->write((const char*)bytes, 7);
        }else
        {
            //cout<<"12\n";
            unsigned char c=128+64+32+16+8;
            zippedout->write((const char*) &c, 1);
            zippedout->write((const char*) &mismatchchar, 1);


            bytes[0]=(pos>>24);
            bytes[1]=(pos>>16)%256;
            bytes[2]=(pos>>8)%256;
            bytes[3]=pos%256;

            bytes[4]=(length>>24);
            bytes[5]=(length>>16)%256;
            bytes[6]=(length>>8)%256;
            bytes[7]=length%256;

            bytes[8]=(mismatchcount>>24);
            bytes[9]=(mismatchcount>>16)%256;
            bytes[10]=(mismatchcount>>8)%256;
            bytes[11]=mismatchcount%256;


            zippedout->write((const char *)&bytes, 12);
        }

        lastsavedrefpos=pos;
        lastsavedlength=length;

        return posstep;
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


    void fillSumBytes(unsigned char *sumbytes, long bytecount, long indexpos, vector<ReferentialMatchEntry>* rcdata, long sumlen)
    {
        long curpos=indexpos;
        for (long b=0;b<bytecount-1;b++)
        {
            sumbytes[b]=(getLeftOverChar((*rcdata)[curpos].mismatchchar)-1)<<6;
            curpos++;

            sumbytes[b]+=(getLeftOverChar((*rcdata)[curpos].mismatchchar)-1)<<4;
            curpos++;

            sumbytes[b]+=(getLeftOverChar((*rcdata)[curpos].mismatchchar)-1)<<2;
            curpos++;

            sumbytes[b]+=(getLeftOverChar((*rcdata)[curpos].mismatchchar)-1);
            curpos++;
        }
        long deltab=bytecount-1;
        sumbytes[deltab]=0;

        long missing=4;
        for (long delta=(bytecount-1)*4;delta<sumlen;delta++)
        {
            sumbytes[deltab]=(sumbytes[deltab]<<2)+(getLeftOverChar((*rcdata)[curpos].mismatchchar)-1);
            curpos++;
            missing--;
        }
        for (long t=0;t<missing;t++)
        {
            sumbytes[deltab]=(sumbytes[deltab]<<2);
        }

    }


    void reproduceSumBytes(unsigned char *sumbytes, long bytecount, vector<ReferentialMatchEntry>& l, long sumlen)
    {

        long curpos=0;
        for (long b=0;b<bytecount-1;b++)
        {

            ReferentialMatchEntry rme(0, 0, convertLeftOverChar((sumbytes[b]>>6)+1),1);
            l.push_back(rme);

            ReferentialMatchEntry rme2(0, 0, convertLeftOverChar(((sumbytes[b]>>4)%4)+1),1);
            l.push_back(rme2);

            ReferentialMatchEntry rme3(0, 0, convertLeftOverChar(((sumbytes[b]>>2)%4)+1),1);
            l.push_back(rme3);

            ReferentialMatchEntry rme4(0, 0, convertLeftOverChar(((sumbytes[b])%4)+1),1);
            l.push_back(rme4);

            curpos+=4;
        }

        if (curpos<sumlen)
        {
            ReferentialMatchEntry rme(0, 0, convertLeftOverChar((sumbytes[bytecount-1]>>6)+1),1);
            l.push_back(rme);
        }

        if (curpos+1<sumlen)
        {
            ReferentialMatchEntry rme2(0, 0, convertLeftOverChar(((sumbytes[bytecount-1]>>4)%4)+1),1);
            l.push_back(rme2);
        }

        if (curpos+2<sumlen)
        {
            ReferentialMatchEntry rme2(0, 0, convertLeftOverChar(((sumbytes[bytecount-1]>>2)%4)+1),1);
            l.push_back(rme2);
        }

        if (curpos+3<sumlen)
        {
            ReferentialMatchEntry rme2(0, 0, convertLeftOverChar(((sumbytes[bytecount-1])%4)+1),1); l.push_back(rme2);
        }


    }

    boost::iostreams::filtering_ostream *zippedout;

    long lastsavedrefpos;
    long lastsavedlength;

    unsigned char bytes[14];

    long longentry;
};



class RCSDeserializer
{

public:






};



#endif // COMPACTSERIALIZER_H_INCLUDED
