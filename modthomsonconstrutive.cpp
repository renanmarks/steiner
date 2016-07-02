#include "modthomsonconstrutive.h"
#include <iostream>

st::ModThomsonConstrutive::VertexPair st::ModThomsonConstrutive::getMinDistanceVertices()
{
    if (this->vertexPairSet.size() > 0)
    {
        const VertexPair pair = minDistanceHeap.top();
        minDistanceHeap.pop();
        const auto it = this->vertexPairSet.find(pair);

        if (it != this->vertexPairSet.end() )
        {
            this->vertexPairSet.erase( it );
        }

        return pair;
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

    setupDistanceHeap(vertices);
}

void st::ModThomsonConstrutive::setupDistanceHeap( const std::vector<Vertex>& l)
{
    for (Vertex i : l)
    {
        for (const Vertex j : this->graph.getVertices() )
        {
            if (i == j)
            {
                continue;
            }

            VertexPair temp = std::make_pair( i, j );
            auto it = vertexPairSet.find(temp);
            if ( it != vertexPairSet.end() )
            {
                vertexPairSet.erase(it);
            }

            vertexPairSet.insert(temp);
        }
    }

    for (const VertexPair p : vertexPairSet)
    {
        minDistanceHeap.push(p);
    }
}

void st::ModThomsonConstrutive::connect(const st::ModThomsonConstrutive::VertexPair &pair)
{
    /* Collinear vertices */
    if ((pair.first.x == pair.second.x) || (pair.first.y == pair.second.y))
    {
        setupDistanceHeap({ pair.first, pair.second });

        /* Need to check the "crystalization" of corner node to steiner node*/

        this->graph.addEdge(Graph::Edge(pair.first, pair.second));

        return;
    }

    /* Need to make a corner */
    Vertex corner( pair.first.x, pair.second.y, Vertex::Type::CORNER );

    corner = this->graph.addVertex(corner);
    this->graph.addEdge(Graph::Edge(pair.first, corner));
    this->graph.addEdge(Graph::Edge(pair.second, corner));
}

st::ModThomsonConstrutive::ModThomsonConstrutive(const st::Data &_data)
    : graph(_data.terminal.terminals.size()), vertexPairSet(), data(&_data)
{
    loadVertices();
    this->graph.setup();
}

st::Graph st::ModThomsonConstrutive::run()
{
    while (this->graph.getNumberOfComponents() > 1)
    {
        VertexPair pair = getMinDistanceVertices();

        if (this->graph.areOnSameComponent(pair.first, pair.second) == false)
        {
            connect(pair);
        }
    }

    return this->graph;
}
