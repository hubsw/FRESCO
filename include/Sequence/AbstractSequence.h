#ifndef ABSTRACTSEQUENCE_H_INCLUDED
#define ABSTRACTSEQUENCE_H_INCLUDED

class AbstractSequence
{
public:
    virtual ~AbstractSequence() {};
    virtual void load(string path) {};
    virtual void save(string path) {};

    string data;
    char* cdata;
};

#endif // ABSTRACTSEQUENCE_H_INCLUDED
