#include "modthomsonconstrutive.h"
#include <iostream>

st::ModThomsonConstrutive::VertexPair st::ModThomsonConstrutive::getMinDistanceVertices()
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
    this->vertexPairSet.clear();
    this->minDistanceHeap = MinimumDistanceSet();
    const auto l2 = this->graph.getVertices();

    for (const Vertex i : l2)
    {
        for (const Vertex j : l2 )
        {
            VertexPair temp = std::make_pair( i, j );
            vertexPairSet.insert(temp);
        }
    }

    for (const VertexPair p : vertexPairSet)
    {
        minDistanceHeap.push(p);
    }
}

void st::ModThomsonConstrutive::checkAndCrystalize(const Vertex& steinerNode)
{
    if (steinerNode.type == Vertex::Type::CORNER)
    {
        /* Cristalization to Steiner Nodes */
        if ( this->graph.getVertexDegree(steinerNode) > 2 )
        {
            Vertex temp = steinerNode;
            temp.type = Vertex::Type::STEINER;
            this->graph.updateVertex(steinerNode, temp);
        }
    }
    else if (steinerNode.type == Vertex::Type::STEINER)
    {
        /* Cristalization to Steiner Nodes */
        if ( this->graph.getVertexDegree(steinerNode) == 2 )
        {
            Vertex temp = steinerNode;
            temp.type = Vertex::Type::CORNER;
            this->graph.updateVertex(steinerNode, temp);
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
        /* Need to check the "crystalization" of corner node to steiner node*/
        this->graph.addEdge(edge);
        return;
    }

    if (pair.first.type == Vertex::Type::CORNER || pair.second.type == Vertex::Type::CORNER)
    {
        std::cout << "Vish" << pair.first.index << " " << pair.second.index << std::endl;
    }

    /* Need to make a corner */
    Vertex corner( pair.first.x, pair.second.y, Vertex::Type::CORNER );
    //corner = this->graph.getVertexWithCoords(corner);

    //if (corner.index == -1)
    //{
        corner = this->graph.addVertex(corner);
    //}

    connect({corner, pair.first});
    connect({corner, pair.second});
    //this->graph.addEdge(Graph::Edge(pair.first, corner));
    //this->graph.addEdge(Graph::Edge(pair.second, corner));
}

st::ModThomsonConstrutive::ModThomsonConstrutive(const st::Data &_data)
    : graph(_data.terminal.terminals.size()), vertexPairSet(), data(&_data)
{
    loadVertices();
    this->graph.setup();
}

st::Graph st::ModThomsonConstrutive::run()
{
    std::uint32_t components = this->graph.getNumberOfComponents();

    while (components > 1)
    {
        VertexPair pair = getMinDistanceVertices();

        if (this->graph.areOnSameComponent(pair.first, pair.second) == false)
        {
            connect(pair);
            //this->graph.addEdge(Graph::Edge(pair.first, pair.second));
        }

        components = this->graph.getNumberOfComponents();
    }

    this->graph.printGraphviz(1);

    return this->graph;
}
