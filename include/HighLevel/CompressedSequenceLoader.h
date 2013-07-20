#ifndef COMPRESSEDSEQUENCELOADER_H_INCLUDED
#define COMPRESSEDSEQUENCELOADER_H_INCLUDED

class CompressedSequenceLoader
{

public:

    CompressedSequenceLoader()
    {

    }

    static void loadSingleCompressedFile(string comprPath, vector<ReferentialMatchEntry> &rmes, string &filename)
    {
        if (!SystemTools::existsFile(comprPath))
        {
            cout<<"CompressedSequenceLoader: File does not exist: "<<comprPath<<". Exit!\n";
            exit(0);
        }else
        {
            AbstractSerializer *serializer=SerializerFactory::getSerializer("CompactSerializer");
            serializer->unserialize(comprPath,rmes);
            filename=getFileName(comprPath);
            delete serializer;
        }
    }

    static void loadAllCompressedFiles(string comprPath, vector<vector<ReferentialMatchEntry> > &rmess, vector<string> &filenames, long limit=-1)
    {
        std::vector<string> files;
        getAllFilesInDirectory(comprPath, files);

        long counter=0;
        BOOST_FOREACH(string path, files)
        {
            vector<ReferentialMatchEntry> rmes;
            string filename=SystemTools::getFileName(path);

            loadSingleCompressedFile(path, rmes, filename);
            rmess.push_back(rmes);
            filenames.push_back(filename);

            counter++;

            if (limit>0 and limit==counter)
                break;
        }


    }

    static void loadAllCompressedFilesRCS(string comprPath, vector<ReferentiallyCompressedSequence > &rcss, vector<string> &filenames, long limit=-1)
    {
        std::vector<string> files;
        getAllFilesInDirectory(comprPath, files);

        long counter=0;
        BOOST_FOREACH(string path, files)
        {
            string filename=SystemTools::getFileName(path);
            cout<<counter<<":"<<filename<<"\n";

            ReferentiallyCompressedSequence rcs(path);
            rcss.push_back(rcs);
            filenames.push_back(filename);

            counter++;

            if (limit>0 and limit==counter)
                break;
        }


    }


};


#endif // COMPRESSEDSEQUENCELOADER_H_INCLUDED
