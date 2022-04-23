#pragma once

#ifndef UTIL_HPP_
#define UTIL_HPP_

#include <iostream>
#include <fstream>
#include <vector>

//reads n characters (or up to null terminator) of filename into buf;
//returns length, or -1 if failure
int get_file_str(const char *filename, char *buf, int n);

class Image {
    unsigned char *_data;
    int _w;
    int _h;
    int _components;
    int _size;
public:
    Image(const char *filename);
    Image(const Image &other);
    Image();
    Image& operator=(const Image &other);
    ~Image();

    void load(const char *filename);
    void free();

    unsigned char* copydata() const;
    int width();
    int height();
    int components();
    int size();
    bool empty();
};

class Partitioner {
    //main ID vector
    std::vector<bool> IDs;
    //vector of free IDs
    std::vector<int> freeIDs;

public:
    Partitioner();
    ~Partitioner();

    //occupies an index in IDs (use last index from freeIDs if available),
	//and return ID
    int push();

    //sets element i to false and pushes its index to freeIDs
    bool erase_at(int i);

    //get vector of all indices that are true
    std::vector<int> getused();

    void clear();

    //access element i
    bool at(int i);
    //get whether used IDs are empty
    bool empty();
    //get size of IDs (includes free IDs)
    unsigned size();
    //get size of free IDs
    unsigned freesize();
    //get size of used IDs
    unsigned fillsize();
};

#endif