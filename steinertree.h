#ifndef STEINERTREE_H
#define STEINERTREE_H

#include <cstdint>
#include <vector>
#include <ostream>
#include "data.h"

namespace st
{

class RectilinearSteinerTree
{
private:
    std::uint32_t distance;
    std::size_t numberOfEdges;
    const st::Data* data;
    using AdjacencyList = std::vector<std::vector<std::uint32_t>>;
    AdjacencyList adjacencyList;

public:

    struct Edge
    {
        std::uint32_t first;
        std::uint32_t second;

        Edge() = default;
        Edge(std::uint32_t f, std::uint32_t s);
        bool isValid() const;
        Edge reverse() const;
        bool isReverseOf(const st::RectilinearSteinerTree::Edge& rhs) const;
        bool hasVertexOf(const st::RectilinearSteinerTree::Edge& rhs) const;
        bool operator() (const st::RectilinearSteinerTree::Edge& rhs) const;
        bool operator== (const st::RectilinearSteinerTree::Edge& rhs) const;
        bool operator!= (const st::RectilinearSteinerTree::Edge& rhs) const;

    };

    RectilinearSteinerTree(const st::Data& _data);

    void insertEdge(const st::Data::Vertex& first, const st::Data::Vertex& second);
    void insertEdge(const Edge& edge);

    void eraseEdge(const st::Data::Vertex& first, const st::Data::Vertex& second);
    void eraseEdge(const Edge& edge);

    bool haveEdge(const st::Data::Vertex& first, const st::Data::Vertex& second) const;
    bool haveEdge(const Edge& edge) const;

    bool isValid() const;

    std::size_t getNumberOfEdges() const;
    Edge getEdge(const std::size_t i) const;
    std::vector<Edge> getEdges() const;

    void print(std::ostream& out) const;

    std::uint32_t getDistance() const;
    void setDistance(const std::uint32_t &value);

    bool operator== (const st::RectilinearSteinerTree& rhs) const;
    bool operator!= (const st::RectilinearSteinerTree& rhs) const;
};

std::ostream& operator<<(std::ostream& os, const st::RectilinearSteinerTree::Edge& e);

}

#endif // STEINERTREE_H
