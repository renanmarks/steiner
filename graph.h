#ifndef GRAPH_H
#define GRAPH_H

#include <cstdint>
#include <boost/graph/adjacency_list.hpp>
#include <boost/pending/disjoint_sets.hpp>
#include "data.h"

namespace st
{

class Graph
{ 
public:
    struct Vertex
    {
        enum class Type
        {
            NODE, CORNER, STEINER
        };

        std::int32_t index;
        std::int32_t x;
        std::int32_t y;
        Type type;

        Vertex();
        explicit Vertex(const st::Data::Vertex& v);
        Vertex(int32_t _x, int32_t _y);
        Vertex(int32_t _x, int32_t _y, Type _t);

        std::int32_t distanceTo(const Vertex& other) const;

        bool operator==(const Vertex& other) const;
        bool operator!=(const Vertex& other) const;
    };

    struct Edge
    {
        enum class Type
        {
            NONE, HORIZONTAL, VERTICAL
        };

        Vertex source;
        Vertex target;
        Type type;
        bool locked;

        Edge();
        Edge(const Vertex& _s, const Vertex& _t);
        bool isCollinearToVertex(const Vertex& v) const;
        std::int32_t getDistance() const;
    };

    Graph();
    Graph(std::uint32_t nVertices);

    Vertex getVertexWithCoords(const Vertex& v) const;
    Vertex addVertex(const Vertex& v);
    Vertex updateVertex(const Vertex& old, const Vertex& newVertex);
    void removeVertex(const Vertex& v);
    std::vector<Edge> getIncidentEdges(const Vertex& v) const;
    Vertex getVertex(std::int32_t index) const;
    std::uint32_t getVertexDegree(const Vertex& v) const;
    std::vector<Vertex> getVertices() const;
    std::uint32_t getNumberOfVertices() const;

    void addEdge(const Edge& e);
    void updateEdge(const Edge& old, const Edge& newEdge);
    void removeEdge(const Edge& e);
    Edge getEdge(std::int32_t e) const;
    Edge getEdge(std::int32_t s, std::int32_t t) const;
    std::vector<Edge> getEdges() const;
    std::vector<Edge> getCollinearEdgesWithVertex(const Vertex& v) const;
    std::uint32_t getNumberOfEdges() const;

    void printGraphviz(const std::string &filename) const;

    std::uint32_t getNumberOfComponents() const;
    bool areOnSameComponent(const Vertex& v1, const Vertex& v2);
    std::uint32_t getDistance() const;

    void print() const;
    void setup();

private:
    using BoostGraph = boost::adjacency_list<boost::setS, boost::vecS, boost::undirectedS, Vertex, Edge>;
    BoostGraph graph;

    std::int32_t distanceBalance;

    struct DisjointSetData
    {
        using Vertex = boost::graph_traits<BoostGraph>::vertex_descriptor;
        using VertexSizeType = boost::graph_traits<BoostGraph>::vertices_size_type;

        using Rank = VertexSizeType*;
        using Parent = Vertex*;

        std::vector<Vertex> parent;
        std::vector<VertexSizeType> rank;

        using DisjointSet = boost::disjoint_sets<Rank, Parent>;
        DisjointSet ds;

        const BoostGraph& graph;

        DisjointSetData(const BoostGraph& _graph);
        void setup();
    };

    DisjointSetData disjointSet;
};

}

#endif // GRAPH_H
