/*
 * Segment library for use with Arudino Mega 2560. Created by Cameron Mann.
*/

#include "Segment.h"

Segment::Segment(int x, int y) {
	_x = x;
	_y = y;
}

void Segment::updateSelf(int x, int y) {
	_nextX = _x;
	_nextY = _y;
	_x = x;
	_y = y;
}

void Segment::updateNext() {
	_next->updateSelf(_nextX, _nextY);
}

void Segment::setNext(Segment *n) {
	_next = n;
}

int Segment::getX() {
	return _x;
}

int Segment::getY() {
	return _y;
}
