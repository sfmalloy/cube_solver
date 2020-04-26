// TODO
// 2. Implement rotate face
// 3. Implement scramble
// 4. Implement operator '=='
// 5. Copy ctor
// ...
// N. Comments

#include <iostream>
#include <string>
#include <iterator>
#include <unordered_map>
#include <string.h>

#ifndef CUBE_HPP
#define CUBE_HPP

typedef struct side
{
	static const unsigned length = 3;
	static const unsigned size = length * length;
	
	int pieces[size];
	int color;

	int adj[4];
	int opp;

	int piece_swaps[3];
} side_t;

class Cube
{
public:
	using value_type = side_t;
	
	using iterator = value_type*;
	using const_iterator = const value_type*;
	using reverse_iterator = const value_type*;

	using reference = value_type&;
	using const_reference = const value_type&;

	using size_type = unsigned;
	using difference_type = ptrdiff_t;

	// default ctor
	Cube()
	{
		int adj_colors[6][4] = { { 3, 2, 1, 4 }, 
														 { 0, 2, 5, 4 }, 
														 { 0, 3, 5, 1 }, 
														 { 0, 4, 5, 2 }, 
														 { 0, 1, 5, 3 }, 
														 { 1, 2, 3, 4 } };
		
		int piece_swaps[6][3] = { {    0, 1111, 2222 },
															{ 8206, 5137, 2068 },
															{ 8808, 5535, 2262 },
															{ 6820, 3751, 682  },
															{ 2000, 5333, 8666 },
															{ 6666, 7777, 8888 } };

		auto sum = [&](int color)
		{
			int s = 0;
			for (unsigned i = 0; i < 4; ++i)
				s += adj_colors[color][i];
			return s;
		};

		int color = 0;
		for (auto it = _begin(); it != _end(); ++it, ++color)
		{
			it->color = color;
			for (unsigned i = 0; i < it->size; ++i)
				it->pieces[i] = color;

			memcpy(it->adj, adj_colors[color], 4 * sizeof(int));
			memcpy(it->piece_swaps, piece_swaps[color], 3 * sizeof(int));
//			it->adj = adj_colors[color];
//			it->piece_swaps = piece_swaps[color];
			it->opp = 15 - sum(color) - color;
		}

		std::string faces = "UFRBLD";
		for (unsigned i = 0; i < faces.size(); ++i)
			move_map[faces[i]] = i;
	}

	// TODO
	Cube(Cube& other)
	{
		// copy ctor
	}

	void
	reset()
	{
		for (auto& side : m_cube)
			for (unsigned i = 0; i < side.size; ++i)
				side.pieces[i] = side.color;
	}

	bool
	is_solved()
	{
		for (auto it = _begin(); it != _end(); ++it)
			for (unsigned i = 0; i < it->size - 1; ++i)
				if (it->pieces[i] != it->color)
					return false;

		return true;
	}
	
	// TODO
	void
	rotate_side(int side_num)
	{
		// 1. Reverse rows
		// 2. Matrix transpose
		// 3. Swap adjacent pieces to their new side
		// 
		// NOTE: { 
		//				 U : 0, // White
		//				 F : 1, // Green
		//				 R : 2, // Red
		//				 B : 3, // Blue
		//				 L : 4, // Orange
		//				 D : 5  // Yellow
		//			 }
		//
		// Pieces to swap for different directions:
		//
		
		// face row reversal
		side_t* face = &(m_cube[side_num]);
		for (unsigned f = 0, b = face->length * 2; f < face->length && b < face->size; ++f, ++b)
			std::swap(face->pieces[f], face->pieces[b]);

		side_t rotated = *face;
		unsigned len = face->length;
		for (unsigned i = 0; i < len; ++i)
			for (unsigned j = 0; j < len; ++j)
				rotated.pieces[len * i + j] = face->pieces[len * j + i];

		// Moves adjacent pieces 90 degrees to next face.
		for (unsigned i = 0; i < face->size; ++i)
		{
			int swaps = face->piece_swaps[i];
			int curr = swaps % 10;
			int buffer;
			for (unsigned j = 0; j < 3; ++j)
			{
				int next = (swaps / 10) % 10;
				buffer = m_cube[face->adj[j + 1]].pieces[next];
				m_cube[face->adj[j + 1]].pieces[next] = m_cube[face->adj[j]].pieces[curr];
				curr = next;
				swaps /= 10;
			}

			m_cube[face->adj[0]].pieces[curr] = buffer;
		}

		m_cube[side_num] = rotated;
	}

	// TODO
	void
	scramble(const std::string& moves);
	
	// TODO
	bool
	operator ==(const Cube& other);

	const_iterator
	begin() const
	{
		return m_cube;
	}
	
	const_iterator
	end() const
	{
		return m_cube + size();
	}

	unsigned
	size() const
	{
		return NUM_SIDES;
	}

	side_t
	get_side(int side_num) const
	{
		return m_cube[side_num];
	}

private:
	iterator
	_begin()
	{
		return m_cube;
	}
	
	iterator
	_end()
	{
		return m_cube + size();
	}
	
	static const int NUM_SIDES = 6;
	side_t m_cube[NUM_SIDES];
	std::unordered_map<char, int> move_map;

};

#endif
