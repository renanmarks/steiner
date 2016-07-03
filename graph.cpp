#include <algorithm>
#include <iostream>
#include <sstream>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/incremental_components.hpp>
#include <boost/graph/graphviz.hpp>
#include "graph.h"

namespace
{
uint32_t roundNearestInt(double d)
{
    return static_cast<uint32_t>(d+0.5);
}

uint32_t distanceManhattan2D(const st::Graph::Vertex& c0, const st::Graph::Vertex& c1)
{
    double xd = std::abs(c0.x - c1.x);
    double yd = std::abs(c0.y - c1.y);

    return roundNearestInt(xd + yd);
}

st::Graph::Edge::Type getType(const st::Graph::Vertex& c0, const st::Graph::Vertex& c1)
{
    if ((c0.x == c1.x) && (c0.y != c1.y))
    {
        return st::Graph::Edge::Type::VERTICAL;
    }

    if ((c0.x != c1.x) && (c0.y == c1.y))
    {
        return st::Graph::Edge::Type::HORIZONTAL;
    }

    return st::Graph::Edge::Type::NONE;
}
}

st::Graph::Graph()
    : Graph(0)
{

}

st::Graph::Graph(uint32_t nVertices)
    : graph(nVertices), distanceBalance(0), disjointSet(graph)
{

}

namespace
{
struct EqualVertex : public std::equal_to<st::Graph::Vertex>
{
    bool operator()(const st::Graph::Vertex &lhs, const st::Graph::Vertex &rhs) const
    {
        return ((lhs.x == rhs.x) && (lhs.y == rhs.y));
    }
};
}

st::Graph::Vertex st::Graph::getVertexWithCoords(const st::Graph::Vertex &v) const
{
    const auto vertices = this->getVertices();
    auto const it = std::find_if(vertices.begin(), vertices.end(), [v](const st::Graph::Vertex& v2) { return EqualVertex()(v, v2); });

    if (it == vertices.end())
    {
        return v;
    }

    return *it;
}

st::Graph::Vertex st::Graph::addVertex(const st::Graph::Vertex &v)
{
    const auto s = boost::add_vertex(v, this->graph);
    auto obj = v;

    obj.index = s;
    this->graph[s] = obj;

    this->setup();

    return obj;
}

st::Graph::Vertex st::Graph::updateVertex(const st::Graph::Vertex &old, const st::Graph::Vertex &newVertex)
{
    const auto s = boost::vertex(old.index, this->graph);
    auto v = newVertex;
    v.index = s;
    this->graph[s] = v;

    return this->graph[s];
}

void st::Graph::removeVertex(const st::Graph::Vertex &v)
{
    const auto s = boost::vertex(v.index, this->graph);

    boost::remove_vertex(s, this->graph);

    this->setup();
}

st::Graph::Vertex st::Graph::getVertex(int32_t index) const
{
    const auto s = boost::vertex(index, this->graph);

    return this->graph[s];
}

uint32_t st::Graph::getVertexDegree(const st::Graph::Vertex &v) const
{
    const auto vd = boost::vertex(v.index, this->graph);
    return boost::degree(vd, this->graph);
}

std::vector<st::Graph::Vertex> st::Graph::getVertices() const
{
    std::vector<st::Graph::Vertex> returnVerts;

    const auto iterators = boost::vertices(this->graph);

    for (auto i = iterators.first; i != iterators.second; i++)
    {
        const auto v = this->graph[*i];

        returnVerts.push_back(v);
    }

    return returnVerts;
}

uint32_t st::Graph::getNumberOfVertices() const
{
    return boost::num_vertices(this->graph);
}

void st::Graph::addEdge(const st::Graph::Edge &e)
{
    const auto sd = boost::vertex(e.source.index, this->graph);
    const auto td = boost::vertex(e.target.index, this->graph);

    const auto ed = boost::add_edge(sd, td, this->graph);
    this->graph[ed.first] = e;
    this->disjointSet.ds.union_set(sd, td);
}

void st::Graph::updateEdge(const st::Graph::Edge &old, const st::Graph::Edge &newEdge)
{
    this->removeEdge(old);
    this->addEdge(newEdge);
}

void st::Graph::removeEdge(const st::Graph::Edge &e)
{
    const auto sd = boost::vertex(e.source.index, this->graph);
    const auto td = boost::vertex(e.target.index, this->graph);

    const auto ed = boost::edge(sd, td, this->graph);

    if (ed.second == true)
    {
        boost::remove_edge(sd, td, this->graph);
        this->setup();
    }
}

std::vector<st::Graph::Edge> st::Graph::getEdges() const
{
    std::vector<st::Graph::Edge> returnEdges;

    const auto iterators = boost::edges(this->graph);

    for (auto i = iterators.first; i != iterators.second; i++)
    {
        const auto v = this->graph[*i];

        returnEdges.push_back(v);
    }

    return returnEdges;
}

std::vector<st::Graph::Edge> st::Graph::getCollinearEdgesWithVertex(const st::Graph::Vertex &v) const
{
    std::vector<st::Graph::Edge> returnEdges;

    const auto iterators = boost::edges(this->graph);

    for (auto i = iterators.first; i != iterators.second; i++)
    {
        const Edge& e = this->graph[*i];

        if (e.isCollinearToVertex(v))
        {
            returnEdges.push_back(e);
        }
    }

    return returnEdges;
}

uint32_t st::Graph::getNumberOfEdges() const
{
    return boost::num_edges(this->graph);
}

void st::Graph::printGraphviz(const std::string& filename) const
{
    std::ofstream f(filename);

    using VD = BoostGraph::vertex_descriptor;

    boost::write_graphviz(f, this->graph, [this](std::ostream& out, const VD& v) {
        const Vertex vertex = this->graph[v];

        out << "[pos=\"" << vertex.x << "," << vertex.y << "!\",";

        switch(vertex.type)
        {
        default:
        case Vertex::Type::NODE:
            out << "shape=\"square\"]";
            break;
        case Vertex::Type::CORNER:
            out << "shape=\"diamond\"]";
            break;
        case Vertex::Type::STEINER:
            out << "shape=\"circle\"]";
            break;
        };
    });

    f.flush();
}

uint32_t st::Graph::getNumberOfComponents() const
{
    using Components = boost::component_index<DisjointSetData::Vertex>;
    Components components(this->disjointSet.parent.begin(), this->disjointSet.parent.end());
    return components.size();
}

bool st::Graph::areOnSameComponent(const st::Graph::Vertex &v1, const st::Graph::Vertex &v2)
{
    return (this->disjointSet.ds.find_set(v1.index) == this->disjointSet.ds.find_set(v2.index));
}

void st::Graph::print() const
{
    std::int32_t distance = 0;

    for (const auto e : this->getEdges())
    {
        distance += e.getDistance();
    }

    std::cout << "Distance   : " << distance << std::endl;

    std::cout << "Vertices   : ";
    boost::print_vertices(this->graph, get(&st::Graph::Vertex::index, this->graph));

    std::cout << "Edges      : ";
    boost::print_edges(this->graph, get(&st::Graph::Vertex::index, this->graph));

    std::cout << "Components : " << getNumberOfComponents() << std::endl;

    std::cout << "Graph      : " << std::endl;
    boost::print_graph(this->graph);
}

void st::Graph::setup()
{
    this->disjointSet.setup();
}


st::Graph::Vertex::Vertex()
    : Vertex(0, 0)

{

}

st::Graph::Vertex::Vertex(const st::Data::Vertex &v)
    : Vertex(v.values[st::Data::Vertex::x], v.values[st::Data::Vertex::y])
{

}

st::Graph::Vertex::Vertex(int32_t _x, int32_t _y)
    : Vertex(_x, _y, Type::NODE)
{

}

st::Graph::Vertex::Vertex(int32_t _x, int32_t _y, st::Graph::Vertex::Type _t)
    : index(-1), x(_x), y(_y), type(_t)
{

}

int32_t st::Graph::Vertex::distanceTo(const st::Graph::Vertex &other) const
{
    return distanceManhattan2D(*this, other);
}

bool st::Graph::Vertex::operator==(const st::Graph::Vertex &other) const
{
    bool sameX = this->x == other.x;
    bool sameY = this->y == other.y;
    bool sameI = this->index == other.index;

    return sameX && sameY && sameI;
}

bool st::Graph::Vertex::operator!=(const st::Graph::Vertex &other) const
{
    return !(*this == other);
}

st::Graph::Edge::Edge()
    : Edge(Vertex(), Vertex())
{

}

st::Graph::Edge::Edge(const st::Graph::Vertex &_s, const st::Graph::Vertex &_t)
    : source(_s), target(_t), type(getType(_s, _t))
{

}

bool st::Graph::Edge::isCollinearToVertex(const st::Graph::Vertex &v) const
{
    if (this->type == Edge::Type::NONE)
    {
        return false;
    }

    if (this->type == Edge::Type::HORIZONTAL)
    {
        const auto minmax = std::minmax(this->source.x, this->target.x);

        return ((minmax.first < v.x) && (v.x < minmax.second));
    }

    const auto minmax = std::minmax(this->source.y, this->target.y);

    return ((minmax.first < v.y) && (v.y < minmax.second));
}

int32_t st::Graph::Edge::getDistance() const
{
    return distanceManhattan2D(this->source, this->target);
}

st::Graph::DisjointSetData::DisjointSetData(const BoostGraph &_graph)
    : rank(boost::num_vertices(_graph)), parent(boost::num_vertices(_graph)), ds(&rank[0], &parent[0]), graph(_graph)
{
    boost::initialize_incremental_components(_graph, ds);
    boost::incremental_components(_graph, ds);
}

void st::Graph::DisjointSetData::setup()
{
    rank.resize(boost::num_vertices(this->graph));
    parent.resize(boost::num_vertices(this->graph));

    ds = DisjointSet(&rank[0], &parent[0]);

    boost::initialize_incremental_components(this->graph, ds);
    boost::incremental_components(this->graph, ds);
}
