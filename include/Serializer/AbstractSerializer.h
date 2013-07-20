#ifndef ABSTRACTSERIALIZER_H_INCLUDED
#define ABSTRACTSERIALIZER_H_INCLUDED

class AbstractSerializer
{
public:
    virtual ~AbstractSerializer(){};
    virtual void serialize(vector<ReferentialMatchEntry> &l, string outputPath) {};
    virtual void unserialize(string inputPath, vector<ReferentialMatchEntry> &l) {};
    virtual void unserializeFromMainMemory(unsigned char *data, long comprLength, vector<ReferentialMatchEntry> &l) {};
};


#endif // ABSTRACTSERIALIZER_H_INCLUDED
