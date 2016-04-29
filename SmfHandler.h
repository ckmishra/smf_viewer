/*
 *
 *  Created on: Feb 4, 2016
 *      Author: chandan
 */

#ifndef SmfHandler_H_
#define SmfHandler_H_
#include <vector>
#include <string>
#include <map>
#include <set>
#include <GL/glut.h>
#include "WE/WEVertex.h"
#include "WE/WEFace.h"
#include "WE/WEEdge.h"

using namespace std;
/*
 *
 * This class is for backend processing for smf file conversion in Winged edge data structure.
 */

class SmfHandler {
public:
	SmfHandler();
	virtual ~SmfHandler();
	/*
	 *For saving .SMF file.
	 *@param filename.
	 *@return boolean
	 */
	bool save(const std::string filename);
	/*
	 *For loading  SMF file.
	 *@param filename.
	 *@return boolean
	 */
	bool loadFile(const std::string filename);
	/*
	 *Method for display of given SMF in different shading option.
	 *@param shadingOption.
	 *		1. Smooth Shaded
	 *		2. Flat Shaded
	 *		3. Wireframe
	 *		4. Shaded Edge
	 *@return
	 */
	void displaySmf(int shadingOption);

	/*Method for getting instance of SmfHandler.
	 *Singleton design pattern.
	 */
	static SmfHandler *instance() {
		if (!s_instance)
			s_instance = new SmfHandler;
		return s_instance;
	}
protected:
	// for clearing all tables
	void clearAll();
	// convert to Winged data structure
	void convert_to_WE(vector<size_t> face);
	// find edges given vertex
	vector<WE_Edge> findEdges(WE_Vertex* vertex);
	// find faces given vertex
	vector<WE_Face> findFaces(WE_Vertex* vertex);
	// find edges given face
	vector<WE_Edge> findEdges(WE_Face* face);
	// get face normal for smooth shaded.
	void getFaceNormal();
	// get vertex normal for smooth shaded.
	void getVertexNormal();
private:
	std::size_t num_vert;
	std::size_t num_face;
	std::map<int, WE_Vertex*> vertex_table;
	std::map<int, WE_Face*> face_table;
	std::map<pair<int, int>, WE_Edge*> edge_table;
	std::map<int, vector<GLfloat> > face_normals;
	std::map<int, vector<GLfloat> > vertex_normals;
	static SmfHandler *s_instance;
};

#endif /*SmfHandler_H_ */

