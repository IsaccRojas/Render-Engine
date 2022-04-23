#include "util.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//reads n characters (or up to null terminator) of filename into buf;
//returns length, or -1 if failure
int get_file_str(const char *filename, char *buf, int n) {
    std::ifstream file;
    try {
        file.open(filename);
    } catch (std::system_error& e) {
        std::cout << "ERR: get_file_str(): " << e.code().message() << std::endl;
        return -1;
    }

    char c;
    for (int i = 0; i < n; i++) {
        //store c in buf if good; else store \0 if not eof and return
        try {
            c = file.get();
        } catch (std::system_error& e) {
            //no problem
            if (file.eof())
                return i;
            
            std::cout << "ERR: get_file_str(): " << e.code().message() << std::endl;
            buf[i] = '\0';
            return -1;
        }

        buf[i] = c;
    }
    return n;
}

Image::Image(const char *filename) {
    load(filename);
}
Image::Image(const Image &other) :
    _data(other.copydata()),
    _w(other._w),
    _h(other._h),
    _components(other._components),
    _size(other._size)
{}
Image::Image() : _data(NULL), _w(0), _h(0), _size(0) {};
Image& Image::operator=(const Image &other) {
    _data = other.copydata();
    _w = other._w;
    _h = other._h;
    _components = other._components;
    _size = other._size;
    return *this;
}
Image::~Image() {
    free();
}

void Image::load(const char *filename) {
    _data = stbi_load(filename, &_w, &_h, &_components, 4);
    _size = _w * _h * 4;
}

void Image::free() {
    stbi_image_free(_data);
    _data = NULL;
}

unsigned char* Image::copydata() const {
    if (_data == NULL)
        return NULL;
    
    char *copy = new char[_size];
    return (unsigned char*)memcpy((void*)copy, (void*)_data, _size * sizeof(unsigned char));
}

int Image::width() { return _w; }
int Image::height() { return _h; }
int Image::components() { return _components; }
int Image::size() { return _size; }
bool Image::empty() { return (_data == NULL); }

Partitioner::Partitioner() {}
Partitioner::~Partitioner() {
    clear();
}

//occupies an index in IDs (use last index from freeIDs if available),
//and return ID
int Partitioner::push() {
    if (freeIDs.empty()) {
        IDs.push_back(true);
        return IDs.size() - 1;
    }

    int i = freeIDs.back();
    freeIDs.pop_back();
    IDs.at(i) = true;
    return i;
}

//sets element i to false and pushes its index to freeIDs
bool Partitioner::erase_at(int i) {
    if (IDs.at(i)) {
        IDs.at(i) = false;
        freeIDs.push_back(i);
        return true;
    }
    return false;
}

//get vector of all indices that are true
std::vector<int> Partitioner::getused() {
    std::vector<int> indices;
    for (unsigned i = 0; i < IDs.size(); i++)
        if (IDs[i])
            indices.push_back(i);
    
    return indices;
}

void Partitioner::clear() {
    IDs.clear();
    freeIDs.clear();
}

//access element i
bool Partitioner::at(int i) { return IDs.at(i); }
//get whether used IDs are empty
bool Partitioner::empty() { return (IDs.size() == 0); }
//get size of IDs (includes free IDs)
unsigned Partitioner::size() { return IDs.size(); }
//get size of free IDs
unsigned Partitioner::freesize() { return freeIDs.size(); }
//get size of used IDs
unsigned Partitioner::fillsize() { return IDs.size() - freeIDs.size(); }