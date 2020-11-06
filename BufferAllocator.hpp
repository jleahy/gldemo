#ifndef BufferAllocator_hpp
#define BufferAllocator_hpp

#include <list>
#include <map>


class BufferAllocator {
private:
    struct Region;
    typedef std::list<Region> Regions;
    typedef std::multimap<unsigned, Regions::iterator> RegionIndex;

public:
    class Ref {
    public:
        Ref();

        unsigned operator*() const {
            return _it->start;
        }

    private:
        Ref(Regions::iterator it);

        Regions::iterator _it;
        friend class BufferAllocator;
    };

    BufferAllocator();
    BufferAllocator(BufferAllocator && rhs);
    BufferAllocator & operator=(BufferAllocator && rhs);
    BufferAllocator(unsigned initialSz, unsigned target, unsigned usage);
    ~BufferAllocator();

    Ref allocate(void * data, unsigned sz);
    void free(Ref ref);

    unsigned operator*() const {
        return _buffer;
    }

private:
    struct Region {
        RegionIndex::iterator it;
        unsigned start;
        unsigned end;
    };
    
    Regions         _regions;
    RegionIndex     _index; // by size
    unsigned        _target;
    unsigned        _buffer;
};


#endif
