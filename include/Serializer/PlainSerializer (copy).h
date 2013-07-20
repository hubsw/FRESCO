#ifndef PLAINSERIALIZER_H_INCLUDED
#define PLAINSERIALIZER_H_INCLUDED

class PlainSerializer: public AbstractSerializer
{
public:
    void serialize(list<ReferentialMatchEntry> &l, string outputPath)
    {
        ofstream ofs(outputPath.c_str());

        for (std::list<ReferentialMatchEntry>::const_iterator iterator = l.begin(), end = l.end(); iterator != end; ++iterator)
            ofs<<((*iterator).pos)<<","<<(*iterator).length<<","<<(*iterator).mismatchchar<<","<<(*iterator).mismatchcount<<"\n";

        ofs.close();
    }

    virtual void unserialize(string inputPath, list<ReferentialMatchEntry> &l)
    {
        ifstream ifs(inputPath.c_str());
        string s;
        while (getline(ifs,s))
        {
            istringstream iss(s);
            long pos, length, mismatchcount;
            char mismatchchar,t1,t2,t3;
            iss>>pos>>t1>>length>>t2>>mismatchchar>>t3>>mismatchcount;
            ReferentialMatchEntry rme(pos, length, mismatchchar, mismatchcount);
            l.push_back(rme);
        }

        ifs.close();
    }

};

#endif // PLAINSERIALIZER_H_INCLUDED
