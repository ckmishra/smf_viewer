/*
 * This is SmfViewer class which will intract with end user.
 *
 *  Created on: Feb 4, 2016
 *      Author: chandan
 */

#include <math.h>
#include <iostream>
#include <GL/glui.h>
#include "SmfHandler.h"

using namespace std;

//*************************************************************************
//  GLUT Declarations
//*************************************************************************

//  Callback functions
void display(void);
void reshape(int w, int h);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void pmotion(int x, int y);
void keyboard(unsigned char key, int x, int y);

//  define the window position on screen
int window_x;
int window_y;

//  variables representing the window size
int window_width = 800;
int window_height = 600;

// xy aspect for frustum
float xy_aspect;

//  The id of the main window
GLuint main_window;

//*************************************************************************
//  GLUI Declarations
//*************************************************************************
//  Declare live variables (related to GLUI)
//  Rotation Matrix Live Variable Array
float rotation_matrix[16] = { 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0,
		1.0, 0.0, 0.0, 0.0, 0.0, 1.0 };
//  Translation XY Live Variable
GLfloat translate_xy[2] = { 0, 0 };
GLfloat translate_x = 0;		//  Translation X Live Variable
GLfloat translate_y = 0;		//  Translation Y Live Variable
GLfloat translate_z = 0;		//  Translation Z Live Variable

float scale = 1;			//  Spinner Scale Live Variable

//  Set up the GLUI window and its components
int setupGLUI();

//  Idle callack function
void idle();

//  Declare callbacks related to GLUI
void glui_callback(int arg);

//  Declare the IDs of controls generating callbacks
enum {
	SHADING_LISTBOX = 0,
	TRANSLATION_XY,
	TRANSLATION_X,
	TRANSLATION_Y,
	TRANSLATION_Z,
	ROTATION,
	SCALE_SPINNER,
	RESET,
	QUIT_BUTTON,
	SAVE_BUTTON,
	OPEN_BUTTON,
	FILE_NAME
};

int listbox_item_id = 1; // live variable

//*************************************************************************
//  Lighting Declarations
//*************************************************************************
GLfloat light0_ambient[] = { 0.1f, 0.1f, 0.3f, 1.0f };
GLfloat light0_diffuse[] = { .4f, .4f, 1.0f, 1.0f };
GLfloat light0_position[] = { .5f, .5f, 1.0f, 0.0f };

GLfloat light1_ambient[] = { 0.1f, 0.1f, 0.4f, 1.0f };
GLfloat light1_diffuse[] = { .8f, .6f, 0.0f, 1.0f };
GLfloat light1_position[] = { -1.0f, -1.0f, 1.0f, 0.0f };
// set up lighting
int setupLighting();

// initial loading of mesh
SmfHandler *handler; // Singleton handler object.
char filetext[sizeof(GLUI_String)] = "sphere";	// default value
std::string folder = "smf_files/";
std::string filename = folder.append(filetext).append(".smf");
// run handler on start of program.
void runSmfHandler() {
	handler = SmfHandler::instance();
	handler->loadFile(filename);
}
//-------------------------------------------------------------------------
//  This function sets the window x and y coordinates
//  such that the window becomes centered
//-------------------------------------------------------------------------
void centerOnScreen() {
	window_x = (glutGet(GLUT_SCREEN_WIDTH) - window_width) / 2;
	window_y = (glutGet(GLUT_SCREEN_HEIGHT) - window_height) / 2;
}

//-------------------------------------------------------------------------
//  This function is passed to glutDisplayFunc in order to display
//	OpenGL contents on the window.
//-------------------------------------------------------------------------
void display(void) {
	glClearColor(.9f, .9f, .9f, 1.0f);
	//  Clear the window
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode( GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-xy_aspect * .05, xy_aspect * .05, -.05, .05, 0.1, 12.0);
	glMatrixMode( GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -2.5f);
	// apply rotation
	glMultMatrixf(rotation_matrix);
	//  Apply the translation
	glTranslatef(translate_x, translate_y, -translate_z);
	glTranslatef(translate_xy[0], translate_xy[1], -translate_z);
	//apply scaling
	glScalef(scale, scale, scale);

	switch (listbox_item_id) {
	case 1:
	case 2:
		// for both smooth and flat shaded
		glPushMatrix();
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL);
		handler->displaySmf(listbox_item_id);
		glPopMatrix();
		break;
	case 3:
		// Wireframe
		glPushMatrix();
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		handler->displaySmf(listbox_item_id);
		glPopMatrix();
		break;
	case 4:
		// Flat Edge shaded. Using Flat and displaying edge on top using Wireframe.
		glPushMatrix();
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL);
		handler->displaySmf(listbox_item_id);
		glPopMatrix();
		// the edges
		glPushMatrix();
		glColorMaterial(GL_FRONT, GL_DIFFUSE);
		glEnable(GL_COLOR_MATERIAL);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glColor3f(0.0f, 0.0f, 0.0f);
		handler->displaySmf(listbox_item_id);
		glColor3f(0.9f, 0.9f, 0.9f);
		glDisable(GL_COLOR_MATERIAL);
		glPopMatrix();
		break;

	default:
		break;
	}

	glEnable(GL_LIGHTING);
	//  Swap contents of backward and forward frame buffers
	glutSwapBuffers();

}

//-------------------------------------------------------------------------
//  This function is passed to the glutReshapeFunc and is called 
//  whenever the window is resized.
//-------------------------------------------------------------------------
void reshape(int w, int h) {
//  Stay updated with the window width and height
	window_width = w;
	window_height = h;
	int vx, vy, vw, vh;
//  Get the drawing area viewport characteristics
	GLUI_Master.get_viewport_area(&vx, &vy, &vw, &vh);
//  Set the viewport
	glViewport(vx, vy, vw, vh);
	xy_aspect = (float) vw / (float) vh;
//  Call post redisplay to redraw contents
	glutPostRedisplay();
}

//-------------------------------------------------------------------------
//  This function is passed to the glutMouseFunc and is called 
//  whenever the mouse is clicked.
//-------------------------------------------------------------------------
void mouse(int button, int state, int x, int y) {

}

//-------------------------------------------------------------------------
//  This function is passed to the glutMotionFunc and is called 
//  whenever the mouse is dragged.
//-------------------------------------------------------------------------
void motion(int x, int y) {
	glutPostRedisplay();

}

//-------------------------------------------------------------------------
//  This function is passed to the glutPassiveMotionFunc and is called 
//  whenever the mouse is moved.
//-------------------------------------------------------------------------
void pmotion(int x, int y) {
	glutPostRedisplay();

}

//-------------------------------------------------------------------------
//  This function is passed to the glutKeyboardFunc and is called 
//  whenever the user hits a key.
//-------------------------------------------------------------------------
void keyboard(unsigned char key, int x, int y) {

	switch (key) {
	//  User hits ESC key
	case 27:
	case 'q':
	case 'Q':
		exit(1);
		break;
	}
	glutPostRedisplay();
}

//*************************************************************************
//  GLUI Functions.
//*************************************************************************

//-------------------------------------------------------------------------
//  GLUI callback function.
//-------------------------------------------------------------------------
void glui_callback(int control_id) {
//  Behave based on control ID
	switch (control_id) {
	case SHADING_LISTBOX:
		break;
		//  Translation XY control
	case TRANSLATION_XY:
		break;
		//  Translation Z control
	case TRANSLATION_Z:
		break;
		//  rotation control is being rotated
	case ROTATION:
		break;
		//  Scaling
	case SCALE_SPINNER:
		break;
		//File Name
	case FILE_NAME: {
		filename = "";
		folder = "smf_files/";
		string str(filetext);
		if (str.find(".smf") != std::string::npos) {
			filename = folder.append(filetext);
		} else {
			filename = folder.append(str).append(".smf");
		}
	}
		break;
		//  Open Button clicked
	case OPEN_BUTTON: {
		handler->loadFile(filename);
		glutPostRedisplay();
		break;
	}
		//  Save Button clicked
	case SAVE_BUTTON:
		handler->save(filename);
		break;
		//  Quit Button clicked
	case QUIT_BUTTON:
		printf("Quit Button clicked... Exit!\n");
		exit(1);
		break;
	case RESET:	//RESET all translation,rotation and scaling.
		printf("RESET Button clicked!\n");
		// reset wire value
		for (int i = 0; i < 16; i++) {
			if (i == 0 || i == 5 || i == 10 || i == 15)
				rotation_matrix[i] = 1.0;
			else
				rotation_matrix[i] = 0.0;

		}
		translate_xy[0] = 0;
		translate_xy[1] = 0;
		translate_x = 0;
		translate_y = 0;
		translate_z = 0;
		scale = 1;
		glutPostRedisplay();
		break;

	}
}
//-------------------------------------------------------------------------
//  Idle Callback function.
//
//  Set the main_window as the current window to avoid sending the
//  redisplay to the GLUI window rather than the GLUT window.
//  Call the Sleep function to stop the GLUI program from causing
//  starvation.
//-------------------------------------------------------------------------
void idle() {
	// According to the GLUT specification, the current window is undefined during an idle callback.  So we need to explicitly change it if necessary
	if (glutGetWindow() != main_window) {
		glutSetWindow(main_window);
	}
	GLUI_Master.sync_live_all();
	glutPostRedisplay();
}
// GLUI setup
int setupGLUI() {
	//  Set idle function
	GLUI_Master.set_glutIdleFunc(idle);
	GLUI *glui_h_subwindow, *glui_v_subwindow;
//  Create GLUI vertical subwindow (placed on right)
	glui_v_subwindow = GLUI_Master.create_glui_subwindow(main_window,
	GLUI_SUBWINDOW_RIGHT);

//  Create GLUI horizontal subwindow (placed on bottom)
	glui_h_subwindow = GLUI_Master.create_glui_subwindow(main_window,
	GLUI_SUBWINDOW_BOTTOM);

//---------------------------------------------------------------------
// 'Object Properties' Panel
//---------------------------------------------------------------------

//  Add the 'Object Properties' Panel to the GLUI vertical subwindow
	GLUI_Panel *op_panel = glui_v_subwindow->add_panel("Shading Options");

//  Add the Color listbox to the 'Object Properties' Panel
	GLUI_Listbox *shading_listbox = glui_v_subwindow->add_listbox_to_panel(
			op_panel, "", &listbox_item_id, SHADING_LISTBOX, glui_callback);

//  Add the items to the listbox
	shading_listbox->add_item(1, "Smooth Shaded");
	shading_listbox->add_item(2, "Flat Shaded");
	shading_listbox->add_item(3, "Wireframe");
	shading_listbox->add_item(4, "Shaded Edge");

//  Select the Smooth shaded by default
	shading_listbox->set_int_val(1);

//  Add a separator
	glui_v_subwindow->add_separator();
	glui_v_subwindow->add_separator();

	GLUI_Panel *user_panel = glui_v_subwindow->add_panel("User Operations");

	glui_v_subwindow->add_edittext_to_panel(user_panel, "smf_files/",
	GLUI_EDITTEXT_TEXT, filetext, FILE_NAME, glui_callback);

	glui_v_subwindow->add_separator_to_panel(user_panel);
//---------------------------------------------------------------------
// Buttons
//---------------------------------------------------------------------
	glui_v_subwindow->add_button_to_panel(user_panel, "Open", OPEN_BUTTON,
			glui_callback);
	glui_v_subwindow->add_button_to_panel(user_panel, "Save", SAVE_BUTTON,
			glui_callback);
	glui_v_subwindow->add_button_to_panel(user_panel, "Reset", RESET,
			glui_callback);
	glui_v_subwindow->add_button_to_panel(user_panel, "Quit", QUIT_BUTTON,
			glui_callback);

//---------------------------------------------------------------------
// 'Transformation' Panel
//---------------------------------------------------------------------

//  Create transformation panel 1 that will contain the Translation controls
	GLUI_Panel *transformation_panel = glui_h_subwindow->add_panel("");

//  Add the xy translation control
	GLUI_Translation *translation_xy =
			glui_h_subwindow->add_translation_to_panel(transformation_panel,
					"Translation XY", GLUI_TRANSLATION_XY, translate_xy,
					TRANSLATION_XY, glui_callback);

//  Set the translation speed
	translation_xy->set_speed(0.005);

	//  Add column, but don't draw it
	glui_h_subwindow->add_column_to_panel(transformation_panel, false);

	//  Add the x translation control
	GLUI_Translation *translation_x =
			glui_h_subwindow->add_translation_to_panel(transformation_panel,
					"Translation X", GLUI_TRANSLATION_X, &translate_x,
					TRANSLATION_X, glui_callback);

	//  Set the translation speed
	translation_x->set_speed(0.005);
	//  Add column, but don't draw it
	glui_h_subwindow->add_column_to_panel(transformation_panel, false);
	//  Add the y translation control
	GLUI_Translation *translation_y =
			glui_h_subwindow->add_translation_to_panel(transformation_panel,
					"Translation Y", GLUI_TRANSLATION_Y, &translate_y,
					TRANSLATION_Y, glui_callback);

	//  Set the translation speed
	translation_y->set_speed(0.005);
	//  Add column, but don't draw it
	glui_h_subwindow->add_column_to_panel(transformation_panel, false);
	//  Add the z translation control
	GLUI_Translation *translation_z =
			glui_h_subwindow->add_translation_to_panel(transformation_panel,
					"Translation Z", GLUI_TRANSLATION_Z, &translate_z,
					TRANSLATION_Z, glui_callback);

	//  Set the translation speed
	translation_z->set_speed(0.005);

	//  Add column, but don't draw it
	glui_h_subwindow->add_column(false);

	//  Create transformation panel 2 that will contain the rotation and spinner controls
	GLUI_Panel *rotation_panel = glui_h_subwindow->add_panel("");

	//  Add the rotation control
	glui_h_subwindow->add_rotation_to_panel(rotation_panel, "Rotation",
			rotation_matrix, ROTATION, glui_callback);

	//  Add column, but don't draw it
	glui_h_subwindow->add_column_to_panel(rotation_panel, false);

	//  Add the scale spinner
	GLUI_Spinner *spinner = glui_h_subwindow->add_spinner_to_panel(
			rotation_panel, "Scale", GLUI_SPINNER_FLOAT, &scale, SCALE_SPINNER,
			glui_callback);

	//  Set the limits for the spinner
	spinner->set_float_limits(-4.0, 4.0);

	//  set the alignment of the spinner control
	spinner->set_alignment(GLUI_ALIGN_CENTER);
	spinner->set_speed(0.5);

	//  Let the GLUI vertical subwindow know where its main graphics window is
	glui_v_subwindow->set_main_gfx_window(main_window);
	//  Let the GLUI horizontal subwindow know where its main graphics window is
	glui_h_subwindow->set_main_gfx_window(main_window);

	return 1;

}

int setupLighting() {
	/****************************************/
	/*       Set up OpenGL lights           */
	/****************************************/
	// set up OpenGL lights
	glEnable(GL_LIGHTING);
	glEnable( GL_NORMALIZE);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

	glEnable(GL_LIGHT1);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);

	/****************************************/
	/*          Enable z-buferring          */
	/****************************************/

	glEnable(GL_DEPTH_TEST);

	return 1;

}

//*************************************************************************
//  Program main method and entry point.
//*************************************************************************
int main(int argc, char **argv) {

	// load file
	runSmfHandler();
	// initialize glut
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	centerOnScreen();
	glutInitWindowPosition(window_x, window_y);
	glutInitWindowSize(window_width, window_height);
	main_window = glutCreateWindow("Smf Viewer");

	// Set the GLUT callback functions
	glutDisplayFunc(display);
	GLUI_Master.set_glutReshapeFunc(reshape);
	GLUI_Master.set_glutKeyboardFunc(keyboard);
	GLUI_Master.set_glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutPassiveMotionFunc(pmotion);

	//  Setup all GLUI stuff
	setupGLUI();
	// Setup lightning
	setupLighting();
	//  Start GLUT event processing loop
	glutMainLoop();
	// free handler
	free(handler);
	return 0;
}

