#include "Object.h"
class LIB_API LeapHand {
public:
	struct LIB_API Position {
		float x;
		float y;
		float z;
	};
	void setPosition(Position pos);
	Position getPosition();
	void setNumHands(int hands);
	int getNumHands();
private:
	Position pos;
	int hands;
};