#include "modthomsonconstrutive.h"

st::ModThomsonConstrutive::VertexPair st::ModThomsonConstrutive::getMinDistanceVertices()
{
    const VertexPair pair = this->heap.top();
    this->heap.pop();
    return pair;
}

void st::ModThomsonConstrutive::loadVertices()
{
    for (const auto index : this->data->terminal.terminals)
    {
        const st::Data::Vertex dv = this->data->vertices.at(index);
        Vertex v(dv);

        v = this->graph.addVertex(v);

        for (const Vertex j : this->graph.getVertices() )
        {
            if (v == j)
            {
                continue;
            }

            const auto pair = std::make_pair(v,j);

            this->heap.push(pair);
        }
    }
}

st::ModThomsonConstrutive::ModThomsonConstrutive(const st::Data &_data)
    : graph(), heap(), data(&_data)
{
    loadVertices();

    std::int32_t numberOfComponents = this->graph.getNumberOfVertices();
}

st::Graph st::ModThomsonConstrutive::run()
{
    return this->graph;
}
