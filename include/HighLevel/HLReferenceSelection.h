#ifndef HLREFERENCESELECTION_H_INCLUDED
#define HLREFERENCESELECTION_H_INCLUDED

#define MAXMODIDS 360000

class ReferenceSelection
{
public:

    string runOnPath(ConfigFileParser &cfp, string path)
    {

        ifstream inf(path);

        vector<string> compressedStringsPaths;
        string refpath;
        getline(inf,refpath);
        string s;
        while (getline(inf,s))
        {
            if (s.length()>1)
            {
                compressedStringsPaths.push_back(s);
            }
        }

        std::vector<string> files(compressedStringsPaths.begin(), compressedStringsPaths.end());
        //getAllFilesInDirectory(path, files);

        std::vector<string> files2(compressedStringsPaths.begin(), compressedStringsPaths.end());

        //files.resize(100);

        std::map<string, bitset<MAXMODIDS> > mymap;

        //ofstream ofsdebug("/bio/debug.txt");

        BOOST_FOREACH(string fp, files2)
        {
            //ReferentiallyCompressedSequence rcs;
            //RCSDeserializer des;
            //des.deserialize("/run/media/sebastian/MiniStation/bio/1K/COMPvsHG19/chr"+chr+"/"+name+"_"+chr+".comp", rcs);
            //des.deserialize(fp, rcs);

            vector<ReferentialMatchEntry> rmes=_loadsRMEs(cfp, fp);

            //long extrmodcount=0;

            BOOST_FOREACH(ReferentialMatchEntry rme, rmes)
            {
                long ID=abs((rme.pos*7771+rme.length*7+rme.mismatchchar+rme.mismatchcount*8743)%MAXMODIDS);

                if (ID<0)
                {
                    cout<<"ID<0\n";
                    exit(0);
                }
                //if((extrmodcount++)%5==0)

                    //ofsdebug<<ID<<"\n"<<flush;


                    mymap[fp].set(ID);

                //maxcount++;

                //if (maxcount>500)
                    //break;
            }
        }
        //ofsdebug.close();
        //cout<<"TT\n";

        map<string, long> result;

        BOOST_FOREACH(string fp, files)
        {
            cout<<fp<<"\n";
            long sim=0;
            BOOST_FOREACH(string fp2, files2)
            {
                double cup=(mymap[fp]|mymap[fp2]).count();
                double cap=(mymap[fp]&mymap[fp2]).count();
                long diff=10000*(1-cap/cup);
                sim+=diff;
            }
            //string fname=getFileName(fp);

            result[fp]=sim;

            //maxcount++;

            //if (maxcount>500)
            //    break;

        }


        //ofstream ofs("/bio/refselout.csv");

        //M::iterator itor = std::min_element(result.begin(), result.end(), value_comparer);
        //cout<<algs[algid]->name<<":"<<itor->first<<"=>"<<itor->second<<"\n";

        map<string, long > :: iterator it;

        long min=23523523523;
        string minstring="";
        for(it = result.begin(); it != result.end(); ++it)
        {
            //ofs<<it->first<<","<<it->second<<"\n";

            if (it->second<min)
            {
                min=it->second;
                minstring=it->first;
            }
        }

        //ofs.close();

        return minstring;

    }

    /*string runOnPath(string path)
    {

        std::vector<string> files;
        getAllFilesInDirectory(path, files);

        std::vector<string> files2;
        getAllFilesInDirectory(path, files2);

        //files.resize(100);

        std::map<string, bitset<MAXMODIDS> > mymap;

        //ofstream ofsdebug("/bio/debug.txt");

        BOOST_FOREACH(string fp, files2)
        {
            //ReferentiallyCompressedSequence rcs;
            //RCSDeserializer des;
            //des.deserialize("/run/media/sebastian/MiniStation/bio/1K/COMPvsHG19/chr"+chr+"/"+name+"_"+chr+".comp", rcs);
            //des.deserialize(fp, rcs);

            vector<ReferentialMatchEntry> rmes=_loadsRMEs(fp);

            //long extrmodcount=0;

            BOOST_FOREACH(ReferentialMatchEntry rme, rmes)
            {
                long ID=(rme.pos*7771+rme.length*7+rme.mismatchchar+rme.mismatchcount*8743)%MAXMODIDS;

                if (ID<0)
                {
                    cout<<"ID<0\n";
                    exit(0);
                }
                //if((extrmodcount++)%5==0)

                    //ofsdebug<<ID<<"\n"<<flush;


                    mymap[fp].set(ID);

                //maxcount++;

                //if (maxcount>500)
                    //break;
            }
        }
        //ofsdebug.close();
        //cout<<"TT\n";

        map<string, long> result;

        BOOST_FOREACH(string fp, files)
        {
            cout<<fp<<"\n";
            long sim=0;
            BOOST_FOREACH(string fp2, files2)
            {
                double cup=(mymap[fp]|mymap[fp2]).count();
                double cap=(mymap[fp]&mymap[fp2]).count();
                long diff=10000*(1-cap/cup);
                sim+=diff;
            }
            string fname=getFileName(fp);

            result[fname]=sim;

            //maxcount++;

            //if (maxcount>500)
            //    break;

        }


        ofstream ofs("/bio/refselout.csv");

        //M::iterator itor = std::min_element(result.begin(), result.end(), value_comparer);
        //cout<<algs[algid]->name<<":"<<itor->first<<"=>"<<itor->second<<"\n";

        map<string, long > :: iterator it;

        long min=23523523523;
        string minstring="";
        for(it = result.begin(); it != result.end(); ++it)
        {
            ofs<<it->first<<","<<it->second<<"\n";

            if (it->second<min)
            {
                min=it->second;
                minstring=it->first;
            }
        }

        ofs.close();

        return minstring;

    }*/

};


#endif // HLREFERENCESELECTION_H_INCLUDED
