#include "modthomsonconstrutive.h"

st::ModThomsonConstrutive::VertexPair st::ModThomsonConstrutive::getMinDistanceVertices()
{
    const VertexPair pair = *(this->set.begin());
    this->set.erase(this->set.begin());
    return pair;
}

void st::ModThomsonConstrutive::loadVertices()
{
    for (const auto index : this->data->terminal.terminals)
    {
        const st::Data::Vertex dv = this->data->vertices.at(index);
        Vertex v(dv);

        v.index = index;
        v = this->graph.updateVertex(v,v);

        for (const Vertex j : this->graph.getVertices() )
        {
            if (v == j)
            {
                continue;
            }

            set.insert(std::make_pair(v,j));
        }
    }
}

void st::ModThomsonConstrutive::connect(const st::ModThomsonConstrutive::VertexPair &pair)
{
    if ((pair.first.x == pair.second.x) || (pair.first.y == pair.second.y))
    {
        for (Vertex v1 : { pair.first, pair.second })
        {
            for (const Vertex j : this->graph.getVertices() )
            {
                VertexPair temp = std::make_pair( v1, j );
                auto it = set.find(temp);

                if ( it != set.end() )
                {
                    set.erase(it);
                }

                set.insert(temp);
            }
        }

        return;
    }

    Vertex corner( pair.first.x, pair.second.y );

    corner = this->graph.addVertex(corner);
    this->graph.addEdge(Graph::Edge(pair.first, corner));
    this->graph.addEdge(Graph::Edge(pair.second, corner));
}

st::ModThomsonConstrutive::ModThomsonConstrutive(const st::Data &_data)
    : graph(_data.terminal.terminals.size()), set(), data(&_data)
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
