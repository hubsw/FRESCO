#ifndef PLAINDELTASERIALIZER_H_INCLUDED
#define PLAINDELTASERIALIZER_H_INCLUDED

class PlainDeltaSerializer: public AbstractSerializer
{
public:
    void serialize(list<ReferentialMatchEntry> &l, string outputPath)
    {
        ofstream ofs(outputPath.c_str());

        long oldpos=0;

        for (std::list<ReferentialMatchEntry>::const_iterator iterator = l.begin(), end = l.end(); iterator != end; ++iterator)
        {
            ofs<<((*iterator).pos-oldpos)<<","<<(*iterator).length<<","<<(*iterator).mismatchchar<<","<<(*iterator).mismatchcount<<"\n";
            oldpos=(*iterator).pos+(*iterator).length+(*iterator).mismatchcount;
        }


        ofs.close();
    }

    virtual void unserialize(string inputPath, list<ReferentialMatchEntry> &l)
    {
        ifstream ifs(inputPath.c_str());
        string s;
        long oldpos=0;

        while (getline(ifs,s))
        {
            istringstream iss(s);
            long deltapos, length, mismatchcount;
            char mismatchchar,t1,t2,t3;
            iss>>deltapos>>t1>>length>>t2>>mismatchchar>>t3>>mismatchcount;
            ReferentialMatchEntry rme(deltapos+oldpos, length, mismatchchar, mismatchcount);
            l.push_back(rme);

            oldpos=oldpos+deltapos+length+mismatchcount;
        }

        ifs.close();
    }

};

#endif // PLAINDELTASERIALIZER_H_INCLUDED
