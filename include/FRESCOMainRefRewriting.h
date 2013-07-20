#ifndef FRESCOMAINREFREWRITING_H_INCLUDED
#define FRESCOMAINREFREWRITING_H_INCLUDED

class FRESCOMainRefRewriting
{
public:

    void rewrite(ConfigFileParser &cfp, string inpath, string outpath)
    {
        ifstream inf(inpath+"compressedlist.txt");

        vector<string> compressedStringsPaths;
        string refpath;
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

        //compute the delta structures from RMEs
        ProbabilisticGenomeComputer pgc;
        pgc.computeProbabilisticReference(cfp, compressedStringsPaths, refpath, outpath+"newref.txt", 0.47);

        HighLevel::convertCompression(cfp, compressedStringsPaths, refpath,outpath+"newref.txt", outpath);

        ofstream ofs(outpath+"compressedlist.txt");
        ofs<<outpath<<"newref.txt"<<"\n";
        BOOST_FOREACH(string s, compressedStringsPaths)
        {
            string fn=SystemTools::getFileName(s);
            ofs<<outpath<<fn<<"\n";
        }
        ofs.close();

    }

};

#endif // FRESCOMAINREFREWRITING_H_INCLUDED
