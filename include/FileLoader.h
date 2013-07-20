#ifndef FILELOADER_H_INCLUDED
#define FILELOADER_H_INCLUDED


class MyFileEntry{
public:
    MyFileEntry(string _name, long _size)
    {
        name=_name;
        size=_size;
    }

    bool operator< (const MyFileEntry &mfe) const
    {
        if (size>mfe.size)
            return true;

        return false;
    }


    string name;
    long size;
};


class FileLoader
{
public:

    FileLoader(string path)
    {
        if (!SystemTools::existsFile(path))
        {
            cout<<"Problem: file "<<path<<" does not exist! Exit.\n";
            exit(0);
        }

        ifstream inf(path.c_str());

        vector<MyFileEntry> ve;

        string s;
        while (getline(inf,s))
        {
            if (s.length()>1)
            {
                MyFileEntry mfe(s, SystemTools::getFileSize(s));
                ve.push_back(mfe);
            }


        }
        inf.close();

        //sort by size of the files
        sort (ve.begin(), ve.end());

        //heuristic: take longest sequence as reference
        reference=ve[0].name;

        //heuristic: take random sequence as reference
        //reference=ve[abs(rand()%ve.size())].name;

        for (long l=0;l<(long)ve.size();l++)
        {
            allpaths.push_back(ve[l].name);
        }

    }


    void saveFileList(string path)
    {
        ofstream ofs(path);

        BOOST_FOREACH(string s, allpaths)
        {
            ofs<<s<<".fresco\n";
        }

        ofs.close();
    }

    string reference;
    vector<string> allpaths;

};

#endif // FILELOADER_H_INCLUDED
