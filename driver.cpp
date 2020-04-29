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
#include <unistd.h>

/************************************************/
// Local includes
#include "Cube.hpp"
#include "Timer.hpp"

/************************************************/
// Constants
const unsigned FACE_COUNT = 6;
const char MOVE_NAMES[7]  = "ULFRBD";

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

  Cube cube;
  moveset_t solution;
};

typedef std::queue<CubeState> serialFrontier_t;

/************************************************/
// Forward declarations
moveset_t
getStartMoves(const std::string& faceNames);

moveset_t
serialBFS(Cube cube);

moveset_t
serialHelper(serialFrontier_t& frontier, const moveset_t& moves);

bool
uniqueMoves(const char& face, const moveset_t& solution);

char
oppositeFace(const char& face);
/************************************************/

int
main()
{
  std::cout << "Scramble => ";
  std::string scramble;
  std::getline(std::cin, scramble);

  Cube cube;
  cube.scramble(scramble);
  
  Timer t;

  t.start();
  moveset_t solution = serialBFS(cube);
  t.stop();

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

  for (const char& faceChar : faceNames)
  {
    std::string face;
    face += faceChar;
    moves.push_back(face);

    for (const char& var : variants)
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
  serialFrontier_t frontier;

  for (const auto& move : initMoves)
  {
    CubeState state(cube);
    state.cube.move(move);
    state.solution.push_back(move);

    frontier.push(state);
  }

  return serialHelper(frontier, initMoves);
}

/************************************************/

moveset_t
serialHelper(serialFrontier_t& frontier, const moveset_t& moves)
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

bool
uniqueMoves(const char& face, const moveset_t& solution)
{
  if (face == solution.back()[0])
    return false;

  if (solution.size() >= 3 && face == solution[solution.size() - 3][0] && 
      solution[solution.size() - 2][0] == oppositeFace(face))
    return false;

  return true;
}

char
oppositeFace(const char& face)
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
