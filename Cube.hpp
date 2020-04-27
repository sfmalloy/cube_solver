/*
 * Sean Malloy
 * 04/26/2020
 * Cube.hpp
 * 3x3x3 cube data structure represented as a 1D array of integers.
 */

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <iterator>


typedef int piece_t;
typedef piece_t* moveCycle_t;

#ifndef CUBE_HPP
#define CUBE_HPP

struct move_t
{
  unsigned index;
  std::vector<moveCycle_t> cycles;
};

class Cube
{
public:
  // default ctor
  Cube()
  {
    for (unsigned i = 0; i < PIECE_COUNT; ++i)
      m_cube[i] = i + 1;

    std::string colors = "WOGRBY";
    for (unsigned i = 0; i < SIDE_COUNT; ++i)
      for (unsigned c = 0; c < SIDE_PIECE_COUNT; ++c)
        m_colors.push_back(colors[i]);

    initCycles();
  }
  
  // copy ctor
  Cube(Cube& other)
  {
    initCycles();
    std::copy(std::begin(other.m_cube), std::end(other.m_cube), std::begin(m_cube));
  }

  ~Cube()
  {
    for (auto& p : m_moveMap)
      for (auto& cycle : p.second.cycles)
        delete[] cycle;
  }

  void
  initCycles()
  {
    std::ifstream cyclesFile("cycles.txt");
    if (!cyclesFile.is_open())
    {
      std::cout << "Failed to open cycles file, exiting\n";
      exit(1);
    }
    else
    {
      std::string line, token;
      unsigned nameIndex = 0, i = 0;

      while (std::getline(cyclesFile, line))
      {
        std::stringstream tokenize(line);
        moveCycle_t cycle = new piece_t[CYCLE_LENGTH];
        move_t currMove;
          
        while (std::getline(tokenize, token, ' '))
        {
          cycle[i] = std::stoi(token) - 1;
          ++i;
          
          if (i == CYCLE_LENGTH)
          {
            currMove.cycles.push_back(cycle);
            cycle = new piece_t[CYCLE_LENGTH];
            i = 0;
          }
        }
        currMove.index = nameIndex;
        m_moveMap[MOVE_NAMES[nameIndex++]] = currMove;

        delete[] cycle;
      }
    }
  }

  void
  printCube()
  {
    for (unsigned i = 0; i < SIDE_COUNT; ++i)
    {
      printf("%c\n", MOVE_NAMES[i]);
      printSide(i);
    }
  }

  // print single side
  void
  printSide(int sideNum)
  {
    unsigned index = 8 * sideNum + 4, printCount = 0;
    for (unsigned i = index - 4; i < index; ++i, ++printCount)
    {
      printf("%2c ", m_colors[m_cube[i] - 1]);
      if (printCount % 3 == 2)
        std::cout << '\n';
    }

    printf("%2c ", m_colors[sideNum * 8]);
    ++printCount;

    for (unsigned i = index; i < index + 4; ++i, ++printCount)
    {
      printf("%2c ", m_colors[m_cube[i] - 1]);
      if (printCount % 3 == 2)
        std::cout << '\n';
    }
  }

  // rotate single side given a move
  void
  move(const std::string& moveStr)
  {
    unsigned index = m_moveMap[moveStr[0]].index;
    if (moveStr.size() == 2)
    {
      if (moveStr[1] == '\'')
        rotateSide(index, true);
      else if (moveStr[1] == '2')
      {
        rotateSide(index, false);
        rotateSide(index, false);
      }
      else
      {
        fprintf(stderr, "Invalid move (%s), exiting\n", moveStr.c_str());
        exit(1);
      }
    }
    else if (moveStr.size() == 1)
      rotateSide(index, false);
    else
    {
      fprintf(stderr, "Invalid move (%s), exiting\n", moveStr.c_str());
      exit(1);
    }
  }

  // scramble the cube given a space seperated scramble string
  void
  scramble(const std::string& moveStr)
  {
    std::stringstream tokenize(moveStr);
    std::string token;

    while (std::getline(tokenize, token, ' '))
      move(token);
  }

  bool
  isSolved()
  {
    for (unsigned i = 0; i < PIECE_COUNT; ++i)
      if ((int) i + 1 != m_cube[i])
        return false;

    return true;
  }

  bool
  operator==(const Cube& other)
  {
    for (unsigned i = 0; i < PIECE_COUNT; ++i)
      if (other.m_cube[i] != m_cube[i])
        return false;

    return true;
  }

private:
  
  // rotate side one time
  // if prime is true, rotate counter-clockwise
  void
  rotateSide(int sideNum, bool prime)
  {
    piece_t cycleBuffer[CYCLE_LENGTH], cycle[CYCLE_LENGTH];
    for (auto& c : m_moveMap[MOVE_NAMES[sideNum]].cycles)
    {
      std::copy(c, c + CYCLE_LENGTH, std::begin(cycle));
      
      if (prime)
        std::reverse(std::begin(cycle), std::end(cycle));
      
      for (unsigned i = 1; i < CYCLE_LENGTH; ++i)
        cycleBuffer[i] = m_cube[cycle[i - 1]];
      cycleBuffer[0] = m_cube[cycle[CYCLE_LENGTH - 1]];

      for (unsigned i = 0; i < CYCLE_LENGTH; ++i)
        m_cube[cycle[i]] = cycleBuffer[i];
    }
  }
  // private constants
  static const unsigned CUBE_ORDER       = 3;
  static const unsigned SIDE_COUNT       = 6;
  static const unsigned SIDE_PIECE_COUNT = 8;
  static const unsigned PIECE_COUNT      = 48;
  static const unsigned CYCLE_LENGTH     = 4;
  static constexpr char MOVE_NAMES[7]    = "ULFRBD";

  // member variables
  piece_t m_cube[PIECE_COUNT];
  std::unordered_map<char, move_t> m_moveMap;
  std::string m_colors;
};

#endif
