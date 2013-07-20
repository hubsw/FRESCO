#ifndef SERIALIZERFACTORY_H_INCLUDED
#define SERIALIZERFACTORY_H_INCLUDED

class SerializerFactory
{
public:

    static AbstractSerializer* getSerializer(string type)
    {
        if (type=="CompactSerializer") return new CompactSerializer();
        else if (type=="PlainSerializer") return new PlainSerializer();
        else if (type=="PlainDeltaSerializer") return new PlainDeltaSerializer();
        else
        {
            cout<<"Unknown compressor type: "<<type<<"\n";
        }
        return NULL;
    }

};

#endif // SERIALIZERFACTORY_H_INCLUDED
