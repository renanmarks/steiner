#include "edgeorientedlocalsearch.h"
#include "tabusearch.h"
#include <algorithm>

st::TabuSearch::TabuSearch(st::IRSTAlgorithm& _construtive, uint32_t _numberIterations, size_t _tabuListSize, int32_t _incumbentCost)
    : construtive(_construtive), numberIterations(_numberIterations), tabuListSize(_tabuListSize), incumbentCost(_incumbentCost)
{

}

st::Graph st::TabuSearch::run()
{
    // TODO: Build better architecture :)
    std::deque<st::Graph> tabuList;
    st::Graph bestTree = construtive.run();

    std::uint32_t maxNotChangedCount = this->numberIterations;
    std::uint32_t notChangedCount = 0;
    std::uint32_t iterations = 0;

    for (iterations = 0;
         (iterations < this->numberIterations) && (notChangedCount < maxNotChangedCount);
         ++iterations, ++notChangedCount)
    {
        st::EdgeOrientedLocalSearch localSearch(bestTree);
        auto neighbourMoves = localSearch.getValidNeighbourMoves();


        for (const st::Graph& move : neighbourMoves )
        {
            auto found = std::find(tabuList.begin(), tabuList.end(), move);

            if (found == tabuList.end() && move.getDistance() < bestTree.getDistance())
            {
                bestTree = move;
                maxNotChangedCount = 0;
            }
            else
            {
                std::int32_t cost  = std::abs( static_cast<std::int32_t>(bestTree.getDistance() - move.getDistance()));
                std::int32_t cost2 = this->incumbentCost;

                if ( (cost < cost2) && (move.getDistance() > bestTree.getDistance()) )
                {
                    bestTree = move;
                    maxNotChangedCount = 0;
                }
            }
        }

        tabuList.push_back(bestTree);

        if (tabuList.size() > this->tabuListSize)
        {
            tabuList.pop_front();
        }
    }

    return bestTree;
}
