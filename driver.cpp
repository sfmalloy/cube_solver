#include <iostream>
#include <string>
#include <vector>
#include <queue>

#include "Cube.hpp"

typedef std::vector<std::string> moveset_t;

struct CubeState
{
  // default ctor
  CubeState()
    : cube(),
      solution()
  { }
  
  CubeState(const Cube& c)
    : cube(c),
      solution()
  { }
  
  CubeState(const Cube& c, const moveset_t& s)
    : cube(c),
      solution(s)
  { }
  
  // copy ctor
  CubeState(const CubeState& other)
    : cube(other.cube),
      solution(other.solution)
  { }

//  CubeState& operator=(CubeState const& other)
//  {
//    if (&other == this) 
//      return *this;
//    
//    cube = other.cube;
//    solution = other.solution;
//    
//    return *this;
//  }
  
  Cube cube;
  moveset_t solution;
};

moveset_t
getStartMoves();

CubeState
serialBFS(Cube c, const moveset_t& moves);

CubeState
serialBFS(std::queue<CubeState>& frontier, const moveset_t& moves);

bool
isLooping(const std::string& move, const moveset_t& solution);

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

  moveset_t startingMoves = getStartMoves();
  CubeState solved = serialBFS(c, startingMoves);

  for (const auto& m : solved.solution)
    std::cout << m << ' ';
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
  if (c.isSolved())
    return CubeState();
  std::queue<CubeState> frontier;
  for (const auto& m : moves)
  {
    CubeState state(c);
    state.cube.move(m);
    state.solution.push_back(m);

    frontier.push(state);
  }

  return serialBFS(frontier, moves);
}

CubeState
serialBFS(std::queue<CubeState>& frontier, const moveset_t& moves)
{
  if (frontier.front().cube.isSolved())
    return frontier.front();
  
  while (!frontier.front().cube.isSolved())
  {
    for (const auto& m : moves)
    {
      if (!isLooping(m, frontier.front().solution))
      {
        CubeState temp(frontier.front());
        temp.cube.move(m);
        temp.solution.push_back(m);

        frontier.push(temp);
      }
    }

    frontier.pop();
  }
  
  return frontier.front();
}

bool
isLooping(const std::string& move, const moveset_t& solution)
{
  if (solution.size() < 3 || move[0] != solution.back()[0])
    return false;
  
  size_t begin = solution.size() - 1, end = begin - 3;
  bool looping = true;
  for (size_t i = begin; i > end; --i)
    looping &= solution[i][0] == solution[i - 1][0];

  return looping;
}
