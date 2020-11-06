
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstring>
#include <stdexcept>

#include "BufferAllocator.hpp"


BufferAllocator::Ref::Ref() {
}


BufferAllocator::Ref::Ref(Regions::iterator it) :
    _it(it) {
}


BufferAllocator::BufferAllocator() {
    _buffer = 0;
}


BufferAllocator::BufferAllocator(BufferAllocator && rhs) :
    _regions(std::move(rhs._regions)),
    _index(std::move(rhs._index)),
    _target(rhs._target),
    _buffer(rhs._buffer) {
    rhs._buffer = 0;
}


BufferAllocator & BufferAllocator::operator=(BufferAllocator && rhs) {
    _regions = std::move(rhs._regions);
    _index = std::move(rhs._index);
    _target = rhs._target;
    _buffer = rhs._buffer;
    rhs._buffer = 0;
    return *this;
}


BufferAllocator::BufferAllocator(unsigned initialSz, unsigned target, unsigned usage) {
    // Create the buffer
    _target = target;
    glGenBuffers(1, &_buffer);
    glBindBuffer(target, _buffer);
    glBufferData(target, initialSz, 0, usage);
    glBindBuffer(target, 0);

    // Add the initial range
    _regions.emplace_front();
    Regions::iterator it = _regions.begin();
    it->start = 0;
    it->end = initialSz;
    it->it = _index.insert(std::make_pair(initialSz, it));
}


BufferAllocator::~BufferAllocator() {
    // Release the buffer
    glDeleteBuffers(1, &_buffer);
}


BufferAllocator::Ref BufferAllocator::allocate(void * data, unsigned sz) {
    // Find the best fit
    RegionIndex::iterator it = _index.lower_bound(sz);
    if (it == _index.end()) {
        // No memory available
        throw std::runtime_error("Out of memory"); // TODO something useful
    }

    // Remove this region from the index
    Regions::iterator thisRegion = it->second;
    _index.erase(it);
    thisRegion->it = _index.end();

    // If there is a tail then split the region
    if (thisRegion->end - thisRegion->start != sz) {
        Regions::iterator nextRegion = thisRegion;
        ++nextRegion;
        nextRegion = _regions.insert(nextRegion, Region());

        nextRegion->end = thisRegion->end;
        nextRegion->start = thisRegion->start + sz;
        thisRegion->end = thisRegion->start + sz;
        _index.insert(std::make_pair(nextRegion->end - nextRegion->start, nextRegion));
    }

    // Copy the data into the region
    glBindBuffer(_target, _buffer);
    void * buf = glMapBufferRange(_target, thisRegion->start, sz, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
    memcpy(buf, data, sz);
    glUnmapBuffer(_target);
    glBindBuffer(_target, 0);

    // Return the iterator for deallocation
    return Ref(thisRegion);
}


void BufferAllocator::free(Ref ref) {
    // Merge with a free section before
    if (ref._it != _regions.begin()) {
        Regions::iterator prev = ref._it;
        --prev;

        if (prev->it != _index.end()) {
            _index.erase(prev->it);
            ref._it->start = prev->start;
            _regions.erase(prev);
        }
    }

    // Merge with a free section after
    if (ref._it != _regions.end()) {
        Regions::iterator next = ref._it;
        ++next;

        if (next != _regions.end() && next->it != _index.end()) {
            _index.erase(next->it);
            ref._it->end = next->end;
            _regions.erase(next);
        }
    }

    // Insert into index
    ref._it->it = _index.insert(std::make_pair(ref._it->end - ref._it->start, ref._it));
}


