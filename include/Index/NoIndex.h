#ifndef NOINDEX_H_INCLUDED
#define NOINDEX_H_INCLUDED

class NoIndex: public AbstractIndex
{
public:

    ~NoIndex()
    {
        delete refcontent;
    }
    void initialize(string referencePath)
    {

        string reference=loadFromRaw(referencePath);
        reflength=reference.length();
        refcontent=new char[reflength+16];
        strcpy(refcontent, reference.c_str());
        for (long l=0;l<16;l++)
            refcontent[reflength+l]=0;
    }

    void findLongestMatch(char* cur, long &pos, long &length)
    {
        length=0;
    }
};

#endif // NOINDEX_H_INCLUDED
