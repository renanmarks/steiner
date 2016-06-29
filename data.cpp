#include "data.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <regex>
#include <algorithm>

namespace
{
    std::vector<std::string> getMatches(std::istream &stream, std::string regexStr)
    {
        std::string fileLine;
        std::getline(stream, fileLine);

        /* Matches all the specification tags */
        std::regex specsRegex(regexStr);
        std::smatch specsMatches;

        /* Regex search */
        std::regex_search(fileLine, specsMatches, specsRegex);

        if (specsMatches.ready() && specsMatches.empty() && specsMatches.size() == 0)
        {
            return std::vector<std::string>();
        }

        return std::vector<std::string>(specsMatches.begin()+1, specsMatches.end());
    }

    std::string getString(std::istream &stream, std::string regexStr)
    {
        return getMatches(stream, regexStr)[1];
    }

    std::string getStringAndRewind(std::istream &stream, std::string regexStr)
    {
        std::string stringValue = getString(stream, regexStr);
        stream.seekg(0, stream.beg);

        return stringValue;
    }

    std::string toLower(const std::string& str)
    {
        std::string returnStr = str;

        std::transform(returnStr.begin(), returnStr.end(), returnStr.begin(), ::tolower);

        return returnStr;
    }
}

bool st::Data::parseHeader(std::istream &file)
{
    std::string header;
    std::getline(file, header);

    return (toLower(header) == toLower("33D32945 STP File, STP Format Version 1.00"));
}

void st::Data::parseCommentSection(std::istream &file)
{
    std::string line;

    for (std::getline(file, line);
         toLower(line) != "end";
         std::getline(file, line))
    {
        std::istringstream stream(line);
        std::vector<std::string> matches = getMatches(stream, "^\\s*(\\w*)\\s*\"(.*)\"\\s*$");
        stream.seekg(0, stream.beg);

        const std::string itemName  = toLower(matches[0]);
        const std::string itemValue = matches[1];

        if (itemName == "name")
        {
            this->comment.name = itemValue;
        }
        else if (itemName == "date")
        {
            this->comment.date = itemValue;
        }
        else if (itemName == "creator")
        {
            this->comment.creator = itemValue;
        }
        else if (itemName == "remark")
        {
            this->comment.remark = itemValue;
        }
        else if (itemName == "problem")
        {
            this->comment.problem = itemValue;
        }
    }

    return;
}

void st::Data::parseGraphSection(std::istream &file)
{
    std::string line;

    for (std::getline(file, line);
         toLower(line) != "end";
         std::getline(file, line))
    {
        std::istringstream stream(line);
        std::vector<std::string> matches = getMatches(stream, "^\\s*(\\w*)\\s*\"?(.*)\"?\\s*$");
        stream.seekg(0, stream.beg);

        const std::string itemName  = toLower(matches[0]);
        const std::string itemValue = matches[1];

        if (itemName == "obstacles")
        {
            this->graph.numberOfObstacles = std::stoi(itemValue);
        }
        else if (itemName == "nodes")
        {
            this->graph.numberOfNodes = std::stoi(itemValue);
            this->vertices.resize(this->graph.numberOfNodes);
        }
        else if (itemName == "edges")
        {
            this->graph.edges.reserve(std::stoi(itemValue));
        }
        else if (itemName == "arcs")
        {
            this->graph.arcs.reserve(std::stoi(itemValue));
        }
        else
        {
            matches = getMatches(stream, "^\\s*E\\s*(\\d+)\\s*(\\d+)\\s*(\\d+)\\s*$");

            std::uint32_t first = std::stoi(matches[0]);
            std::uint32_t second = std::stoi(matches[1]);
            std::uint32_t weight = std::stoi(matches[2]);

            if (itemName == "e")
            {
                this->graph.edges.push_back(Graph::Edge(first, second, weight));
            }
            else if (itemName == "a")
            {
                this->graph.arcs.push_back(Graph::Arc(first, second, weight));
            }
        }
    }

    return;
}

void st::Data::parseTerminalsSection(std::istream &file)
{
    std::string line;

    for (std::getline(file, line);
         toLower(line) != "end";
         std::getline(file, line))
    {
        std::istringstream stream(line);
        std::vector<std::string> matches = getMatches(stream, "^\\s*(\\w*)\\s*\"?(.*)\"?\\s*$");
        stream.seekg(0, stream.beg);

        const std::string itemName  = toLower(matches[0]);
        const std::string itemValue = matches[1];

        if (itemName == "terminals")
        {
            this->terminal.terminals.reserve(std::stoi(itemValue));
        }
        else if (itemName == "rootp")
        {
            this->terminal.rootP = std::stoi(itemValue);
        }
        else if (itemName == "root")
        {
            this->terminal.root = std::stoi(itemValue);
        }
        else if (itemName == "t")
        {
            this->terminal.terminals.push_back(std::stoi(itemValue));
        }
    }

    return;
}

void st::Data::parseCoordinatesSection(std::istream &file)
{
    std::string line;

    for (std::getline(file, line);
         toLower(line) != "end";
         std::getline(file, line))
    {
        std::istringstream stream(line);
        std::vector<std::string> matches = getMatches(stream, "^\\s*D+\\s*(\\d+)\\s*(\\d+)\\s*(\\d+)\\s*$");

        const std::uint32_t coordIndex  = std::stoi(matches[0]) - 1;
        const double coordX             = std::stod(matches[1]);
        const double coordY             = std::stod(matches[2]);

        this->vertices.at(coordIndex) = Vertex(coordX, coordY, 0.0);
    }

    return;
}

st::Data::Data()
{

}

void st::Data::load(std::istream &file)
{
    if (parseHeader(file) == false)
    {
        std::cout << "Error." << std::endl;
        return;
    }

    std::string line;

    for (std::getline(file, line);
         file.eof() == false;
         std::getline(file, line))
    {
        std::istringstream stream(line);
        std::vector<std::string> matches = getMatches(stream, "^\\s*([sS][eE][cC][tT][iI][oO][nN])\\s*(.*)\\s*$");
        stream.seekg(0, stream.beg);

        if (line.empty() || matches.size() == 0)
        {
            continue;
        }

        const std::string itemValue = toLower(matches[1]);

        if (itemValue == "comment")
        {
            parseCommentSection(file);
        }
        else if (itemValue == "graph")
        {
            parseGraphSection(file);
        }
        else if (itemValue == "terminals")
        {
            parseTerminalsSection(file);
        }
        else if (itemValue == "coordinates")
        {
            parseCoordinatesSection(file);
        }
    }
}

void st::Data::print(std::ostream &stream)
{
    stream << "Comments\n";
    this->comment.print(stream);
    stream << "Graph\n";
    this->graph.print(stream);
    stream << "Terminals\n";
    this->terminal.print(stream);
}

bool st::Data::operator==(const st::Data &other) const
{
    bool sameGraph = this->graph == other.graph;
    bool sameTerminals = this->terminal == other.terminal;
    bool sameVertices = this->vertices == other.vertices;

    return sameGraph && sameTerminals && sameVertices;
}

bool st::Data::operator!=(const st::Data &other) const
{
    return !(*this == other);
}

void st::Data::Comment::print(std::ostream& out) const
{
    out << "Name    : " << this->name    << '\n'
        << "Creator : " << this->creator << '\n'
        << "Date    : " << this->date    << '\n'
        << "Problem : " << this->problem << '\n'
        << "Remark  : " << this->remark  << '\n'
        << std::endl;
}

st::Data::Graph::Edge::Edge()
    : Edge(0,0,0)
{

}

st::Data::Graph::Edge::Edge(uint32_t _first, uint32_t _second, int32_t _weight)
    : first(_first), second(_second), weight(_weight)
{

}

st::Data::Graph::Graph()
    : numberOfObstacles(0), numberOfNodes(0)
{

}

void st::Data::Graph::print(std::ostream &out) const
{
    out << "Number of Edges     : " << this->edges.size() << '\n'
        << "Number of Nodes     : " << this->numberOfNodes << '\n'
        << "Number of Arcs      : " << this->arcs.size() << '\n'
        << "Number of Obstacles : " << this->numberOfObstacles << '\n'
        << std::endl;
}

bool st::Data::Graph::operator==(const st::Data::Graph &other) const
{

}

bool st::Data::Graph::operator!=(const st::Data::Graph &other) const
{

}

st::Data::Graph::Arc::Arc()
    : Arc(0,0,0)
{

}

st::Data::Graph::Arc::Arc(uint32_t _first, uint32_t _second, int32_t _weight)
    : first(_first), second(_second), weight(_weight)
{

}

st::Data::Terminal::Terminal()
    : rootP(0), root(0)
{

}

void st::Data::Terminal::print(std::ostream &out) const
{
    out << "Number of Terminals  : " << this->terminals.size() << '\n'
        << "Root                 : " << this->root << '\n'
        << "Root (Prize-Collect) : " << this->rootP << '\n'
        << std::endl;
}

bool st::Data::Terminal::operator==(const st::Data::Terminal &other) const
{

}

bool st::Data::Terminal::operator!=(const st::Data::Terminal &other) const
{

}

st::Data::Vertex::Vertex()
    : Vertex(0.0, 0.0, 0.0)
{

}

st::Data::Vertex::Vertex(double x, double y, double z)
    : Vertex(-1, x, y, z)
{

}

st::Data::Vertex::Vertex(int32_t i, double x, double y, double z)
    : index(i), values({x, y, z})
{

}

st::Data::Vertex::value_type st::Data::Vertex::operator[](size_t i) const
{
    return values.at(i);
}

bool st::Data::Vertex::operator==(const st::Data::Vertex &other) const
{
    return this->values == other.values;
}

bool st::Data::Vertex::operator!=(const st::Data::Vertex &other) const
{
    return !(*this == other);
}
