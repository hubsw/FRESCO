#ifndef CONFIGFILEPARSER_H_INCLUDED
#define CONFIGFILEPARSER_H_INCLUDED

class ConfigFileParser
{
public:
    ConfigFileParser(string path)
    {
        ifstream myf(path.c_str());

        string line;
        while(getline(myf, line))
        {
            trimWS(line);
            if (line.length()>0)
            {

                std::vector<std::string> kv=split(line, '=');
                entries[kv[0]]=kv[1];
            }
        }

    }

    map<string,string> entries;


};

#endif // CONFIGFILEPARSER_H_INCLUDED
