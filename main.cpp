#include <iostream>
#include <fstream>
#include "data.h"
#include "graph.h"
#include "thomsonconstrutive.h"

int main(int argc, char* argv[])
{
    if (argc <= 1)
    {
        std::cout << "Error: Please inform options and filename." << std::endl;
        //TODO: printHelp(argv[0]);
        return 1;
    }

    std::string option = argv[1];

    if (option == "--test")
    {
        //TODO: testRun();
        return 0;
    }

    if (option == "--help")
    {
        //TODO: printHelp(argv[0]);
        return 0;
    }

    st::Data data;
    std::ifstream file(argv[1]);

    if (!file.is_open())
    {
        std::cout << "Error: cannot open file." << std::endl;
        //TODO: printHelp(argv[0]);
        return 0;
    }

    data.load(file);
//    data.print(std::cout);

    st::ThomsonConstrutive a(data);
    st::Graph tree = a.run();

    std::cout << tree.getDistance() << std::endl;
    tree.printGraphviz(data.comment.name + "_st.dot");


    return 0;
}
