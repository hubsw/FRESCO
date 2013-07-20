#ifndef ABSTRACTCOMPRESSOR_H_INCLUDED
#define ABSTRACTCOMPRESSOR_H_INCLUDED


class ReferentialMatchEntry
{
public:
    ReferentialMatchEntry(long _pos, long _length, char _mismatchchar, long _mismatchcount)
    {
        pos=_pos;
        length=_length;
        mismatchchar=_mismatchchar;
        mismatchcount=_mismatchcount;
    }

    long getTotalLength()
    {
        return length+mismatchcount;
    }

    string toString()
    {
        string result;
        result="("+boost::lexical_cast<string>(pos)+","+boost::lexical_cast<string>(length)+","+mismatchchar+","+boost::lexical_cast<string>(mismatchcount)+")";
        return result;
    }

    bool operator<( const ReferentialMatchEntry& other ) const
    {
        if (pos==other.pos)
        {
            if (length==other.length)
            {
                if (mismatchchar==other.mismatchchar)
                    return mismatchcount<other.mismatchcount;
                else
                {
                    return mismatchchar<other.mismatchchar;
                }
        } else
            {
                return length<other.length;
            }
        }else
            return pos<other.pos;

    }

    bool operator==( const ReferentialMatchEntry& other ) const
    {
        if (pos!=other.pos) return false;

        if (length!=other.length) return false;


        if (mismatchchar!=other.mismatchchar) return false;

        if (mismatchcount!=other.mismatchcount) return false;

        return true;
    }


    int pos;
    int length;
    int mismatchcount;
    char mismatchchar;

};

class AbstractCompressor
{
public:
    virtual ~AbstractCompressor(){};
    virtual void compress(AbstractSequence *seq, AbstractIndex *ci, vector<ReferentialMatchEntry> &outputRMEs) {};
    virtual void decompress(vector<ReferentialMatchEntry> inputRMEs, AbstractIndex *ci, AbstractSequence *seq) {};
};


#endif // ABSTRACTCOMPRESSOR_H_INCLUDED
