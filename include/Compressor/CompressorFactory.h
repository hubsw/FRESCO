#ifndef COMPRESSORFACTORY_H_INCLUDED
#define COMPRESSORFACTORY_H_INCLUDED

class CompressorFactory
{
public:

    static AbstractCompressor* getCompressor(string type)
    {
        if (type=="SimpleCompressor") return new SimpleCompressor();
        else if (type=="SimpleCompressor_LO") return new SimpleCompressor_LO();
        else if (type=="SimpleCompressor_LO_MD") return new SimpleCompressor_LO_MD();
        else
        {
            cout<<"Unknown compressor type: "<<type<<"\n";
        }
        return NULL;
    }

};


#endif // COMPRESSORFACTORY_H_INCLUDED
