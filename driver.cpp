#include <iostream>
#include <string>

#include "Cube.hpp"

int
main()
{
  std::cout << "Input a scramble (WCA notation): ";
  std::string scramble;
  std::getline(std::cin, scramble);

  Cube c;
  c.scramble(scramble);
  
  for (int i = 0; i < 6; ++i)
  {
    c.printSide(i);
    std::cout << '\n';
  }
}


