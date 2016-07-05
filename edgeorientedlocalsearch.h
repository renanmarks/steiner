#ifndef EDGEORIENTEDLOCALSEARCH_H
#define EDGEORIENTEDLOCALSEARCH_H

#include <unordered_set>
#include <queue>
#include "irstalgorithm.h"
#include "graph.h"

namespace st
{

class EdgeOrientedLocalSearch : public st::IRSTAlgorithm
{
private:
    using Edge = Graph::Edge;
    using Vertex = Graph::Vertex;
    using VertexPair = std::pair<Vertex, Vertex>;

    struct LowerDistanceTree : public std::less<Graph>
    {
        bool operator()( const Graph& lhs, const Graph& rhs ) const
        {
            return lhs.getDistance() >= rhs.getDistance();
        }
    };

    using MinimumDistanceSet = std::priority_queue<Graph, std::vector<Graph>, LowerDistanceTree>;
    MinimumDistanceSet minDistanceTrees;
    Graph minDistanceTree;

    Graph tree;

    void removeVertex(Vertex s, Graph &tree) const;
    void removeSteinerLeafs(Graph &graph) const;
    Graph buildLocalTrees() const;
    void connect(const VertexPair &pair, Graph &graph) const;
    void checkAndCrystalize(const Vertex &steinerNode, Graph &graph) const;
public:
    EdgeOrientedLocalSearch(const Graph& _tree);

    std::vector<Graph> getValidNeighbourMoves() const;
    // IRSTAlgorithm interface
public:
    st::Graph run();
};

}
#endif // EDGEORIENTEDLOCALSEARCH_H
