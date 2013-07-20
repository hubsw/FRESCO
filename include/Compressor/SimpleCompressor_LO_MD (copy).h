#ifndef SIMPLECOMPRESSOR_LO_MD_H_INCLUDED
#define SIMPLECOMPRESSOR_LO_MD_H_INCLUDED

class SimpleCompressor_LO_MD: public AbstractCompressor
{
public:
    void compress(AbstractSequence *seq, AbstractIndex *ci, list<ReferentialMatchEntry> &outputRMEs)
    {
        string sinput=seq->data;
        char* input=(char*)sinput.c_str();;

        outputRMEs.clear();
        long ml=0,mp=0,mc=0,prevml=0;
        char* curinput=input;

        long outputsize=0;
        long processedsum=0;

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
                        ml=getMatchLength(curinput, ci->refcontent+mp);
                }

                else
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

            if (ml<=5 and *curinput!='N'and prevml==0)
            {
                for (long t=0;t<ml;t++)
                {
                    if (curinput[t]==-31)
                    {
                        long a=1;
                    }

                    outputRMEs.push_back(ReferentialMatchEntry(max(mp+t,(long)0), 0, curinput[t],1));
                    outputsize++;
                }

                if (curinput[ml]==-31)
                {
                    long a=1;
                }

                outputRMEs.push_back(ReferentialMatchEntry(max(mp+ml,(long)0), 0, curinput[ml],1));
                outputsize++;
                prevml=0;

            }else
            {
                if (curinput[ml]==-31)
                {
                    long a=1;
                }
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

    void decompress(list<ReferentialMatchEntry> inputRMEs, AbstractIndex *ci, AbstractSequence *seq)
    {
        /*stringstream buffer;

        for (std::list<ReferentialMatchEntry>::const_iterator iterator = inputRMEs.begin(), end = inputRMEs.end(); iterator != end; ++iterator)
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

        for (std::list<ReferentialMatchEntry>::const_iterator iterator = inputRMEs.begin(), end = inputRMEs.end(); iterator != end; ++iterator)
        {
            char *snippet=new char[(*iterator).length+1];
            strncpy(snippet, ci->refcontent+(*iterator).pos, (*iterator).length);
            snippet[(*iterator).length]=0;
            seq->data+=snippet;
            delete[] snippet;
            for (long l=0;l<(*iterator).mismatchcount;l++)
                seq->data+=(*iterator).mismatchchar;
        }


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
