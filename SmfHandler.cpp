/*
 * SmfHandler.cpp
 *
 *  Created on: Feb 4, 2016
 *  Author: chandan
 */

#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <math.h>
#include "SmfHandler.h"

using namespace std;
// global variables
int vertex_counter = 1;
int face_counter = 1;
// singleton instance
SmfHandler *SmfHandler::s_instance = 0;
// constructor
SmfHandler::SmfHandler() {
	num_face = 0;
	num_vert = 0;
}

//deconstructor
SmfHandler::~SmfHandler() {
}

// clear all tables and initialize counters
void SmfHandler::clearAll() {
	vertex_counter = 1;
	face_counter = 1;
	vertex_table.clear();
	face_table.clear();
	edge_table.clear();
	face_normals.clear();
	vertex_normals.clear();
}
// load file
bool SmfHandler::loadFile(const std::string file) {
	std::ifstream infile(file.c_str());
	if (infile.good()) {
		// clear all tables
		clearAll();
	} else {
		std::cerr << "Fail to load file " << file << std::endl;
		return false;
	}
	std::string line;
	bool first_line = true; // flag for reading first line
	// load line by line
	while (std::getline(infile, line)) {
		if (line.size() < 1) {
			continue;
		}
		std::istringstream iss(line.substr(1));
		std::vector<GLfloat> ver(3, 0.0);
		std::vector<size_t> face(3, 0);

		// switch option by first character
		switch (line[0]) {
		case '#': {
			if (first_line) {
				iss >> num_vert;
				iss >> num_face;
				first_line = false;
			}
			break;
		}
		case 'v': {
			for (size_t i = 0; i < 3; i++) {
				iss >> ver[i];
				ver[i] /= 1.0;
			}
			WE_Vertex* v = new WE_Vertex(ver[0], ver[1], ver[2]);
			v->id = vertex_counter++; // set id field to make key in map.
			vertex_table.insert(std::make_pair(v->id, v));
		}
			break;
		case 'f': {
			for (size_t i = 0; i < 3; i++) {
				iss >> face[i];
				face[i] /= 1;
			}
			convert_to_WE(face);
			break;
		}
		default:
			break;
		}
	}
	// Using Euler's formula to check file.
	if (vertex_table.size() != num_vert || face_table.size() != num_face
			|| edge_table.size() != num_face + num_vert - 2) {
		std::cerr << "Something wrong with given file. Please check.." << file
				<< std::endl;
		return false;
	}
	infile.close();

	// now populate normals table.
	getFaceNormal();
	getVertexNormal();

	std::cout << "File loaded successfully : " << file << std::endl;

	return true;
}

// save given smf file
bool SmfHandler::save(const std::string file) {
	std::string newfile = file.substr(0, file.find(".")).append("_copy.smf");
	ofstream myfile(newfile.c_str());
	myfile.clear();
	if (myfile.is_open()) {
		myfile << "# " << num_vert << " " << num_face << endl; // writing num of v and f
		// writing vertex
		map<int, WE_Vertex*>::iterator it_vertex = vertex_table.begin();
		while (it_vertex != vertex_table.end()) {
			it_vertex->second->x;
			myfile << "v" << " " << it_vertex->second->x << " "
					<< it_vertex->second->y << " " << it_vertex->second->z
					<< "\n";
			it_vertex++;
		}
		// writing face
		map<int, WE_Face*>::iterator it_face = face_table.begin();
		while (it_face != face_table.end()) {
			vector<WE_Edge> edges = findEdges(it_face->second);
			myfile << "f" << " ";
			for (size_t i = 0; i < 3; i++) {
				if ((edges[i].left) == (it_face->second)) {
					myfile << edges[i].start->id << " ";
				} else {
					myfile << edges[i].end->id << " ";
				}
			}
			myfile << "\n";
			it_face++;
		}
	} else {
		cout << "Unable to save.";
	}
	myfile.close();
	return true;
}

// helper function for converting .smf to Winged data structure.
void SmfHandler::convert_to_WE(vector<size_t> face) {
	WE_Face* we_face = new WE_Face();
	WE_Edge* we_edge[3];

// construct edge and vertex
	for (size_t i = 0; i < 2; ++i) {
		we_edge[i] = new WE_Edge();
		// check if edge exist
		map<pair<int, int>, WE_Edge*>::iterator it = edge_table.find(
				make_pair(face[i + 1], face[i]));
		if (it == edge_table.end()) {
			// if edge does not exist then insert in edge table and make this face as left face.
			we_edge[i]->start = (vertex_table.find(face[i])->second);
			we_edge[i]->end = (vertex_table.find(face[i + 1])->second);
			we_edge[i]->left = we_face;
			edge_table.insert(
					make_pair(make_pair(face[i], face[i + 1]), we_edge[i]));
			WE_Vertex* vertex = vertex_table[face[(i + 1)]];
			// assign this edge to vertex
			if (vertex->Vertex_edge == NULL)
				vertex->Vertex_edge = we_edge[i];

		} else {
			// if edge exist in table then update right face of edge.
			it->second->right = we_face;
			we_edge[i] = it->second;
		}
	}

	// Handling back edge using same logic
	we_edge[2] = new WE_Edge();
	map<pair<int, int>, WE_Edge*>::iterator it = edge_table.find(
			make_pair(face[0], face[2]));
	if (it == edge_table.end()) {
		we_edge[2]->start = (vertex_table.find(face[2])->second);
		we_edge[2]->end = (vertex_table.find(face[0])->second);
		we_edge[2]->left = we_face;
		edge_table.insert(make_pair(make_pair(face[2], face[0]), we_edge[2]));
		WE_Vertex* vertex = vertex_table[face[0]];
		if (vertex->Vertex_edge == NULL)
			vertex->Vertex_edge = we_edge[2];
	} else {
		it->second->right = we_face;
		we_edge[2] = it->second;
	}

	// attaching edge to face
	we_face->face_edge = we_edge[0];
	we_face->id = face_counter++;// attaching ID  also for easy access using id as Key in map
	face_table.insert(std::make_pair(we_face->id, we_face));

	// updating edge right and left successor of current edge
	for (size_t i = 1; i < 3; ++i) {
		map<pair<int, int>, WE_Edge*>::iterator it1 = edge_table.find(
				make_pair(face[i - 1], face[i]));
		map<pair<int, int>, WE_Edge*>::iterator it2 = edge_table.find(
				make_pair(face[i], face[i - 1]));
		if (it1 != edge_table.end()) {
			it1->second->left_prev = we_edge[i];
			it1->second->left_next = we_edge[(i + 1) % 3];
		} else if (it2 != edge_table.end()) {
			it2->second->right_prev = we_edge[i];
			it2->second->right_next = we_edge[(i + 1) % 3];
		}
	}
	// for back edge
	map<pair<int, int>, WE_Edge*>::iterator it1 = edge_table.find(
			make_pair(face[2], face[0]));
	map<pair<int, int>, WE_Edge*>::iterator it2 = edge_table.find(
			make_pair(face[0], face[2]));
	if (it1 != edge_table.end()) {
		it1->second->left_prev = we_edge[0];
		it1->second->left_next = we_edge[1];
	} else if (it2 != edge_table.end()) {
		it2->second->right_prev = we_edge[0];
		it2->second->right_next = we_edge[1];
	}
	return;
}

// find edges given vertex
vector<WE_Edge> SmfHandler::findEdges(WE_Vertex* vertex) {
	WE_Edge *e0 = vertex->Vertex_edge;
	WE_Edge *edge = e0;
	vector<WE_Edge> vedges;
	do {
		vedges.push_back(*edge);
		if (edge->end == vertex)
			edge = edge->right_next;
		else {
			edge = edge->left_next;
		}
	} while (edge != e0);
	return vedges;
}
// find faces given vertex
vector<WE_Face> SmfHandler::findFaces(WE_Vertex* vertex) {
	WE_Edge *e0 = vertex->Vertex_edge;
	WE_Edge *edge = e0;
	vector<WE_Face> vFaces;
	do {

		if (edge->end == vertex) {
			vFaces.push_back(*(edge->right));
			edge = edge->right_next;
		} else {
			vFaces.push_back(*(edge->left));
			edge = edge->left_next;
		}
	} while (edge != e0);
	return vFaces;
}
// find edges given face
vector<WE_Edge> SmfHandler::findEdges(WE_Face * face) {

	WE_Edge *e0 = face->face_edge;
	WE_Edge *edge = e0;
	vector<WE_Edge> vEdges;
	do {
		if (edge->right == face) {
			edge = edge->right_prev;
		} else {
			edge = edge->left_prev;
		}
		vEdges.push_back(*edge);
	} while (edge != e0);

	return vEdges;
}
// computing face Normal and storing in table
void SmfHandler::getFaceNormal() {
	map<int, WE_Face*>::iterator face_it = face_table.begin();
	while (face_it != face_table.end()) {
		vector<WE_Edge> edges = findEdges(face_it->second);
		vector<WE_Vertex*> vertexs(3);
		for (int i = 0; i < 3; i++) {
			if (edges[i].left == face_it->second) {
				vertexs[i] = edges[i].start;
			} else
				vertexs[i] = edges[i].end;
		}
		std::vector<GLfloat> vector_x(3), vector_y(3);
		std::vector<GLfloat> normal(3);

		vector_x[0] = (vertexs[1]->x) - (vertexs[0]->x);
		vector_x[1] = (vertexs[1]->y) - (vertexs[0]->y);
		vector_x[2] = (vertexs[1]->z) - (vertexs[0]->z);

		vector_y[0] = (vertexs[2]->x) - (vertexs[1]->x);
		vector_y[1] = (vertexs[2]->y) - (vertexs[1]->y);
		vector_y[2] = (vertexs[2]->z) - (vertexs[1]->z);

		// normal calculation
		normal[0] = vector_x[1] * vector_y[2] - vector_x[2] * vector_y[1];
		normal[1] = vector_x[2] * vector_y[0] - vector_x[0] * vector_y[2];
		normal[2] = vector_x[0] * vector_y[1] - vector_x[1] * vector_y[0];
		// normalize it to one
		GLfloat length = sqrt(
				normal[0] * normal[0] + normal[1] * normal[1]
						+ normal[2] * normal[2]);
		for (size_t j = 0; j < 3; ++j) {
			normal[j] /= length;
		}
		face_normals.insert(make_pair(face_it->first, normal));
		face_it++;
	}
	return;
}

// computing vertex normal
void SmfHandler::getVertexNormal() {
	map<int, WE_Vertex*>::iterator vertex_it = vertex_table.begin();
	while (vertex_it != vertex_table.end()) {

		vector<WE_Face> faces = findFaces(vertex_it->second);
		vector<GLfloat> vertexNormal(3, 0.0);

		for (size_t i = 0; i < faces.size(); i++) {
			for (size_t k = 0; k < 3; k++) {
				vector<GLfloat> temp = face_normals[faces[i].id];
				vertexNormal[k] = vertexNormal[k] + temp.at(k);
			}
		}

		// normalize it to one
		GLfloat length = sqrt(
				vertexNormal[0] * vertexNormal[0]
						+ vertexNormal[1] * vertexNormal[1]
						+ vertexNormal[2] * vertexNormal[2]);
		for (size_t k = 0; k < 3; k++) {
			vertexNormal[k] /= length;
		}
		vertex_normals.insert(make_pair(vertex_it->first, vertexNormal));
		vertex_it++;
	}
	return;
}

void SmfHandler::displaySmf(int shading_option) {
	vector<GLfloat> normal(3);
	vector<WE_Edge> edges(3);
	glBegin(GL_TRIANGLES);
	bool isFlatShaded = false;
	for (size_t i = 1; i <= face_table.size(); i++) {
		WE_Face* f = face_table[i];
		edges = SmfHandler::instance()->findEdges(f);
		for (size_t j = 0; j < edges.size(); j++) {
			switch (shading_option) {
			case 2:
			case 4:
				// in flat shaded - use face normal
				normal = face_normals[i];
				if (j == 0)		// flag for only first time to send face normal in case of flat shaded.
					isFlatShaded = true;
				break;

			default: // smooth shaded uses vertex normal
				if (edges[j].left == f) {
					normal = vertex_normals[edges[j].start->id];
				} else {
					normal = vertex_normals[edges[j].end->id];
				}
				break;
			}
			// sending normal coordinates
			if (isFlatShaded) {
				glNormal3f(normal[0], normal[1], normal[2]);
				isFlatShaded = !isFlatShaded;
			} else {
				glNormal3f(normal[0], normal[1], normal[2]);
			}
			// sending vertex coordinates
			if (edges[j].left == f) {
				glVertex3f(edges[j].start->x, edges[j].start->y,
						edges[j].start->z);
			} else {
				glVertex3f(edges[j].end->x, edges[j].end->y, edges[j].end->z);
			}
		}
	}
	glEnd();
}

