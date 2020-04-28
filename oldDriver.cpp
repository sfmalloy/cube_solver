/* TODO - make this header better
 * Sean Malloy
 * solver.cpp
 * Rubik's Cube Solver - Project for CSCI 476
 */

/*****************************************************/
#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <future>
#include <mutex>

/*****************************************************/
#include "Cube.hpp"
#include "Timer.hpp"

/*****************************************************/
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

  Cube cube;
  moveset_t solution;
};

typedef std::deque<CubeState> bag_t;

/*****************************************************/
moveset_t
getStartMoves();

CubeState
serialBFS(Cube c, const moveset_t& moves);

CubeState
serialBFS(std::queue<CubeState>& frontier, const moveset_t& moves);

CubeState
parallelBFS(Cube c, const moveset_t& moves, unsigned p);

bag_t
parallelBFSHelper(const bag_t& frontier, const moveset_t& moves, unsigned p, unsigned tid);

CubeState
parallelBFS(bag_t& frontier, const moveset_t& moves, unsigned p);

size_t
partition(size_t len, unsigned p, unsigned tid);
/*****************************************************/
int
main()
{
  std::cout << "Scramble (WCA notation) ==> ";
  std::string scramble;
  std::getline(std::cin, scramble);

  std::cout << "Serial/Parallel (s/p) ==> ";
  std::string model;
  std::cin >> model;

  unsigned p;
  if (model == "p")
  {
    std::cout << "Number of threads ==> ";
    std::cin >> p;
  }

//  std::cout << "Algorithm (bfs, a*) ==> ";
//  std::string algorithm;
//  std::cin >> algorithm;

  Cube c;
  c.scramble(scramble);

  moveset_t startingMoves = getStartMoves();
  
  Timer t;
  CubeState solved;
//  if (model == "s")
//  {
    t.start();
    solved = serialBFS(c, startingMoves);
    t.stop();
//  }
//  else
//  {
//    t.start();
//    solved = parallelBFS(c, startingMoves, p);
//    t.stop();
//  }

  std::cout << "Time: "  << (t.elapsed() / 1000) << '\n';

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
      if (m[0] != frontier.front().solution.back()[0])
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

CubeState
parallelBFS(Cube c, const moveset_t& moves, unsigned p)
{
  if (c.isSolved())
    return CubeState();
  
  bag_t frontier;
  for (const auto& m : moves)
  {
    CubeState state(c);
    state.cube.move(m);
    state.solution.push_back(m);

    frontier.push_back(state);
  }
  
  std::vector<std::future<bag_t>> threads;
  for (unsigned tid = 0; tid < p; ++tid)
  {
    threads.push_back(std::async(std::launch::async, parallelBFSHelper, std::cref(frontier), std::cref(moves), p, tid));
  }
 
  bag_t newFrontier;
  for (auto& t : threads)
  {
    auto localFrontier = t.get();
    for (auto& state : localFrontier)
    {
      if (state.cube.isSolved())
        return state;
      newFrontier.push_back(state);
    }
  }

  return parallelBFS(newFrontier, moves, p);
}

CubeState
parallelBFS(bag_t& frontier, const moveset_t& moves, unsigned p)
{
  while (true)
  {
    std::vector<std::future<bag_t>> threads;
    for (unsigned tid = 0; tid < p; ++tid)
    {
      threads.push_back(std::async(std::launch::async, parallelBFSHelper, std::cref(frontier), std::cref(moves), p, tid));
    }
    
    bag_t newFrontier;
    for (auto& t : threads)
    {
      auto localFrontier = t.get();
      printf("%lu\n", localFrontier.size());
      for (auto& state : localFrontier)
      {
        if (state.cube.isSolved())
          return state;
        newFrontier.push_back(state);
      }
    }

    frontier = newFrontier;
  }
}

bag_t
parallelBFSHelper(const bag_t& frontier, const moveset_t& moves, unsigned p, unsigned tid)
{
  size_t begin = partition(frontier.size(), p, tid), end = partition(frontier.size(), p, tid + 1);
  bag_t localFrontier;
  
  for (size_t i = begin; i < end; ++i)
  {
    for (const auto& m : moves)
    {
      if (m[0] != frontier[i].solution.back()[0])
      {
        CubeState temp(frontier[i]);
        temp.cube.move(m);
        temp.solution.push_back(m);

        localFrontier.push_back(temp);
      }
    }
  }

  return localFrontier;
}

size_t
partition(size_t len, unsigned p, unsigned tid)
{
  return len * tid / p;
}
