#ifndef IRSTALGORITHM_H
#define IRSTALGORITHM_H

#include "graph.h"

namespace st
{

class IRSTAlgorithm
{
public:
    virtual ~IRSTAlgorithm() { }

    virtual st::Graph run() = 0;

    IRSTAlgorithm() = default;
    IRSTAlgorithm(const IRSTAlgorithm& copyFrom) = default;
    IRSTAlgorithm& operator=(const IRSTAlgorithm& copyFrom) = default;
    IRSTAlgorithm(IRSTAlgorithm &&) = default;
    IRSTAlgorithm& operator=(IRSTAlgorithm &&) = default;
};

}

#endif // IRSTALGORITHM_H
