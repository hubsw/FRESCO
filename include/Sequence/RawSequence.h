#ifndef RAWSEQUENCE_H_INCLUDED
#define RAWSEQUENCE_H_INCLUDED

class RawSequence: public AbstractSequence
{
public:

    ~RawSequence()
    {
        delete[] cdata;
    }

    void load(string path)
    {
        data=loadFromRaw(path);
        long dl=data.length();
        cdata=new char[dl+16];
        strcpy(cdata, data.c_str());
        for (long l=0;l<16;l++)
            cdata[dl+l]=0;


    }

    void loadFromString(string _data)
    {
        data=_data;
        long dl=data.length();
        cdata=new char[dl+16];
        strcpy(cdata, data.c_str());
        for (long l=0;l<16;l++)
            cdata[dl+l]=0;


    }

    void save(string path)
    {
        writeToRaw(data, path);
    }

};

#endif // RAWSEQUENCE_H_INCLUDED
