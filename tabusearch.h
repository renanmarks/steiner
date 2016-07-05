#ifndef TABUSEARCH_H
#define TABUSEARCH_H

#include "data.h"
#include "irstalgorithm.h"


namespace st {

class TabuSearch : public st::IRSTAlgorithm
{
private:
    st::IRSTAlgorithm& construtive;
    std::uint32_t numberIterations;
    std::size_t tabuListSize;
    std::int32_t incumbentCost;

public:
    TabuSearch(st::IRSTAlgorithm& _construtive, uint32_t _numberIterations, size_t _tabuListSize, int32_t _incumbentCost);

    // IRSTAlgorithm interface
public:
    Graph run();
};

}

#endif // TABUSEARCH_H
