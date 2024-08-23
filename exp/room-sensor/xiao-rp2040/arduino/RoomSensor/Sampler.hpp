#pragma once


typedef unsigned long ulong;

template<typename T>
struct Sample
{
    ulong clock = 0;
    T value; 
};


template<typename T>
class Sampler 
{
    private:

    Sample<T> sampleA;
    Sample<T> sampleB;
    Sample<T>* current = &sampleA;
    Sample<T>* previous = &sampleB;

    public:

    Sample<T>* Current() const { return current; };
    Sample<T>* Previous() const { return previous; };

    void TakeSample(ulong clock, T newValue);
    bool HasSampleChanged() const;

};


#include "Sampler.t.hpp"

typedef Sampler<int> PinSampler;


