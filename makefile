all:
	g++ ./SmfViewer.cpp ./WE/*.cpp ./SmfHandler.cpp -o smf_view -lglui -lglut -lGLU -lGL


