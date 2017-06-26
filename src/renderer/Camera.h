 #pragma once

 /*

 Camera

 For usage, see QGLView.cc, GLView.cc, export_png.cc, openscad.cc

 There are two different types of cameras represented in this class:

 *Gimbal camera - uses Euler Angles, object translation, and viewer distance
 *Vector camera - uses 'eye', 'center', and 'up' vectors ('lookat' style)

 They are not necessarily kept in sync. There are two modes of
 projection, Perspective and Orthogonal.

 */

 #include <vector>
#include <QVector3D>

 class Camera
 {
 public:
 	enum CameraType { NONE, GIMBAL, VECTOR } type;
 	enum ProjectionType { ORTHOGONAL, PERSPECTIVE } projection;
 	Camera(enum CameraType camtype = NONE);
 	void setup(std::vector<double> params);
 	void gimbalDefaultTranslate();
 	void setProjection(ProjectionType type);
 	void zoom(int delta);
 	double zoomValue();
 	void resetView();
 	//void viewAll(const BoundingBox &bbox);
 	std::string statusText();

 	// Vectorcam
	QVector3D eye;
	QVector3D center; // (aka 'target')
	QVector3D up; // not used currently

 						// Gimbalcam
	QVector3D object_trans;
	QVector3D object_rot;

 	// Perspective settings
 	double fov; // Field of view

 				// true if camera should try to view everything in a given
 				// bounding box.
 	bool viewall;

 	// true if camera should point at center of bounding box
 	// (normally it points at 0,0,0 or at given coordinates)
 	bool autocenter;

 	unsigned int pixel_width;
 	unsigned int pixel_height;

 protected:
 	// Perspective settings
 	double viewer_distance;
 	// Orthographic settings
 	double height; // world-space height of viewport
 };
