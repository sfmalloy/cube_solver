/*
 * Sean Malloy
 * CSCI 476 - Project
 * 3x3x3 Cube Solver using BFS and A*
 */
/************************************************/
// System includes
#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <mutex>
#include <future>

/************************************************/
// Local includes
#include "Cube.hpp"
#include "Timer.hpp"

/************************************************/
// Typedefs/structs
typedef std::vector<std::string> moveset_t;

struct CubeState
{
  CubeState()
    : cube(),
      solution()
  { }

  CubeState(const Cube& otherCube)
    : cube(otherCube),
      solution()
  { }

  CubeState(const CubeState& state)
    : cube(state.cube),
      solution(state.solution)
  { }

  bool
  operator<(const CubeState& state)
  {
    return cube < state.cube;
  }

  bool
  operator<(const CubeState& state) const
  {
    return cube < state.cube;
  }

  void
  printSolution()
  {
    for (const auto& m : solution)
      std::cout << m << ' ';
  }
  
  Cube cube;
  moveset_t solution;
};

typedef std::queue<CubeState> frontierBFS_t;
typedef std::priority_queue<CubeState> frontierAStar_t;

/************************************************/
// Forward declarations
moveset_t
getStartMoves(const std::string& faceNames);

moveset_t
serialBFS(Cube cube);

moveset_t
serialBFSHelper(frontierBFS_t& frontier, const moveset_t& moves);

moveset_t
parallelBFS(Cube& cube, unsigned p);

CubeState
parallelBFSHelper(frontierBFS_t frontier, const moveset_t& moves, bool& finished, std::mutex& lock);

moveset_t
serialAStar(Cube& cube);

moveset_t
serialAStarHelper(frontierAStar_t& frontier, const moveset_t& moves);

moveset_t
parallelAStar(Cube& cube, unsigned p);

CubeState
parallelAStarHelper(frontierAStar_t frontier, const moveset_t& moves, bool& finished, std::mutex& lock);

unsigned
partitionStart(const unsigned p, const unsigned tid);

bool
uniqueMoves(const char face, const moveset_t& solution);

char
oppositeFace(const char face);
/************************************************/

int
main()
{
  std::cout << "Scramble => ";
  std::string scramble;
  std::getline(std::cin, scramble);

  std::cout << "Serial/Parallel (s/p) => ";
  std::string version;
  std::cin >> version;

  std::cout << "Algorithm (bfs/astar) => ";
  std::string algorithm;
  std::cin >> algorithm;

  Cube cube;
  cube.scramble(scramble);
  
  Timer t;
  moveset_t solution;
  unsigned p;
  if (version == "s")
  {
    if (algorithm == "bfs")
    {
      t.start();
      solution = serialBFS(cube);
      t.stop();
    }
    else
    {
      t.start();
      solution = serialAStar(cube);
      t.stop();
    }
  }
  else
  {
    std::cout << "p => ";
    std::cin >> p;

    if (algorithm == "bfs")
    {
      t.start();
      solution = parallelBFS(cube, p);
      t.stop();
    }
    else
    {
      t.start();
      solution = parallelAStar(cube, p);
      t.stop();
    }
  }

  std::cout << "\nSolution: ";
  for (const auto& m : solution)
    std::cout << m << ' ';
  std::cout << '\n';

  std::cout << t.elapsed() / 1000 << '\n';
  return 0;
}

/************************************************/

moveset_t
getStartMoves(const std::string& faceNames)
{
  moveset_t moves;
  std::string variants = "2\'";

  for (const char faceChar : faceNames)
  {
    std::string face;
    face += faceChar;
    moves.push_back(face);

    for (const char var : variants)
      moves.push_back(face + var);
  }

  return moves;
}

/************************************************/

moveset_t
serialBFS(Cube cube)
{
  if (cube.isSolved())
    return moveset_t();

  moveset_t initMoves = getStartMoves(cube.getFaceNames());
  frontierBFS_t frontier;

  for (const auto& move : initMoves)
  {
    CubeState state(cube);
    state.cube.move(move);
    state.solution.push_back(move);

    frontier.push(state);
  }

  return serialBFSHelper(frontier, initMoves);
}

/************************************************/

moveset_t
serialBFSHelper(frontierBFS_t& frontier, const moveset_t& moves)
{
  while (!frontier.front().cube.isSolved())
  {
    for (const auto& move : moves)
    {
      if (uniqueMoves(move[0], frontier.front().solution))
      {
        CubeState copyState(frontier.front());
        copyState.cube.move(move);
        copyState.solution.push_back(move);
        
        frontier.push(copyState);
      }
    }
    
    frontier.pop();
  }

  return frontier.front().solution;
}

/************************************************/

moveset_t
parallelBFS(Cube& cube, unsigned p)
{
  if (cube.isSolved())
    return moveset_t();

  bool finished = false;
  std::mutex lock;
  moveset_t initMoves = getStartMoves(cube.getFaceNames());

  std::vector<std::future<CubeState>> threads;
  for (unsigned tid = 0; tid < p; ++tid)
  {
    frontierBFS_t frontier;
    for (unsigned m = partitionStart(p, tid); m < partitionStart(p, tid + 1); ++m)
    {
      CubeState state(cube);
      state.cube.move(initMoves[m]);
      state.solution.push_back(initMoves[m]);

      frontier.push(state);
    }

    threads.push_back(std::async(std::launch::async, parallelBFSHelper, 
          frontier, std::cref(initMoves), std::ref(finished), std::ref(lock)));
  }

  bool foundSolved = false;
  CubeState solved;
  for (auto& t : threads)
  {
    CubeState state = t.get();
    if (!foundSolved && state.cube.isSolved())
      solved = state;
  }

  return solved.solution;
}
/************************************************/

CubeState
parallelBFSHelper(frontierBFS_t frontier, const moveset_t& moves, bool& finished, std::mutex& lock)
{
  while (true)
  {
    if (!finished && frontier.front().cube.isSolved())
    { 
      lock.lock();
      finished = true;
      lock.unlock();
    }
    
    if (finished)
      break;
   
    for (const auto& move : moves)
    {
      if (uniqueMoves(move[0], frontier.front().solution))
      {
        CubeState copyState(frontier.front());
        copyState.cube.move(move);
        copyState.solution.push_back(move);        

        frontier.push(copyState);
      }
    }

    frontier.pop();
  }

  return frontier.front();
}

/************************************************/

moveset_t
serialAStar(Cube& cube)
{
  if (cube.isSolved())
    return moveset_t();

  moveset_t initMoves = getStartMoves(cube.getFaceNames());
  frontierAStar_t frontier;

  for (const auto& move : initMoves)
  {
    CubeState state(cube);
    state.cube.move(move);
    state.solution.push_back(move);

    frontier.push(state);
  }

  return serialAStarHelper(frontier, initMoves);
}

/************************************************/

moveset_t
serialAStarHelper(frontierAStar_t& frontier, const moveset_t& moves)
{
  frontierAStar_t temp;
  while (true)
  {
    if (frontier.top().cube.isSolved())
      break;
    
    if (frontier.size() == 0)
    {
      frontier = temp;
      temp = frontierAStar_t();
    }

    for (const auto& move : moves)
    {
      if (uniqueMoves(move[0], frontier.top().solution))
      {
        CubeState copyState(frontier.top());
        copyState.cube.move(move);
        copyState.solution.push_back(move);
        
        if (copyState.cube.isSolved())
          return copyState.solution;
        
        temp.push(copyState);
      }
    }
    
    frontier.pop();
  }

  return frontier.top().solution;
}

/************************************************/

moveset_t
parallelAStar(Cube& cube, unsigned p)
{
  if (cube.isSolved())
    return moveset_t();

  bool finished = false;
  std::mutex lock;
  moveset_t initMoves = getStartMoves(cube.getFaceNames());

  std::vector<std::future<CubeState>> threads;
  for (unsigned tid = 0; tid < p; ++tid)
  {
    frontierAStar_t frontier;
    for (unsigned m = partitionStart(p, tid); m < partitionStart(p, tid + 1); ++m)
    {
      CubeState state(cube);
      state.cube.move(initMoves[m]);
      state.solution.push_back(initMoves[m]);

      frontier.push(state);
    }

    threads.push_back(std::async(std::launch::async, parallelAStarHelper,
          frontier, std::cref(initMoves), std::ref(finished), std::ref(lock)));
  }

  bool foundSolved = false;
  CubeState solved;
  for (auto& t : threads)
  {
    CubeState state = t.get();
    if (!foundSolved && state.cube.isSolved())
      solved = state;
  }

  return solved.solution;
}

/************************************************/

CubeState
parallelAStarHelper(frontierAStar_t frontier, const moveset_t& moves, bool& finished, std::mutex& lock)
{
  frontierAStar_t temp;
  while (true)
  {
    if (frontier.top().cube.isSolved())
    {
      lock.lock();
      finished = true;
      lock.unlock();
    }

    if (finished)
      break;
    
    if (frontier.size() == 0)
    {
      frontier = temp;
      temp = frontierAStar_t();
    }

    for (const auto& move : moves)
    {
      if (uniqueMoves(move[0], frontier.top().solution))
      {
        CubeState copyState(frontier.top());
        copyState.cube.move(move);
        copyState.solution.push_back(move);
        
        if (copyState.cube.isSolved())
        {
          lock.lock();
          finished = true;
          lock.unlock();

          return copyState;
        }

        temp.push(copyState);
      }
    }
    
    frontier.pop();
  }

  return frontier.top();
}
/************************************************/

bool
uniqueMoves(const char face, const moveset_t& solution)
{
  if (face == solution.back()[0])
    return false;

  if (solution.size() >= 3 && face == solution[solution.size() - 3][0] && 
      solution[solution.size() - 2][0] == oppositeFace(face))
    return false;

  return true;
}

/************************************************/

char
oppositeFace(const char face)
{
  switch (face)
  {
    case 'U':
      return 'D';
    case 'D':
      return 'U';
    case 'R':
      return 'L';
    case 'L':
      return 'R';
    case 'F':
      return 'B';
    default:
      return 'F';
  }
}

/************************************************/

unsigned
partitionStart(const unsigned p, const unsigned tid)
{
  return 18 * tid / p;
}
