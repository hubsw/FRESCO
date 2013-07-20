#ifndef HLREFERENCEREWRITING_H_INCLUDED
#define HLREFERENCEREWRITING_H_INCLUDED

typedef std::map<std::pair<ReferentialMatchEntry, ReferentialMatchEntry>, long > tempmapT;

bool value_comparer(tempmapT::value_type &i1, tempmapT::value_type &i2)
{
    return i1.second<i2.second;
}

struct IntCmp {
    bool operator()(const std::pair<std::pair<ReferentialMatchEntry, ReferentialMatchEntry>, long> &lhs, const std::pair<std::pair<ReferentialMatchEntry, ReferentialMatchEntry>, long> &rhs) {
        return lhs.second > rhs.second;
    }
};



class ProbabilisticGenomeComputer
{
public:

    void computeProbabilisticReference(ConfigFileParser &cfp, vector<string> inputPaths, string referencePath, string referenceOutPath, double percentage)
    {
        string ref=loadFromRaw(referencePath);

        long counter=1;

        tempmapT tempmap;

        short *snprecording=new short[ref.length()*5+6];

        for (long l=0;l<(long)ref.length()*5+5;l++)
            snprecording[l]=0;

        short *snprecordingadd=new short[ref.length()*5+6];

        for (long l=0;l<(long)ref.length()*5+5;l++)
            snprecordingadd[l]=0;

        short *snprecordingdel=new short[ref.length()];

        for (long l=0;l<(long)ref.length()*5+5;l++)
            snprecordingadd[l]=0;


        long replcounter=0,inscounter=0, delcounter=0;
        BOOST_FOREACH(string compPath, inputPaths)
        {
            cout<<counter<<":"<<compPath<<"\n";

            //ReferentiallyCompressedSequence rcs;
            vector<ReferentialMatchEntry> rmes=_loadsRMEs(cfp, compPath);

            for (long pos=0;pos<(long)rmes.size()-1;pos++)
            {
                if (rmes[pos].pos+rmes[pos].getTotalLength()==rmes[pos+1].pos)
                {
                    if (rmes[pos].mismatchchar=='A') snprecording[(rmes[pos].pos+rmes[pos].length)*5]++;
                    if (rmes[pos].mismatchchar=='C') snprecording[(rmes[pos].pos+rmes[pos].length)*5+1]++;
                    if (rmes[pos].mismatchchar=='G') snprecording[(rmes[pos].pos+rmes[pos].length)*5+2]++;
                    if (rmes[pos].mismatchchar=='T') snprecording[(rmes[pos].pos+rmes[pos].length)*5+3]++;
                    if (rmes[pos].mismatchchar=='N') snprecording[(rmes[pos].pos+rmes[pos].length)*5+4]++;

                    replcounter++;

                }else if (rmes[pos].pos+rmes[pos].getTotalLength()-1==rmes[pos+1].pos)
                {
                    if (rmes[pos].mismatchchar=='A') snprecordingadd[(rmes[pos].pos+rmes[pos].length)*5]++;
                    if (rmes[pos].mismatchchar=='C') snprecordingadd[(rmes[pos].pos+rmes[pos].length)*5+1]++;
                    if (rmes[pos].mismatchchar=='G') snprecordingadd[(rmes[pos].pos+rmes[pos].length)*5+2]++;
                    if (rmes[pos].mismatchchar=='T') snprecordingadd[(rmes[pos].pos+rmes[pos].length)*5+3]++;
                    if (rmes[pos].mismatchchar=='N') snprecordingadd[(rmes[pos].pos+rmes[pos].length)*5+4]++;


                    inscounter++;

                }
                else if (rmes[pos].pos+rmes[pos].getTotalLength()+1==rmes[pos+1].pos)
                {
                    snprecordingdel[(rmes[pos].pos+rmes[pos].length)]++;
                    delcounter++;

                }

            }
            counter++;

        }

        cout<<"REPL="<<replcounter<<"\n";

        char* newref=new char[ref.length()+1];
        newref[ref.length()]=0;

        strncpy(newref, ref.c_str(), ref.length());

        //open: copy code for insertion and deletion of bases

        for (long l=0;l<(long)ref.length();l++)
        {
                long maxc=snprecording[l*5];
                char maxchar='A';

                if (maxc<snprecording[l*5+1])
                { maxc=snprecording[l*5+1]; maxchar='C'; }

                if (maxc<snprecording[l*5+2])
                { maxc=snprecording[l*5+2]; maxchar='G'; }

                if (maxc<snprecording[l*5+3])
                { maxc=snprecording[l*5+3]; maxchar='T'; }

                if (maxc<snprecording[l*5+4])
                { maxc=snprecording[l*5+4]; maxchar='N'; }

                if (ref[l]!=maxchar and maxc > (counter*percentage))
                {
                    //cout<<l<<":"<<ref->data[l]<<"->"<<maxchar<<"["<<maxc<<"]\n";
                    newref[l]=maxchar;
                }
        }



        ofstream newrefofstream(referenceOutPath.c_str());
        newrefofstream<<newref;
        newrefofstream.close();

        delete snprecordingdel;
        delete snprecordingadd;
        delete snprecording;
        delete newref;


    }


};


#endif // HLREFERENCEREWRITING_H_INCLUDED
