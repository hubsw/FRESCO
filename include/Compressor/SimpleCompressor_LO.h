#ifndef SIMPLECOMPRESSOR_LO_H_INCLUDED
#define SIMPLECOMPRESSOR_LO_H_INCLUDED

class SimpleCompressor_LO: public SimpleCompressor_LO_MD
{
private:
    bool checkLocalOptimization()
    {
        return true;
    }

    bool checkMatchDistance()
    {
        return false;
    }
};

#endif // SIMPLECOMPRESSOR_LO_H_INCLUDED
