#include "steinertree.h"
#include <algorithm>
#include <cmath>
#include <set>
#include <boost/functional/hash.hpp>

namespace
{
uint32_t roundNearestInt(double d)
{
    return static_cast<uint32_t>(d+0.5);
}

uint32_t distanceManhattan2D(const st::Data::Vertex& c0, const st::Data::Vertex& c1)
{
    double xd = std::abs(c0.values.at(0) - c1.values.at(0));
    double yd = std::abs(c0.values.at(1) - c1.values.at(1));

    return roundNearestInt(xd + yd);
}
}

st::RectilinearSteinerTree::RectilinearSteinerTree(const Data &_data)
    : distance(0), numberOfEdges(0), data(&_data)
{
    this->adjacencyList.resize(_data.vertices.size());
}

void st::RectilinearSteinerTree::insertEdge(const st::Data::Vertex &first, const st::Data::Vertex &second)
{
    auto listBegin = this->adjacencyList.cbegin();
    auto listEnd = this->adjacencyList.cend();

    if ((listBegin + first.index >= listEnd) || (listBegin + second.index >= listEnd))
    {
        return;
    }

    auto& list1 = this->adjacencyList.at(first.index);
    auto& list2 = this->adjacencyList.at(second.index);
    list1.push_back(second.index);
    list2.push_back(first.index);
    std::sort(list1.begin(), list1.end());
    std::sort(list2.begin(), list2.end());

    std::uint32_t distance = distanceManhattan2D(first, second);

    this->setDistance(this->getDistance() + distance);
    ++(this->numberOfEdges);
}

void st::RectilinearSteinerTree::insertEdge(const st::RectilinearSteinerTree::Edge &edge)
{
    const auto& first = this->data->vertices.at(edge.first);
    const auto& second = this->data->vertices.at(edge.second);

    this->insertEdge(first, second);
}

void st::RectilinearSteinerTree::eraseEdge(const st::Data::Vertex &first, const st::Data::Vertex &second)
{
    auto listBegin = this->adjacencyList.cbegin();
    auto listEnd = this->adjacencyList.cend();

    if ((listBegin + first.index == listEnd) || (listBegin + second.index > listEnd))
    {
        return;
    }

    auto& list  = this->adjacencyList.at(first.index);
    auto& list2 = this->adjacencyList.at(second.index);
    auto it     = std::find(std::begin(list), std::end(list), second.index);
    auto it2    = std::find(std::begin(list2), std::end(list2), first.index);

    if (it != list.end())
    {
        list.erase(it);
        std::sort(list.begin(), list.end());
    }

    if (it2 != list2.end())
    {
        list2.erase(it2);
        std::sort(list2.begin(), list2.end());
    }

    std::uint32_t distance = distanceManhattan2D(first, second);

    this->setDistance(this->getDistance() - distance);
    --(this->numberOfEdges);
}

void st::RectilinearSteinerTree::eraseEdge(const st::RectilinearSteinerTree::Edge &edge)
{
    const auto& first = this->data->vertices.at(edge.first);
    const auto& second = this->data->vertices.at(edge.second);

    this->eraseEdge(first, second);
}

bool st::RectilinearSteinerTree::haveEdge(const st::Data::Vertex &first, const st::Data::Vertex &second) const
{
    return this->haveEdge(st::RectilinearSteinerTree::Edge(first.index, second.index));
}

bool st::RectilinearSteinerTree::haveEdge(const st::RectilinearSteinerTree::Edge &edge) const
{
    const auto& list1 = this->adjacencyList.at(edge.first);
    const auto& list2 = this->adjacencyList.at(edge.second);

    const auto it1 = std::find(list1.begin(), list1.end(), edge.second);
    const auto it2 = std::find(list2.begin(), list2.end(), edge.first);

    bool oneway = (it1 != list1.end());
    bool otherway = (it2 != list2.end());

    return oneway && otherway;
}

bool st::RectilinearSteinerTree::isValid() const
{
    st::RectilinearSteinerTree adjList = *this;
    std::set<std::uint32_t> vertices;
    std::uint32_t first = 0;
    std::uint32_t second = 0;
    std::uint32_t zeroInsertions = 0;

    while (adjList.getNumberOfEdges() > 0)
    {
        second = adjList.adjacencyList.at(first).back();
        Edge edge(first,second);

        if (
             ( (second == 0) && (zeroInsertions > 1) && (vertices.find(second) != vertices.end()) ) ||
             ( (second != 0) && (vertices.find(second) != vertices.end()) )
           )
        {
            return false;
        }

        vertices.insert(second);

        if (second == 0)
        {
            ++zeroInsertions;
        }

        first = second;
        second = 0;
        adjList.eraseEdge(edge);
    }

    return true;
}

std::size_t st::RectilinearSteinerTree::getNumberOfEdges() const
{
    return this->numberOfEdges;
}

st::RectilinearSteinerTree::Edge st::RectilinearSteinerTree::getEdge(const std::size_t index) const
{
    return this->getEdges().at(index);
}

std::vector<st::RectilinearSteinerTree::Edge> st::RectilinearSteinerTree::getEdges() const
{
    std::vector<std::vector<bool>> adjacencyMatrix;
    std::vector<Edge> returnEdges;
    const std::size_t numberOfVertices = this->adjacencyList.size();

    returnEdges.reserve(this->numberOfEdges);
    adjacencyMatrix.resize(numberOfVertices);

    for (auto& line : adjacencyMatrix)
    {
        line.resize(numberOfVertices);
    }

    for (std::uint32_t first = 0; first < numberOfVertices; first++)
    {
        const auto& list = this->adjacencyList.at(first);

        for (std::uint32_t second : list)
        {
            Edge edge(first, second);

            if (edge.isValid() &&
                    adjacencyMatrix[first][second] == false &&
                    adjacencyMatrix[second][first] == false)
            {
                adjacencyMatrix[first][second] = true;
                adjacencyMatrix[second][first] = true;

                returnEdges.push_back(edge);
            }
        }
    }

    return returnEdges;
}

std::uint32_t st::RectilinearSteinerTree::getDistance() const
{
    return distance;
}

void st::RectilinearSteinerTree::setDistance(const std::uint32_t &value)
{
    distance = value;
}

bool st::RectilinearSteinerTree::operator==(const st::RectilinearSteinerTree &rhs) const
{
    return this->adjacencyList == rhs.adjacencyList && this->distance == rhs.distance && *(this->data) == *(rhs.data);
}

bool st::RectilinearSteinerTree::operator!=(const st::RectilinearSteinerTree &rhs) const
{
    return !(*this == rhs);
}

void st::RectilinearSteinerTree::print(std::ostream &out) const
{
    out << "Distance : " << this->distance << std::endl;

    out << "Edges(" << this->getNumberOfEdges() << ") : ";
    for (const auto& pair: this->getEdges())
    {
        out << pair << ",";
    }
    out << std::endl;
}

st::RectilinearSteinerTree::Edge::Edge(std::uint32_t f, std::uint32_t s)
    :first(f), second(s)
{

}

bool st::RectilinearSteinerTree::Edge::operator==(const st::RectilinearSteinerTree::Edge &rhs) const
{
    return ((this->first == rhs.first) && (this->second == rhs.second)) || ((this->first == rhs.second) && (this->second == rhs.first));
}

bool st::RectilinearSteinerTree::Edge::operator!=(const st::RectilinearSteinerTree::Edge &rhs) const
{
    return !(*this == rhs);
}

std::ostream & st::operator<<(std::ostream &os, const st::RectilinearSteinerTree::Edge &e)
{
    os << e.first << "-" << e.second;

    return os;
}

bool st::RectilinearSteinerTree::Edge::isValid() const
{
    return this->first != this->second;
}

st::RectilinearSteinerTree::Edge st::RectilinearSteinerTree::Edge::reverse() const
{
    return Edge(this->second, this->first);
}

bool st::RectilinearSteinerTree::Edge::isReverseOf(const st::RectilinearSteinerTree::Edge &rhs) const
{
    return *this == rhs.reverse();
}

bool st::RectilinearSteinerTree::Edge::hasVertexOf(const st::RectilinearSteinerTree::Edge &rhs) const
{
    return this->first == rhs.first || this->first == rhs.second || this->second == rhs.first || this->second == rhs.second;
}

std::size_t st::RectilinearSteinerTree::EdgeHasher::operator()(const st::RectilinearSteinerTree::Edge &rhs) const
{
    using boost::hash_value;
    using boost::hash_combine;

    std::size_t seed = 0;

    hash_combine(seed, hash_value(rhs.first));
    hash_combine(seed, hash_value(rhs.second));

    return seed;
}
