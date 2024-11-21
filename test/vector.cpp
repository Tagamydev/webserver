#include <vector>
#include <iostream>
#include <string>

int main()
{
    std::vector<std::string>    lista;

    lista.push_back("hola");
    std::cout << lista.front() << std::endl;
}