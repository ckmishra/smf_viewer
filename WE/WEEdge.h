/*
 * WEEdge.h
 * This class is for representation of edge in Winged edge data structure.
 *  Created on: Feb 4, 2016
 *      Author: chandan
 */

#ifndef WEEDGE_H_
#define WEEDGE_H_

class WE_Face;
class WE_Vertex;

class WE_Edge {
public:
	WE_Edge();
	virtual ~WE_Edge();
	WE_Vertex *start, *end;
	WE_Face *left, *right;
	WE_Edge *left_next, *right_next;
	WE_Edge *left_prev, *right_prev;
};

#endif /* WEEDGE_H_ */
