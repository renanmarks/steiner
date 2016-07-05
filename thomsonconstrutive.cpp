#include "thomsonconstrutive.h"
#include <iostream>
#include <random>

st::ThomsonConstrutive::VertexPair st::ThomsonConstrutive::getRandomDistanceVertices()
{
    if (this->vertexPairSet.size() > 0)
    {
        // First node
        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_int_distribution<> dist(0, minDistanceHeap.size()-1);
        std::int32_t value = dist(mt);
        VertexPair pair;

        for (std::int32_t i = 0; i <= value; i++)
        {
            pair = getMinDistanceVertices();
        }

        return pair;
    }

    return VertexPair(Vertex(), Vertex());
}

st::ThomsonConstrutive::VertexPair st::ThomsonConstrutive::getMinDistanceVertices()
{
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

void st::ThomsonConstrutive::loadVertices()
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

    setupDistanceHeap(vertices);
}

void st::ThomsonConstrutive::setupDistanceHeap( const std::vector<Vertex>& l)
{
    const auto l2 = this->graph.getVertices();

    for (const Vertex i : l)
    {
        for (const Vertex j : l2 )
        {
            VertexPair temp = std::make_pair( i, j );

            if (vertexPairSet.find(temp) == vertexPairSet.end())
            {
                vertexPairSet.insert(temp);
                minDistanceHeap.push(temp);
            }
        }
    }
}

void st::ThomsonConstrutive::checkAndCrystalize(const Vertex& steinerNode)
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

void st::ThomsonConstrutive::connect(const st::ThomsonConstrutive::VertexPair &pair)
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

st::ThomsonConstrutive::ThomsonConstrutive(const st::Data &_data)
    : ThomsonConstrutive(_data, false)
{

}

st::ThomsonConstrutive::ThomsonConstrutive(const st::Data &_data, bool _randomStart)
    : graph(_data.terminal.terminals.size()), vertexPairSet(), data(&_data), randomStart(_randomStart)
{
    loadVertices();
}

st::Graph st::ThomsonConstrutive::run()
{
    for (Graph::Edge e : this->graph.getEdges() )
    {
        this->graph.removeEdge(e);
    }

    this->graph.setup();

    while (this->graph.getNumberOfComponents() > 1)
    {
        VertexPair pair;

        if (this->randomStart == false)
        {
            pair = getMinDistanceVertices();
        }
        else
        {
            pair = getRandomDistanceVertices();
            this->setupDistanceHeap(this->graph.getVertices());
            this->randomStart = false;
        }

        if (this->graph.areOnSameComponent(pair.first, pair.second) == false)
        {
            connect(pair);
        }
    }

    return this->graph;
}
