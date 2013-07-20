#ifndef SEQUENCEONLYINDEX_H_INCLUDED
#define SEQUENCEONLYINDEX_H_INCLUDED

class SequenceOnlyIndex: public AbstractIndex
{
public:

    SequenceOnlyIndex()
    {

    }

    SequenceOnlyIndex(string reference)
    {
        reflength=reference.length();
        refcontent=new char[reflength+1];
        strcpy(refcontent, reference.c_str());
    }

    void initialize(string referencePath)
    {
        string reference=loadFromRaw(referencePath);
        reflength=reference.length();
        refcontent=new char[reflength+1];
        strcpy(refcontent, reference.c_str());
    }

    void findLongestMatch(char* cur, long &pos, long &length)
    {
    }

};

#endif // SEQUENCEONLYINDEX_H_INCLUDED
