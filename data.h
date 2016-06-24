#ifndef DATA_H
#define DATA_H

#include <string>
#include <vector>
#include <array>

namespace st
{

class Data
{
private:
    struct Comment
    {
        std::string name;
        std::string date;
        std::string creator;
        std::string remark;
        std::string problem;

        void print(std::ostream &out) const;
    } comment;

    struct Graph
    {
        struct Edge
        {
            std::uint32_t first;
            std::uint32_t second;
        };

        struct Arc
        {
            std::uint32_t first;
            std::uint32_t second;
            std::uint32_t weidght;
        };

        std::uint32_t numberOfObstacles;
        std::uint32_t numberOfNodes;
        std::uint32_t numberOfEdges;
        std::uint32_t numberOfArcs;
        std::vector<Edge> edges;
        std::vector<Arc> arcs;
    } graph;

    struct Terminal
    {
        std::uint32_t numberOfTerminals;
        std::uint32_t rootP;
        std::uint32_t root;
        std::vector<std::uint32_t> nodes;
    } terminal;

    struct Coordinates
    {
        struct Vertex
        {
            std::array<double, 3> values;
        };

        std::vector<Vertex> vertices;
    };

    bool parseHeader(std::istream &file);
    void parseCommentSection(std::istream &file);
    void parseGraphSection(std::istream &file);
    void parseTerminalsSection(std::istream &file);
//TODO:    void parseMaximumDegressSection();
    void parseCoordinatesSection(std::istream &file);

public:
    Data();

    void load(std::istream& file);
    void print(std::ostream& stream);
};

}

#endif // DATA_H
