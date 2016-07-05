#include "grasp.h"
#include "edgeorientedlocalsearch.h"


st::Grasp::Grasp(const st::Data &_data, st::IRSTAlgorithm &_construtive, uint32_t _numberIterations)
    : data(_data), construtive(_construtive), numberIterations(_numberIterations)
{

}

st::Graph st::Grasp::run()
{
    // TODO: Build better architecture :)
    st::Graph bestTree;

    std::uint32_t maxNotChangedCount = this->numberIterations / 10;
    std::uint32_t notChangedCount = 0;
    std::uint32_t iterations = 0;

    for (iterations = 0;
         (iterations < this->numberIterations) && (notChangedCount < maxNotChangedCount);
         ++iterations, ++notChangedCount)
    {
        bestTree = this->construtive.run();

        st::EdgeOrientedLocalSearch localsearch(bestTree);

        Graph newTree = localsearch.run();

        if (newTree.getDistance() < bestTree.getDistance())
        {
            bestTree = newTree;
            notChangedCount = 0;
        }
    }

    return bestTree;
}
