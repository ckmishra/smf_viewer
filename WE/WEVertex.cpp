/*
 * WEVertex.cpp
 *
 *  Created on: Feb 4, 2016
 *      Author: chandan
 */

#include "WEVertex.h"

WE_Vertex::WE_Vertex(GLfloat xc, GLfloat yc, GLfloat zc) {
	x = xc;
	y = yc;
	z = zc;
	Vertex_edge = NULL;
}

WE_Vertex::~WE_Vertex() {
	// TODO Auto-generated destructor stub
}

