#include <iostream>
#include <string>
#include <vector>
#include <queue>

#include "Cube.hpp"

typedef std::vector<std::string> moveset_t;

// FIXME
// compiler error when pushing to std::queue
struct CubeState
{
  Cube cube;
  moveset_t solution;
};

moveset_t
getStartMoves();

CubeState
serialBFS(Cube c, const moveset_t& moves);

CubeState
serialBFS(std::queue<CubeState>& frontier, const moveset_t& moves);

int
main()
{
  std::cout << "Scramble (WCA notation) ==> ";
  std::string scramble;
  std::getline(std::cin, scramble);

//  std::cout << "Serial/Parallel (s/p)   ==> ";
//  std::string model;
//  std::cin >> model;
//
//  std::cout << "Algorithm (bfs, a*)     ==> ";
//  std::string algorithm;
//  std::cin >> algorithm;

  Cube c;
  c.scramble(scramble);
  std::cout << std::boolalpha << "Solved?: " << c.isSolved() << '\n';

  moveset_t startingMoves = getStartMoves();
  CubeState solved = serialBFS(c, startingMoves);

  for (const auto& m : solved.solution)
  {
    std::cout << m << ' ';
  }
  std::cout << '\n';
}

moveset_t
getStartMoves()
{
  std::string moveStr = "ULFRBD", variants = "2\'";
  std::vector<std::string> moves;

  for (size_t i = 0; i < moveStr.size(); ++i)
  {
    for (size_t j = 0; j < variants.size() + 1; ++j)
    {
      std::string m = moveStr.substr(i, 1);
      if (j > 0)
        m += variants[j - 1];
      moves.push_back(m);
    }
  }
  return moves;
}

CubeState
serialBFS(Cube c, const moveset_t& moves)
{
  std::queue<CubeState> frontier;
  for (const auto& m : moves)
  {
    Cube cCopy(c);
    c.move(m);
    
    moveset_t solution;
    solution.push_back(m);
    
    CubeState state;
    state.cube = c;
    state.solution = solution;
    frontier.push(state);
  }
  
  return serialBFS(frontier, moves);
}

CubeState
serialBFS(std::queue<CubeState>& frontier, const moveset_t& moves)
{
  CubeState currState;
  currState.cube = frontier.front().cube;
  currState.solution = frontier.front().solution;
  frontier.pop();

  if (currState.cube.isSolved())
    return currState;
  
  for (const auto& m : moves)
  {
    if (m[0] != currState.solution.back()[0])
    {
      CubeState currCopy;
      currCopy.cube = currState.cube;
      currCopy.solution = currState.solution;

      currCopy.cube.move(m);
      currCopy.solution.push_back(m);

      frontier.push(currCopy);
    }
  }

  return serialBFS(frontier, moves);
}
