#ifndef SYSTEMTOOLS_H
#define SYSTEMTOOLS_H

boost::mutex the_mutex_LFT;

#define FASTALINELENGTH 80


class SystemTools
{
    public:
        SystemTools() {}
        virtual ~SystemTools() {}


        static long getFileSize(string source)
        { return boost::filesystem::file_size(source);}


        static long getDirSize(string source)
        {
             std::vector<string> files2;
             SystemTools::getAllFilesInDirectory(source, files2);

             long size=0;
             BOOST_FOREACH(string compPath, files2)
                size+=SystemTools::getFileSize(compPath);

            return size;}

        static void createDir(string path)
        {boost::filesystem::create_directories(path);}

        static void renewDir(string path)
        {
            boost::filesystem::remove_all(path);
            boost::filesystem::create_directories(path);}

        static void remove(string path)
        {boost::filesystem::remove(path);}

        static void removeAll(string path)
        {boost::filesystem::remove_all(path);}

        static bool existsFile(string path)
        {return boost::filesystem::exists(path);}

        static int getAllFilesInDirectory(string dir, std::vector<string> &files)
        {
            unsigned char isFile =0x8;
            DIR *dp;
            struct dirent *dirp;
            if((dp  = opendir(dir.c_str())) == NULL)
            {
                cout << "Error(" << errno << ") opening " << dir << endl;
                return errno;
            }

            while ((dirp = readdir(dp)) != NULL) {
                if ( dirp->d_type == isFile)
                    files.push_back(dir+string(dirp->d_name));
                //files.push_back(dir+"/"+string(dirp->d_name));
            }
            closedir(dp);
            return 0;
        }


        static int getAllDirsInDirectoryRelative(string dir, std::vector<string> &files)
        {
            unsigned char isFile =0x4;
            DIR *dp;
            struct dirent *dirp;
            if((dp  = opendir(dir.c_str())) == NULL)
            {
                cout << "Error(" << errno << ") opening " << dir << endl;
                return errno;
            }

            while ((dirp = readdir(dp)) != NULL) {
                if ( dirp->d_type == isFile)
                    files.push_back(string(dirp->d_name));
                //files.push_back(dir+"/"+string(dirp->d_name));
            }
            closedir(dp);
            return 0;
        }

        static void convert2Fasta(string inpath, string outpath)
        {
            long ref_len;
            char* ref;
            ref=SystemTools::loadFromText(inpath, ref_len);

            string fname=SystemTools::getFileName(inpath);
            string indname=SystemTools::getFirstDotElement(fname);

            ofstream ofs(outpath.c_str());


            ofs<<">"<<indname<<"\n";
            char* cur=ref;
            char temp[FASTALINELENGTH+1];
            temp[FASTALINELENGTH]=0;
            while (cur-ref<ref_len)
            {
                long length=min((long)FASTALINELENGTH, (long)ref_len-(cur-ref));
                strncpy(temp, cur, length);
                temp[length]=0;
                string s=temp;
                ofs<<temp<<"\n";
                cur+=length;
            }


            ofs.close();
            delete []ref;
        }

        static char* loadFromText(string url)
        {
            long a=0;
            char *result=loadFromText(url, a);
            return result;
        }

        static char* loadFromText(string url, long& len)
        {
            //boost::mutex::scoped_lock lock(the_mutex_LFT);

            //cout<<"loading raw file "<<url<<"\n";

            if (!existsFile(url))
            {
                cout<<"File "<<url<<" does not exist! Quit!\n";
                exit(-1);
            }

            ifstream t(url.c_str());
            stringstream buffer;
            buffer << t.rdbuf();
            string s=buffer.str();
            s.length();
            char *result=new char[s.length()+1];
            strcpy(result,s.c_str());
            s[s.length()]=0;
            len=s.length();
            return result;
        }





        static string getFileName(string filePath)
        {
            std::vector<std::string> strs;
            boost::split(strs, filePath, boost::is_any_of("/"));
            return strs.at(strs.size()-1);
        }

        static string getFirstDotElement(string fileName)
        {
            std::vector<std::string> strs;
            boost::split(strs, fileName, boost::is_any_of("."));
            return strs.at(0);
        }



        static long getGZipFileSize(const string &path)
        {
            FILE *file=fopen(path.c_str(), "rb");

            if(file == NULL) return 0;
            if(fseek(file, -4, SEEK_END) != 0) return 0;
            unsigned char           bufSize[4];
            unsigned int len = fread(&bufSize[0], sizeof(unsigned char), 4, file);

            if(len != 4)
            {
                fclose(file);
                return 0;
            }
            long fileSize = (long) ((bufSize[3] << 24) | (bufSize[2] << 16) | (bufSize[1] << 8) | bufSize[0]);
            fclose(file);

            return fileSize;
        }


        static char* getSubString(char* input, long start, long length)
        {
            //cout << "Attention, slow getSubString called ... \n";
            long tempLength=strlen(input);
            if (tempLength<length)
                length=tempLength;

            if (start<0)
            {
                cout << "tt";

            }

            char *block=new char[length+1];
            memcpy(block,(char*)(input+start), ((long)length));
            block[(long)length]=0;
            return block;
        }

        static char* getSubStringL(char* input, long tempLength, long start, long length)
        {
            //cout << "Attention, slow getSubString called ... \n";
            if (tempLength<length)
                length=tempLength;

            if (start<0)
            {
                cout << "tt";

            }

            char *block=new char[length+1];
            memcpy(block,(char*)(input+start), ((long)length));
            block[(long)length]=0;
            return block;
        }

        static string getTextFromPos(const string &path, long pos, long length)
        {
            FILE *file=fopen(path.c_str(), "rb");
            if(file == NULL) return 0;
            if(fseek(file, pos, SEEK_SET) != 0) return 0;
            unsigned char bufSize[length+1];
            fread(&bufSize[0], sizeof(unsigned char), length, file);
            fclose(file);
            bufSize[length]=0;
            return (char*)bufSize;
        }

        static void loadFromZippedText(string inputPath, char* &result, long& retlength)
        {
            //cout<<"loading gz file "<<inputPath<<"\n";

            if (!SystemTools::existsFile(inputPath)) { cout << "File " << inputPath << " does not exist. Stop!\n"; exit(1);}
            long uncsize=SystemTools::getGZipFileSize(inputPath);
            gzFile fp=gzopen(inputPath.c_str(), "r");
            //cout << "Loading " << uncsize << " bytes for"<<inputPath<<".... \n" << flush;
            result=new char[uncsize];
            retlength=gzread(fp, (void*)(result), uncsize);
            gzclose(fp);
        }

        static void zipString2File(string path, char* s, long length)
        {
            gzFile *fi = (gzFile *)gzopen(path.c_str(),"wb");
            //BOOST_FOREACH(char c, s)
            //{
            //    gzwrite(fi,&c,1);
            //}
            gzwrite(fi,s,length);
            gzclose(fi);
        }

        static string set2string(set<string> ss)
        {
            string result="";
            long pos=1, size=ss.size();

            BOOST_FOREACH(string s, ss)
            {
                result+=s;
                if (pos!=size) result+=",";
                pos++;
            }
            return result;
        }

        static string set2string(set<long> ss)
        {
            string result="";
            long pos=1, size=ss.size();

            BOOST_FOREACH(long s, ss)
            {
                result+=boost::lexical_cast<string>(s);
                if (pos!=size) result+=",";
                pos++;
            }
            return result;
        }

        static string set2string(vector<long> ss)
        {
            string result="";
            long pos=1, size=ss.size();

            BOOST_FOREACH(long s, ss)
            {
                result+=boost::lexical_cast<string>(s);
                if (pos!=size) result+=",";
                pos++;
            }
            return result;
        }

        static string vector2string(vector<string> ss)
        {
            string result="";
            long pos=1, size=ss.size();

            BOOST_FOREACH(string s, ss)
            {
                result+=s;
                if (pos!=size) result+=",";
                pos++;
            }
            return result;
        }

        static string vector2string(vector<long> ss)
        {
            string result="";
            long pos=1, size=ss.size();

            BOOST_FOREACH(long s, ss)
            {
                result+=boost::lexical_cast<string>(s);
                if (pos!=size) result+=",";
                pos++;
            }
            return result;
        }

        static string longvector2string(vector<long> ss)
        {
            string result="";
            long pos=1, size=ss.size();

            BOOST_FOREACH(long s, ss)
            {
                long x=(s>>32);
                long y=s&((((long)1)<<32)-1);

                result+=boost::lexical_cast<string>(x)+":"+boost::lexical_cast<string>(y);
                if (pos!=size) result+=",";
                pos++;
            }
            return result;
        }


        static string longset2string(set<long> &ss)
        {
            string result="";
            long pos=1, size=ss.size();

            BOOST_FOREACH(long s, ss)
            {
                result+=boost::lexical_cast<string>(s);
                if (pos!=size) result+=",";
                pos++;
            }
            return result;
        }

        static long getSymbolSize(char *s, long len)
        {
            map<long, long> cmap;
            for (long l=0;l<len;l++)
            {
                cmap[(long)s[l]]++;
            }

            long count=0;

            for (long l=0;l<255;l++)
            {
                if (cmap[l]>0)
                    count++;
            }

            return count;
        }

        static void loadFromTextToPointer(string url, long& len, char* &pointer, long &pointerlen)
        {
            if (!existsFile(url))
            {
                cout<<"File "<<url<<" does not exist! Quit!\n";
                exit(-1);
            }

            len=getFileSize(url);

            if (len+1>pointerlen)
            {
                cout<<"Allocating new load buffer ..\n";

                if (pointer!=NULL) delete[] pointer;

                pointerlen=(long)(len*1.1)+1;
                pointer=new char[pointerlen];
            }

            ifstream t(url.c_str());
            t.read (pointer,len);
            pointer[len]=0;
        }

        static long getSymbolSizeFromPath(string path)
        {
            char* p=NULL;
            long plen, len;
            loadFromTextToPointer(path,len, p, plen);

            long res=getSymbolSize(p, len);

            delete p;

            return res;
        }


    protected:
    private:

        //boost::condition_variable the_condition_variable_LFT;
};

#endif // SYSTEMTOOLS_H
