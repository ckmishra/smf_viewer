/*
 * WEVertex.h
 * This class is for vertex representation of Winged edge data structure.
 *
 *  Created on: Feb 4, 2016
 *      Author: chandan
 */

#ifndef WE_WEVERTEX_H_
#define WE_WEVERTEX_H_
#include <GL/glut.h>
#include "WEEdge.h"

//class WE_Edge;
class WE_Vertex {
public:
	WE_Vertex(GLfloat xc, GLfloat yc, GLfloat zc);
	virtual ~WE_Vertex();
	int id ;
	GLfloat x, y, z;
	WE_Edge *Vertex_edge;
};

#endif /* WE_WEVERTEX_H_ */
