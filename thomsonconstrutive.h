#ifndef THOMSONCONSTRUTIVE_H
#define THOMSONCONSTRUTIVE_H

#include <unordered_set>
#include <queue>
#include <boost/functional/hash.hpp>
#include "irstalgorithm.h"

namespace st
{

class ThomsonConstrutive : public st::IRSTAlgorithm
{
private:
    st::Graph graph;

    using Vertex = st::Graph::Vertex;
    using VertexPair = std::pair<Vertex, Vertex>;

    struct EqualVertexPair : public std::equal_to<VertexPair>
    {
        bool operator()(const VertexPair &lhs, const VertexPair &rhs) const
        {
            bool same1 = (lhs.first == rhs.second) && (lhs.second == rhs.first);
            bool same2 = (lhs.first == rhs.first) && (lhs.second == rhs.second);

            return same1 || same2;
        }
    };

    struct HashVertexPair : public std::less<VertexPair>
    {
        std::size_t operator()( const VertexPair& lhs) const
        {
            std::size_t val { 0 };
            boost::hash_combine(val, lhs.first.index);
            boost::hash_combine(val, lhs.first.type);
            boost::hash_combine(val, lhs.first.x);
            boost::hash_combine(val, lhs.first.y);
            boost::hash_combine(val, lhs.second.index);
            boost::hash_combine(val, lhs.second.type);
            boost::hash_combine(val, lhs.second.x);
            boost::hash_combine(val, lhs.second.y);

            return val;
        }
    };

    struct LessVertexPair : public std::less<VertexPair>
    {
        bool operator()( const VertexPair& lhs, const VertexPair& rhs ) const
        {
            std::int32_t distance1 = lhs.first.distanceTo(lhs.second);
            std::int32_t distance2 = rhs.first.distanceTo(rhs.second);

            return distance1 >= distance2;
        }
    };

    using VertexPairSet = std::unordered_set<VertexPair, HashVertexPair, EqualVertexPair>;
    using MinimumDistanceSet = std::priority_queue<VertexPair, std::vector<VertexPair>, LessVertexPair>;

    VertexPairSet vertexPairSet;
    MinimumDistanceSet minDistanceHeap;

    const st::Data* data;
    bool randomStart;

    VertexPair getRandomDistanceVertices();
    VertexPair getMinDistanceVertices();

    void loadVertices();
    void connect(const VertexPair& pair);

    void setupDistanceHeap(const std::vector<Vertex> &l);
    void checkAndCrystalize(const Vertex &steinerNode);

public:
    ThomsonConstrutive(const st::Data& _data);
    ThomsonConstrutive(const st::Data& _data, bool _randomStart);

    // IRSTAlgorithm interface
public:
    virtual st::Graph run() override;
};

}

#endif // THOMSONCONSTRUTIVE_H
