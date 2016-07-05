#include "modthomsonconstrutive.h"
#include <iostream>
#include <random>

st::ModThomsonConstrutive::VertexPair st::ModThomsonConstrutive::getMinDistanceVertices(const Vertex& v)
{
    setupDistanceHeap({v});

    if (this->vertexPairSet.size() > 0)
    {
        const VertexPair pair = minDistanceHeap.top();
        const auto it = this->vertexPairSet.find(pair);

        if (it != this->vertexPairSet.end() )
        {
            this->vertexPairSet.erase( it );
            minDistanceHeap.pop();
            return pair;
        }
    }

    return VertexPair(Vertex(), Vertex());
}

void st::ModThomsonConstrutive::loadVertices()
{
    /* Setup de vertices */
    for (const auto index : this->data->terminal.terminals)
    {
        const st::Data::Vertex dv = this->data->vertices.at(index);
        Vertex v(dv);

        v.index = index;
        v = this->graph.updateVertex(v,v);
    }

    /* Setup the initial distance heap */
    std::vector<Vertex> vertices;
    const auto terminals = this->data->terminal.terminals;

    for (const auto& t : terminals)
    {
        vertices.push_back(this->graph.getVertex(t));
    }
}

void st::ModThomsonConstrutive::setupDistanceHeap( const std::vector<Vertex>& l)
{
    std::vector<Vertex> l2;

    for (const auto& v : this->graph.getVertices())
    {
        if (this->graph.getVertexDegree(v) == 0)
        {
            l2.push_back(v);
        }
    }

    for (const Vertex i : l)
    {
        for (const Vertex j : l2 )
        {
            if (i == j)
            {
                continue;
            }

            VertexPair temp = std::make_pair( i, j );

            if (vertexPairSet.find(temp) == vertexPairSet.end())
            {
                vertexPairSet.insert(temp);
                minDistanceHeap.push(temp);
            }
        }
    }
}

void st::ModThomsonConstrutive::checkAndCrystalize(const Vertex& steinerNode)
{
    Vertex updated = this->graph.getVertex(steinerNode.index);

    if (updated.type == Vertex::Type::CORNER)
    {
        /* Cristalization to Steiner Nodes */
        if ( this->graph.getVertexDegree(updated) > 2 )
        {
            Vertex temp = updated;
            temp.type = Vertex::Type::STEINER;
            this->graph.updateVertex(updated, temp);
        }
    }
    else if (updated.type == Vertex::Type::STEINER)
    {
        /* Cristalization to Steiner Nodes */
        if ( this->graph.getVertexDegree(updated) == 2 )
        {
            Vertex temp = updated;
            temp.type = Vertex::Type::CORNER;
            this->graph.updateVertex(updated, temp);
        }
    }
}

void st::ModThomsonConstrutive::connect(const st::ModThomsonConstrutive::VertexPair &pair)
{
    /* Collinear vertices */
    if ((pair.first.x == pair.second.x) || (pair.first.y == pair.second.y))
    {
        setupDistanceHeap({ pair.first, pair.second });
        Graph::Edge edge(pair.first, pair.second);

        this->graph.addEdge(edge);

        /* Need to check the "crystalization" of corner node to steiner node*/
        checkAndCrystalize(pair.first);
        checkAndCrystalize(pair.second);

        return;
    }

    /* Need to make a corner */
    Vertex corner( pair.first.x, pair.second.y, Vertex::Type::CORNER );
    corner = this->graph.addVertex(corner);

    connect({corner, pair.first});
    connect({corner, pair.second});
}

st::ModThomsonConstrutive::ModThomsonConstrutive(const st::Data &_data)
    : graph(_data.terminal.terminals.size()), vertexPairSet(), data(&_data)
{
    loadVertices();
}

st::Graph::Vertex st::ModThomsonConstrutive::getRandomVertex() const
{
    // First node
    std::random_device rd;
    std::mt19937 mt(rd());
    std::vector<st::Graph::Vertex> vertices = this->graph.getVertices();
    std::vector<st::Graph::Vertex> l2;

    for (const auto& v : vertices)
    {
        std::int32_t degree = this->graph.getVertexDegree(v);

        if ( degree > 0 && degree < 2 )
        {
            l2.push_back(v);
        }
    }

    std::uniform_int_distribution<> dist(0, l2.size()-1);
    return l2.at(dist(mt));
}

st::Graph st::ModThomsonConstrutive::run()
{
    for (Graph::Edge e : this->graph.getEdges() )
    {
        this->graph.removeEdge(e);
    }

    this->graph.setup();

    // First node
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<> dist(0, this->graph.getNumberOfVertices()-1);
    Vertex v = this->graph.getVertex(dist(mt));

    while (this->graph.getNumberOfComponents() > 1)
    {
        VertexPair pair = getMinDistanceVertices(v);

        if (this->graph.areOnSameComponent(pair.first, pair.second) == false)
        {
            connect(pair);
        }

        v = this->getRandomVertex();
    }

    return this->graph;
}
