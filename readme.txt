User guide:
All features has been implemented.
1.Disply in various mode:
	a. Smooth Shaded
	b. Flat Shaded
	c. Wireframe
	d. Flat sheded edge.
2. For opening .smf file, in text box above open button write filename e.g sphere.smf, sphere, bigsmile etc. 
Sample files has been placed under smf_files directory. If you want to test with more .smf file please copy that to /smf_files directory under smf_view project.
3. Save button will create new file under /smf_files directory by appending _copy e.g on save of sphere, it will create sphere_copy.smf
4. You can apply translation in XY, X, Y, Z, rotation and scaling effect using appropriate controls in lower side of UI. 
5. Use Reset button to reset effect of rotation, translation and scaling.
6. Quit will exit.(Can use esc,q,Q buttons)

For Compile:
make

Run:
./smf_view

References:
1. http://what-when-how.com/advanced-methods-in-computer-graphics/mesh-processing-advanced-methods-in-computer-graphics-part-2/
2. http://www.cs.mtu.edu/~shene/COURSES/cs3621/NOTES/model/winged-e.html
3. http://www.codeproject.com/Articles/20286/GLUI-Window-Template
4. example2.cpp and example5.cpp from offical glui website.

