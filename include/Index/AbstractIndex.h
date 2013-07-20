#ifndef ABSTRACTINDEX_H_INCLUDED
#define ABSTRACTINDEX_H_INCLUDED

class AbstractIndex
{
public:
    virtual ~AbstractIndex(){};
    virtual void initialize(string referencePath) {};
    virtual void findLongestMatch(char* input, long &pos, long &length) {};
    virtual void getMatchLengthInRef(char* s1, long refdelta, long &length) {};

    char* refcontent;
    long reflength;
};

long getMatchLength(char* s1, char* s2)
{
    long matchlen=0;
    while (s1[0]!=0 and s2[0]!=0)
    {
        if (*s1==*s2) matchlen++;
        else return matchlen;

        s1++; s2++;
    }
    return matchlen;
}

long getMatchLength64bit(char* s1, char* s2)
{
    long *ls1=(long*)s1;
    long *ls2=(long*)s2;
    long matchlen=0;
    while (ls1[0]!=0 and ls2[0]!=0)
    {
        if (*ls1==*ls2) matchlen++;
        else
        {
            break;
        }
        ls1++; ls2++;
    }
    matchlen=max((matchlen-1)*8,(long)0);
    char* news1=(char *)max(((long)ls1)-8,(long)s1);
    char* news2=(char *)max(((long)ls2)-8,(long)s2);
    while ((*news1!=0)and(*news2!=0) and(*news1==*news2))
    {
        //cout<<(long)(*news1)<<"/"<<(long)(*news2)<<"\n";
        news1++;news2++;
        matchlen++;

    }

    return matchlen;
}


#endif // ABSTRACTINDEX_H_INCLUDED
