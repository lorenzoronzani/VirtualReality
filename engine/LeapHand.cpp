#include "LeapHand.h"

void LIB_API LeapHand::setPosition(LeapHand::Position pos)
{
	this->pos = pos;
}

LeapHand::Position LIB_API LeapHand::getPosition()
{
	return pos;
}

void LeapHand::setNumHands(int hands)
{
	this->hands = hands;
}

int LeapHand::getNumHands()
{
	return hands;
}
