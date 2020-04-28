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

#include "Cycles.h"

typedef int piece_t;

#ifndef CUBE_HPP
#define CUBE_HPP

struct move_t
{
  unsigned index;
  std::vector<piece_t[4]> cycles;
};

class Cube
{
public:
  // default ctor
  Cube()
  {
    for (unsigned i = 0; i < PIECE_COUNT; ++i)
      m_cube[i] = i;
    
    for (unsigned i = 0; i < SIDE_COUNT; ++i)
      m_moveMap[MOVE_NAMES[i]] = i;

    std::string colors = "WOGRBY";
    for (unsigned i = 0; i < SIDE_COUNT; ++i)
      for (unsigned c = 0; c < SIDE_PIECE_COUNT; ++c)
        m_colors.push_back(colors[i]);
  }
  
  // copy ctor
  Cube(const Cube& other)
  {
    std::copy(std::begin(other.m_cube), std::end(other.m_cube), std::begin(m_cube));
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
      printf("%2c ", m_colors[m_cube[i]]);
      if (printCount % 3 == 2)
        std::cout << '\n';
    }

    printf("%2c ", m_colors[sideNum * 8]);
    ++printCount;

    for (unsigned i = index; i < index + 4; ++i, ++printCount)
    {
      printf("%2c ", m_colors[m_cube[i]]);
      if (printCount % 3 == 2)
        std::cout << '\n';
    }
  }

  // rotate single side given a move
  void
  move(const std::string& moveStr)
  {
    unsigned index = m_moveMap[moveStr[0]];
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
    else if (moveStr.size() == 1 && m_moveMap.find(moveStr[0]) != m_moveMap.end())
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
      if ((piece_t) i != m_cube[i])
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
  
  char
  oppositeFace(const std::string& move)
  {
    return OPP_MOVE_NAMES[m_moveMap[move[0]]];
  }

private:
  // rotate side one time
  // if prime is true, rotate counter-clockwise
  void
  rotateSide(int sideNum, bool prime)
  {
    auto cycles = MOVE_CYCLES[sideNum];
    
    for (unsigned i = 0; i < CYCLE_COUNT; ++i)
    {
      auto cycle = cycles[i];
      piece_t buffer[CYCLE_LENGTH];

      if (prime)
      {
        piece_t revCycle[CYCLE_LENGTH];
        for (unsigned f = 0, b = CYCLE_LENGTH - 1; f < CYCLE_LENGTH; ++f, --b)
          revCycle[f] = cycle[b];

        cycle = revCycle;
      }
      for (unsigned j = 1; j < CYCLE_LENGTH; ++j)
        buffer[j] = m_cube[cycle[j - 1]];
      buffer[0] = m_cube[cycle[CYCLE_LENGTH - 1]];

      for (unsigned j = 0; j < CYCLE_LENGTH; ++j)
        m_cube[cycle[j]] = buffer[j];
    }
  }

  // private constants
  static const unsigned CUBE_ORDER        = 3;
  static const unsigned SIDE_COUNT        = 6;
  static const unsigned SIDE_PIECE_COUNT  = 8;
  static const unsigned PIECE_COUNT       = 48;
  static const unsigned CYCLE_LENGTH      = 4;
  static const unsigned CYCLE_COUNT       = 5;
  static constexpr char MOVE_NAMES[7]     = "ULFRBD";
  static constexpr char OPP_MOVE_NAMES[7] = "DRBLFU";
  

  // member variables
  piece_t m_cube[PIECE_COUNT];
  std::unordered_map<char, unsigned> m_moveMap;
  std::string m_colors;
};

#endif
