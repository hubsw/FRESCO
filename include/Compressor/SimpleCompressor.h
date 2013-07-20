#ifndef SIMPLECOMPRESSOR_H_INCLUDED
#define SIMPLECOMPRESSOR_H_INCLUDED

class SimpleCompressor: public SimpleCompressor_LO_MD
{
private:
    bool checkLocalOptimization()
    {
        return false;
    }

    bool checkMatchDistance()
    {
        return false;
    }
};

#endif // SIMPLECOMPRESSOR_H_INCLUDED
