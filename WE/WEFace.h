/*
 * WEFace.h
 * This class is for face representation of Winged edge data structure.
 *  Created on: Feb 4, 2016
 *      Author: chandan
 *
 */

#ifndef WE_WEFACE_H_
#define WE_WEFACE_H_
#include "WEEdge.h"


class WE_Face {
public:
	WE_Face();
	virtual ~WE_Face();
	WE_Edge* face_edge;
	int id;
};

#endif /* WE_WEFACE_H_ */
