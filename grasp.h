#ifndef GRASP_H
#define GRASP_H

#include "irstalgorithm.h"
#include "data.h"

namespace st
{
class Grasp : public st::IRSTAlgorithm
{
private:
    IRSTAlgorithm& construtive;
    std::uint32_t numberIterations;

public:
    Grasp(IRSTAlgorithm& _construtive, std::uint32_t _numberIterations);

    // IRSTAlgorithm interface
public:
    st::Graph run();
};
}

#endif // GRASP_H
