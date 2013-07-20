#ifndef BASEINCLUDES_H_INCLUDED
#define BASEINCLUDES_H_INCLUDED

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <fstream>
#include <map>
#include <vector>
#include <set>
#include <list>
#include <cstring>
#include <bitset>
#include <queue>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <algorithm>
#include <dirent.h>

using namespace std;

#include "zlib.h"

#include <boost/foreach.hpp>

#include <boost/lambda/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/functional/hash.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/dynamic_bitset.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/unordered_map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/detail/config/zlib.hpp>



string loadFromRaw(string path)
{
    ifstream t(path.c_str());
    stringstream buffer;
    buffer << t.rdbuf();
    string s=buffer.str();

    //remove newline
    if (s[s.size()-1]=='\n')
    {
        s=s.substr(0,s.size()-1);
    }
    t.close();
    return s;
}

void writeToRaw(string data, string path)
{
    ofstream ofs(path.c_str());
    ofs<<data;
    ofs.close();
}

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while(std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    return split(s, delim, elems);
}

long string2long(string &s)
{
    std::stringstream sstr(s);
    long val;
    sstr >> val;
    return val;
}

void trimWS(string &str)
{
    for (int i=0;i<(int)str.length();i++)
        if (str[i]==' ')
        {
            str.erase(i,1);
            i--;
        }
}

class StopWatch
{
    public:
        StopWatch(bool startAlready=true) { if (startAlready) start();}

        void start()
        {

            gettimeofday(&startTime, NULL);
        }

        void stop(bool print=true)
        {
            gettimeofday(&endTime, NULL);

            if (print)
                std::cout << "Total Time Taken: " << getDiff() << " ms."  << std::endl;
        }

        void stopS(string output)
        {
            gettimeofday(&endTime, NULL);

            std::cout << "Time Taken for "<<output<<": " << getDiff() << " ms."  << std::endl;
        }

        double getDiff()
        {
            double tS = startTime.tv_sec*1000000 + (startTime.tv_usec);
            double tE = endTime.tv_sec*1000000  + (endTime.tv_usec);
            return ((tE-tS)/1000);

        }

    protected:
    private:

        struct timeval startTime;
        struct timeval endTime;
};

string getFileName(string filePath)
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

int getAllFilesInDirectory(string dir, std::vector<string> &files)
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

char* getSubStringL(char* input, long tempLength, long start, long length)
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

long getFileSize(string s)
{
    ifstream file(s.c_str(), ios::binary | ios::ate);
    return file.tellg();
}

void renewDir(string path)
{
    boost::filesystem::remove_all(path);
    boost::filesystem::create_directories(path);}



#endif // BASEINCLUDES_H_INCLUDED
