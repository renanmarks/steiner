#ifndef MODTHOMSONCONSTRUTIVE_H
#define MODTHOMSONCONSTRUTIVE_H

#include <queue>
#include "irstalgorithm.h"

namespace st
{

class ModThomsonConstrutive : public st::IRSTAlgorithm
{
private:
    st::Graph graph;

    using Vertex = st::Graph::Vertex;
    using VertexPair = std::pair<Vertex, Vertex>;

    struct LessVertexPair : public std::less<VertexPair>
    {
        bool operator()( const VertexPair& lhs, const VertexPair& rhs ) const
        {
            std::int32_t distance1 = lhs.first.distanceTo(lhs.second);
            std::int32_t distance2 = rhs.first.distanceTo(rhs.second);

            return distance1 < distance2;
        }
    };

    using DistanceHeap = std::priority_queue<VertexPair, std::vector<VertexPair>, LessVertexPair>;

    DistanceHeap heap;
    const st::Data* data;

    VertexPair getMinDistanceVertices();
    void loadVertices();

public:
    ModThomsonConstrutive(const st::Data& _data);

    // IRSTAlgorithm interface
public:
    virtual st::Graph run() override;
};

}

#endif // MODTHOMSONCONSTRUTIVE_H
