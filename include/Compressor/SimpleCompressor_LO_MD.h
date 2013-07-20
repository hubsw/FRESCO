#ifndef SIMPLECOMPRESSOR_LO_MD_H_INCLUDED
#define SIMPLECOMPRESSOR_LO_MD_H_INCLUDED

class SimpleCompressor_LO_MD: public AbstractCompressor
{
public:
    void compress(AbstractSequence *seq, AbstractIndex *ci, vector<ReferentialMatchEntry> &outputRMEs)
    {
        //string sinput=seq->data;
        char* input=seq->cdata;//(char*)sinput.c_str();;

        outputRMEs.clear();
        long ml=0,mp=0,mc=0,prevml=0;
        char* curinput=input;

        long outputsize=0;
        long processedsum=0;

        outputRMEs.reserve(10000);

        while (*curinput!=0)
        {
            mc=1;
            ml=0;
            if (*curinput=='N')
            {
                ml=0;
            }else{

                if (checkLocalOptimization())
                {
                    //cout<<mp<<"\n";
                    if (mp<ci->reflength)
                        ml=getMatchLength64bit(curinput, ci->refcontent+mp);
                }else
                    ml=0;

                if (ml<20)
                {
                    long mp2,ml2;
                    ci->findLongestMatch(curinput, mp2, ml2);

                    if (!checkMatchDistance() or abs(mp2-mp)<100000 or ml2>70)
                    {
                        mp=mp2;ml=ml2;
                    }
                }
            }


            if (ml==0 and *curinput=='N')
            {
                char* temp=curinput;
                while ((*temp==*curinput) and (*temp!=0))
                    temp++;
                mc=temp-curinput;
            }

            if (*(curinput+ml)==0 and ml>0) ml--;

            if (ml<=12 and !((*curinput)=='N') and prevml==0)
            {
                for (long t=0;t<ml;t++)
                {
                    outputRMEs.push_back(ReferentialMatchEntry(0, 0, curinput[t],1));
                    outputsize++;
                }

                outputRMEs.push_back(ReferentialMatchEntry(0, 0, curinput[ml],1));
                outputsize++;
                prevml=0;

            }else
            {
                outputRMEs.push_back(ReferentialMatchEntry(max(mp,(long)0), ml, curinput[ml],mc));
                outputsize++;
                prevml=ml;
            }



            curinput+=(ml+1*mc);
            processedsum+=(ml+1*mc);

            //cout<<processedsum<<"/"<<sinput.length()<<" .... last ml="<<ml<<"\n";

            mp+=ml+1*mc;

        }
    };

    void decompress(vector<ReferentialMatchEntry> inputRMEs, AbstractIndex *ci, AbstractSequence *seq)
    {
        /*stringstream buffer;

        for (std::vector<ReferentialMatchEntry>::const_iterator iterator = inputRMEs.begin(), end = inputRMEs.end(); iterator != end; ++iterator)
        {
            char *snippet=new char[(*iterator).length+1];
            strncpy(snippet, ci->refcontent+(*iterator).pos, (*iterator).length);
            snippet[(*iterator).length]=0;
            buffer << snippet;
            delete[] snippet;
            for (long l=0;l<(*iterator).mismatchcount;l++)
                buffer<<(*iterator).mismatchchar;
        }
        seq->data=buffer.str();*/
        seq->data="";

        long totalLength=0;
        for (std::vector<ReferentialMatchEntry>::const_iterator iterator = inputRMEs.begin(), end = inputRMEs.end(); iterator != end; ++iterator)
            totalLength+=(*iterator).length+(*iterator).mismatchcount;
        //cout<<totalLength<<"\n";

        seq->cdata=new char[totalLength+16];
        for (long l=0;l<16;l++)
            seq->cdata[totalLength+l]=0;

        char* temp=seq->cdata;
        //long advanced=0;
        //ofstream ofs("/bio/debugrmes.txt");
        for (std::vector<ReferentialMatchEntry>::const_iterator iterator = inputRMEs.begin(), end = inputRMEs.end(); iterator != end; ++iterator)
        {
            //ofs<<(*iterator).pos<<","<<(*iterator).length<<","<<(*iterator).mismatchchar<<","<<(*iterator).mismatchcount<<"\n"<<flush;
            //char c=*temp;
            //char d=ci->refcontent[0];
            //char e=ci->refcontent[(*iterator).pos];

            strncpy(temp, ci->refcontent+(*iterator).pos, (*iterator).length);
            temp+=(*iterator).length;
            //advanced+=(*iterator).length;
            for (long l=0;l<(*iterator).mismatchcount;l++)
            {
                *temp=(*iterator).mismatchchar;
                temp++;
                //advanced++;
            }
        }
        //ofs.close();
        seq->data=seq->cdata;

        /*for (std::vector<ReferentialMatchEntry>::const_iterator iterator = inputRMEs.begin(), end = inputRMEs.end(); iterator != end; ++iterator)
        {
            char *snippet=new char[(*iterator).length+1];
            strncpy(snippet, ci->refcontent+(*iterator).pos, (*iterator).length);
            snippet[(*iterator).length]=0;
            seq->data+=snippet;
            delete[] snippet;
            for (long l=0;l<(*iterator).mismatchcount;l++)
                seq->data+=(*iterator).mismatchchar;
        }*/


    };

private:

    virtual bool checkLocalOptimization()
    {
        return true;
    }

    virtual bool checkMatchDistance()
    {
        return true;
    }

};

#endif // SIMPLECOMPRESSOR_LO_MD_H_INCLUDED
