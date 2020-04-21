// TODO
// 0. Macro guards
// 1. Implement scramble
// 2. Implement rotate face
// 3. Implement operator ==
// ...
// N. Comments

#include <string>

class Cube
{
public:
	Cube()
	{ }

	// TODO
	void
	scramble(const std::string& moves);

	// TODO
	void
	rotate_face(int face_num);

	// TODO
	bool
	operator ==(const Cube& other);

private:
	int cube[3][3][3];
};
