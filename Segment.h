/*
 * Segment library for use with Arudino Mega 2560. Created by Cameron Mann.
*/


#ifndef Segment_h
#define Segment_h

#include "Arduino.h"

class Segment {
	public:
		Segment(int x, int y);
		
		/*
			calls updateSelf on the segment pointed to by _next with the params _nextX and _nextY
		*/
		void updateNext();
		
		/*
			used to set the _next pointer to the next segment
		*/
		void setNext(Segment*);
		/*
			sets the _nextX and _nextY to the current x and y coordinates, and the current
			_x and _y coordinates to the params
		*/
		void updateSelf(int x, int y);
		int getX();
		int getY();
	private:
		int _x; // x coordinate of this segments current position
		int _y; // y coordinate of this segments current position
		int _nextX; // The previous x coordinate this segment was at, passed to the next segment
		int _nextY; // The previous y coordinate this segment was at, passed to the next segment
		Segment *_next; // pointer to the next segment directly following this segment
};

#endif
