#include <QMouseEvent>
#include <QDebug>
#include <QMatrix3x3>
#include <QMatrix4x4>
#include <QVector3D>

#include <cmath>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "GLView.h"
#include "backdrop.h"
#include "GLMesh.h"
#include "Mesh.h"
#include "BoundingBox.h"
#include <sstream>  
GLView::GLView(const QGLFormat& format, QWidget *parent)
    : QGLWidget(format, parent)
	, bgcol(QColor(149, 194, 228, 255))
	, axescolor(QColor(0, 0, 0, 255))
	, m_camera(new Camera())
{
}

GLView::~GLView()
{
	delete m_camera;
	delete m_mesh;
}
void GLView::loadMesh(Mesh* m){
	m_mesh = new GLMesh(m);
	viewAll();
	delete m;
}

void GLView::viewAll()
{
	m_camera->object_trans = -m_mesh->box.getCenter();
	m_camera->viewAll(m_mesh->box);
}

void GLView::initializeGL()
{
    initializeGLFunctions();

    mesh_shader.addShaderFromSourceFile(QGLShader::Vertex, ":res/gl/mesh.vert");
    mesh_shader.addShaderFromSourceFile(QGLShader::Fragment, ":res/gl/mesh.frag");
    mesh_shader.link();
}

void GLView::paintGL()
{
	glDisable(GL_LIGHTING);

	
	glClearColor(bgcol.redF(), bgcol.greenF(), bgcol.blueF(), 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	if (m_mesh)
	{
		double dist = m_camera->zoomValue();
		QMatrix4x4 proj;
		proj.perspective(m_camera->fov, aspectratio, 0.1*dist, 100 * dist);

		QMatrix4x4 view;
		QVector3D eye(0.0, -dist, 0.0);
		QVector3D center(0.0, 0.0, 0.0);
		QVector3D up(0.0, 0.0, 1.0);
		view.lookAt(eye, center, up);

		QMatrix4x4 rot_x;		rot_x.rotate(m_camera->object_rot.x(), QVector3D(1.0, 0.0, 0.0));
		QMatrix4x4 rot_y;		rot_y.rotate(m_camera->object_rot.y(), QVector3D(0.0, 1.0, 0.0));
		QMatrix4x4 rot_z;		rot_z.rotate(m_camera->object_rot.z(), QVector3D(0.0, 0.0, 1.0));


		QMatrix4x4 trans;		trans.translate(m_camera->object_trans);

		QMatrix4x4 mvp = proj * view * trans * rot_x * rot_y * rot_z;

		//bind
		mesh_shader.bind();

		glUniformMatrix4fv(mesh_shader.uniformLocation("mvp"), 1, GL_FALSE, mvp.data());

		// Find and enable the attribute location for vertex position
		const GLuint vp = mesh_shader.attributeLocation("vertex_position");
		glEnableVertexAttribArray(vp);

		// Then draw the mesh with that vertex position
		m_mesh->draw(vp);

		showAxes(axescolor);
		showScalemarkers(axescolor);

		// Clean up state machine
		glDisableVertexAttribArray(vp);
		mesh_shader.release();

		
		showSmallaxes(axescolor);
	}
}

void GLView::mousePressEvent(QMouseEvent* event)
{
	mouse_drag_active = true;
	last_mouse = event->globalPos();
}

void GLView::mouseReleaseEvent(QMouseEvent* event)
{
	mouse_drag_active = false;
	releaseMouse();
}

void GLView::normalizeAngle(float& angle)
{
	while (angle < 0)
		angle += 360;
	while (angle > 360)
		angle -= 360;
}
void GLView::mouseMoveEvent(QMouseEvent* event)
{
	QPoint this_mouse = event->globalPos();
	double dx = (this_mouse.x() - last_mouse.x()) * 0.7;
	double dy = (this_mouse.y() - last_mouse.y()) * 0.7;
	if (mouse_drag_active) {
		if (event->buttons() & Qt::RightButton) {
			// Left button rotates in xz, Shift-left rotates in xy
			// On Mac, Ctrl-Left is handled as right button on other platforms
			m_camera->object_rot[0] += dy;
			if ((QApplication::keyboardModifiers() & Qt::ShiftModifier) != 0)
				m_camera->object_rot[1] += dx;
			else
				m_camera->object_rot[2] += dx;

			normalizeAngle(m_camera->object_rot[0]);
			normalizeAngle(m_camera->object_rot[1]);
			normalizeAngle(m_camera->object_rot[2]);
		}
		else {
			// Right button pans in the xz plane
			// Middle button pans in the xy plane
			// Shift-right and Shift-middle zooms
			if ((QApplication::keyboardModifiers() & Qt::ShiftModifier) != 0) {
				m_camera->zoom(-12.0 * dy);
			}
			else {

				double mx = +(dx) * 3.0 * m_camera->zoomValue() / QWidget::width();
				double mz = -(dy) * 3.0 * m_camera->zoomValue() / QWidget::height();

				double my = 0;
#if (QT_VERSION < QT_VERSION_CHECK(4, 7, 0))
				if (event->buttons() & Qt::MidButton) {
#else
				if (event->buttons() & Qt::MiddleButton) {
#endif
					my = mz;
					mz = 0;
					// actually lock the x-position
					// (turns out to be easier to use than xy panning)
					mx = 0;
				}

				QMatrix3x3 aax, aay, aaz, tm3;
				aax = QQuaternion::fromAxisAndAngle(1, 0, 0, -(m_camera->object_rot.x() / 180) * M_PI).toRotationMatrix();
				aay = QQuaternion::fromAxisAndAngle(0, 1, 0, -(m_camera->object_rot.y() / 180) * M_PI).toRotationMatrix();
				aaz = QQuaternion::fromAxisAndAngle(0, 0, 0, -(m_camera->object_rot.z() / 180) * M_PI).toRotationMatrix();
				tm3.setToIdentity();
				tm3 = aaz * (aay * (aax * tm3));

				QMatrix4x4 tm;
				tm.setToIdentity();
				for (int i = 0; i < 3; i++) for (int j = 0; j < 3; j++) tm(j, i) = tm3(j, i);

				QMatrix4x4 vec(
					0, 0, 0, mx,
					0, 0, 0, my,
					0, 0, 0, mz,
					0, 0, 0, 1
				);
				tm = tm * vec;
				m_camera->object_trans[0] += tm(0, 3);
				m_camera->object_trans[1] += tm(1, 3);
				m_camera->object_trans[2] += tm(2, 3);
				printf("object_trans:%f %f %f\n", m_camera->object_trans.x(), m_camera->object_trans.y(), m_camera->object_trans.z());
				}
			}
		update();
		}
	last_mouse = this_mouse;
}
void GLView::wheelEvent(QWheelEvent *event)
{
	
#if QT_VERSION >= 0x050000
	m_camera->zoom(event->angleDelta().y());
#else
	m_camera->zoom(event->delta());
#endif
	update();
}

void GLView::resizeGL(int width, int height)
{
	glViewport(0, 0, width, height);
	aspectratio = 1.0*width / height;
	
}
void GLView::showCrosshairs()
{

}

void GLView::showAxes(const QColor &col)
{
	double l = m_camera->zoomValue();

	// FIXME: doesn't work under Vector Camera
	// Large gray axis cross inline with the model
	glLineWidth(this->getDPI());
	glColor3f(col.red(), col.green(), col.blue());

	glBegin(GL_LINES);
	glVertex3d(0, 0, 0);
	glVertex3d(+l, 0, 0);
	glVertex3d(0, 0, 0);
	glVertex3d(0, +l, 0);
	glVertex3d(0, 0, 0);
	glVertex3d(0, 0, +l);
	glEnd();

	glPushAttrib(GL_LINE_BIT);
	glEnable(GL_LINE_STIPPLE);
	glLineStipple(3, 0xAAAA);
	glBegin(GL_LINES);
	glVertex3d(0, 0, 0);
	glVertex3d(-l, 0, 0);
	glVertex3d(0, 0, 0);
	glVertex3d(0, -l, 0);
	glVertex3d(0, 0, 0);
	glVertex3d(0, 0, -l);
	glEnd();
	glPopAttrib();
}

void GLView::showSmallaxes(const QColor &col)
{
	// Fixme - this doesnt work in Vector Camera mode

	float dpi = this->getDPI();
	// Small axis cross in the lower left corner
	glDepthFunc(GL_ALWAYS);

	// Set up an orthographic projection of the axis cross in the corner
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glTranslatef(-0.8f, -0.8f, 0.0f);
	double scale = 90;
	glOrtho(-scale*dpi*aspectratio, scale*dpi*aspectratio,
		-scale*dpi, scale*dpi,
		-scale*dpi, scale*dpi);
	gluLookAt(0.0, -1.0, 0.0,
		0.0, 0.0, 0.0,
		0.0, 0.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotated(m_camera->object_rot.x(), 1.0, 0.0, 0.0);
	glRotated(m_camera->object_rot.y(), 0.0, 1.0, 0.0);
	glRotated(m_camera->object_rot.z(), 0.0, 0.0, 1.0);

	glLineWidth(dpi);
	glBegin(GL_LINES);
	glColor3d(1.0, 0.0, 0.0);
	glVertex3d(0, 0, 0); glVertex3d(10 * dpi, 0, 0);
	glColor3d(0.0, 1.0, 0.0);
	glVertex3d(0, 0, 0); glVertex3d(0, 10 * dpi, 0);
	glColor3d(0.0, 0.0, 1.0);
	glVertex3d(0, 0, 0); glVertex3d(0, 0, 10 * dpi);
	glEnd();

	GLdouble mat_model[16];
	glGetDoublev(GL_MODELVIEW_MATRIX, mat_model);

	GLdouble mat_proj[16];
	glGetDoublev(GL_PROJECTION_MATRIX, mat_proj);

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	GLdouble xlabel_x, xlabel_y, xlabel_z;
	gluProject(12 * dpi, 0, 0, mat_model, mat_proj, viewport, &xlabel_x, &xlabel_y, &xlabel_z);
	xlabel_x = std::round(xlabel_x); xlabel_y = std::round(xlabel_y);

	GLdouble ylabel_x, ylabel_y, ylabel_z;
	gluProject(0, 12 * dpi, 0, mat_model, mat_proj, viewport, &ylabel_x, &ylabel_y, &ylabel_z);
	ylabel_x = std::round(ylabel_x); ylabel_y = std::round(ylabel_y);

	GLdouble zlabel_x, zlabel_y, zlabel_z;
	gluProject(0, 0, 12 * dpi, mat_model, mat_proj, viewport, &zlabel_x, &zlabel_y, &zlabel_z);
	zlabel_x = std::round(zlabel_x); zlabel_y = std::round(zlabel_y);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glTranslated(-1, -1, 0);
	glScaled(2.0 / viewport[2], 2.0 / viewport[3], 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glColor3f(col.red(), col.green(), col.blue());

	float d = 3 * dpi;
	glBegin(GL_LINES);
	// X Label
	glVertex3d(xlabel_x - d, xlabel_y - d, 0); glVertex3d(xlabel_x + d, xlabel_y + d, 0);
	glVertex3d(xlabel_x - d, xlabel_y + d, 0); glVertex3d(xlabel_x + d, xlabel_y - d, 0);
	// Y Label
	glVertex3d(ylabel_x - d, ylabel_y - d, 0); glVertex3d(ylabel_x + d, ylabel_y + d, 0);
	glVertex3d(ylabel_x - d, ylabel_y + d, 0); glVertex3d(ylabel_x, ylabel_y, 0);
	// Z Label
	glVertex3d(zlabel_x - d, zlabel_y - d, 0); glVertex3d(zlabel_x + d, zlabel_y - d, 0);
	glVertex3d(zlabel_x - d, zlabel_y + d, 0); glVertex3d(zlabel_x + d, zlabel_y + d, 0);
	glVertex3d(zlabel_x - d, zlabel_y - d, 0); glVertex3d(zlabel_x + d, zlabel_y + d, 0);
	// FIXME - depends on gimbal camera 'viewer distance'.. how to fix this
	//         for VectorCamera?
	glEnd();
}

void GLView::showScalemarkers(const QColor &col)
{
	// Add scale tics on large axes
	double l = m_camera->zoomValue();
	glLineWidth(this->getDPI());
	glColor3f(col.redF(), col.greenF(), col.blueF());

	// determine the log value to provide proportional tics
	int log_l = (int)log10(l);

	// j represents the increment for each minor tic
	double j = 10;
	// deal with 0 log values
	if (l < 1.5) {
		j = pow(10, log_l - 2);
	}
	else {
		j = pow(10, log_l - 1);
	}

	int size_div_sm = 60;       // divisor for l to determine minor tic size
	int size_div = size_div_sm;
	int line_cnt = 0;

	for (double i = 0; i<l; i += j) {      // i represents the position along the axis
		if (line_cnt++ == 10) {      // major tic
			size_div = size_div_sm * .5; // resize to a major tic
			line_cnt = 1;                // reset the major tic counter
			decodeMarkerValue(i, l, size_div_sm);    // print number
		}
		else {                    // minor tic
			size_div = size_div_sm;      // set the minor tic to the standard size
		}

		/*
		* The length of each tic is proportional to the length of the axis
		* (which changes with the zoom value.)  l/size_div provides the
		* proportional length
		*
		* Commented glVertex3d lines provide additional 'arms' for the tic
		* the number of arms will (hopefully) eventually be driven via Preferences
		*/

		// positive axes
		glBegin(GL_LINES);
		// x
		glVertex3d(i, 0, 0); glVertex3d(i, -l / size_div, 0); // 1 arm
															  //glVertex3d(i,-l/size_div,0); glVertex3d(i,l/size_div,0); // 2 arms
															  //glVertex3d(i,0,-l/size_div); glVertex3d(i,0,l/size_div); // 4 arms (w/ 2 arms line)

															  // y
		glVertex3d(0, i, 0); glVertex3d(-l / size_div, i, 0); // 1 arm
															  //glVertex3d(-l/size_div,i,0); glVertex3d(l/size_div,i,0); // 2 arms
															  //glVertex3d(0,i,-l/size_div); glVertex3d(0,i,l/size_div); // 4 arms (w/ 2 arms line)

															  // z
		glVertex3d(0, 0, i); glVertex3d(-l / size_div, 0, i); // 1 arm
															  //glVertex3d(-l/size_div,0,i); glVertex3d(l/size_div,0,i); // 2 arms
															  //glVertex3d(0,-l/size_div,i); glVertex3d(0,l/size_div,i); // 4 arms (w/ 2 arms line)
		glEnd();

		// negative axes
		glPushAttrib(GL_LINE_BIT);
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(3, 0xAAAA);
		glBegin(GL_LINES);
		// x
		glVertex3d(-i, 0, 0); glVertex3d(-i, -l / size_div, 0); // 1 arm
																//glVertex3d(-i,-l/size_div,0); glVertex3d(-i,l/size_div,0); // 2 arms
																//glVertex3d(-i,0,-l/size_div); glVertex3d(-i,0,l/size_div); // 4 arms (w/ 2 arms line)

																// y
		glVertex3d(0, -i, 0); glVertex3d(-l / size_div, -i, 0); // 1 arm
																//glVertex3d(-l/size_div,-i,0); glVertex3d(l/size_div,-i,0); // 2 arms
																//glVertex3d(0,-i,-l/size_div); glVertex3d(0,-i,l/size_div); // 4 arms (w/ 2 arms line)

																// z
		glVertex3d(0, 0, -i); glVertex3d(-l / size_div, 0, -i); // 1 arm
																//glVertex3d(-l/size_div,0,-i); glVertex3d(l/size_div,0,-i); // 2 arms
																//glVertex3d(0,-l/size_div,-i); glVertex3d(0,l/size_div,-i); // 4 arms (w/ 2 arms line)
		glEnd();
		glPopAttrib();
	}
}

void GLView::decodeMarkerValue(double i, double l, int size_div_sm)
{
	// convert the axis position to a string
	std::ostringstream oss;
	oss << i;
	std::string digit = oss.str();

	// setup how far above the axis (or tic TBD) to draw the number
	double dig_buf = (l / size_div_sm) / 4;
	// setup the size of the character box
	double dig_w = (l / size_div_sm) / 2;
	double dig_h = (l / size_div_sm) + dig_buf;
	// setup the distance between characters
	double kern = dig_buf;
	double dig_wk = (dig_w)+kern;

	// set up ordering for different axes
	int ax[6][3] = {
		{ 0,1,2 },
		{ 1,0,2 },
		{ 1,2,0 },
		{ 0,1,2 },
		{ 1,0,2 },
		{ 1,2,0 } };

	// set up character vertex seqeunces for different axes
	int or_2[6][6] = {
		{ 0,1,3,2,4,5 },
		{ 1,0,2,3,5,4 },
		{ 1,0,2,3,5,4 },
		{ 1,0,2,3,5,4 },
		{ 0,1,3,2,4,5 },
		{ 0,1,3,2,4,5 } };

	int or_3[6][7] = {
		{ 0,1,3,2,3,5,4 },
		{ 1,0,2,3,2,4,5 },
		{ 1,0,2,3,2,4,5 },
		{ 1,0,2,3,2,4,5 },
		{ 0,1,3,2,3,5,4 },
		{ 0,1,3,2,3,5,4 } };

	int or_4[6][5] = {
		{ 0,2,3,1,5 },
		{ 1,3,2,0,4 },
		{ 1,3,2,0,4 },
		{ 1,3,2,0,4 },
		{ 0,2,3,1,5 },
		{ 0,2,3,1,5 } };

	int or_5[6][6] = {
		{ 1,0,2,3,5,4 },
		{ 0,1,3,2,4,5 },
		{ 0,1,3,2,4,5 },
		{ 0,1,3,2,4,5 },
		{ 1,0,2,3,5,4 },
		{ 1,0,2,3,5,4 } };

	int or_6[6][6] = {
		{ 1,0,4,5,3,2 },
		{ 0,1,5,4,2,3 },
		{ 0,1,5,4,2,3 },
		{ 0,1,5,4,2,3 },
		{ 1,0,4,5,3,2 },
		{ 1,0,4,5,3,2 } };

	int or_7[6][3] = {
		{ 0,1,4 },
		{ 1,0,5 },
		{ 1,0,5 },
		{ 1,0,5 },
		{ 0,1,4 },
		{ 0,1,4 } };

	int or_9[6][5] = {
		{ 5,1,0,2,3 },
		{ 4,0,1,3,2 },
		{ 4,0,1,3,2 },
		{ 4,0,1,3,2 },
		{ 5,1,0,2,3 },
		{ 5,1,0,2,3 } };

	int or_e[6][7] = {
		{ 1,0,2,3,2,4,5 },
		{ 0,1,3,2,3,5,4 },
		{ 0,1,3,2,3,5,4 },
		{ 0,1,3,2,3,5,4 },
		{ 1,0,2,3,2,4,5 },
		{ 1,0,2,3,2,4,5 } };

	std::string stash_digit = digit;

	// walk through axes
	for (int di = 0; di < 6; di++) {

		// setup negative axes
		double polarity = 1;
		if (di > 2) {
			polarity = -1;
			digit = "-" + stash_digit;
		}

		// fix the axes that need to run the opposite direction
		if (di > 0 && di < 4) {
			std::reverse(digit.begin(), digit.end());
		}

		// walk through and render the characters of the string
		for (std::string::size_type char_num = 0; char_num < digit.size(); ++char_num) {
			// setup the vertices for the char rendering based on the axis and position
			double dig_vrt[6][3] = {
				{ polarity*((i + ((char_num)*dig_wk)) - (dig_w / 2)),dig_h,0 },
				{ polarity*((i + ((char_num)*dig_wk)) + (dig_w / 2)),dig_h,0 },
				{ polarity*((i + ((char_num)*dig_wk)) - (dig_w / 2)),dig_h / 2 + dig_buf,0 },
				{ polarity*((i + ((char_num)*dig_wk)) + (dig_w / 2)),dig_h / 2 + dig_buf,0 },
				{ polarity*((i + ((char_num)*dig_wk)) - (dig_w / 2)),dig_buf,0 },
				{ polarity*((i + ((char_num)*dig_wk)) + (dig_w / 2)),dig_buf,0 } };

			// convert the char into lines appropriate for the axis being used
			// psuedo 7 segment vertices are:
			// A--B
			// |  |
			// C--D
			// |  |
			// E--F
			switch (digit[char_num]) {
			case '1':
				glBegin(GL_LINES);
				glVertex3d(dig_vrt[0][ax[di][0]], dig_vrt[0][ax[di][1]], dig_vrt[0][ax[di][2]]);  //a
				glVertex3d(dig_vrt[4][ax[di][0]], dig_vrt[4][ax[di][1]], dig_vrt[4][ax[di][2]]);  //e
				glEnd();
				break;

			case '2':
				glBegin(GL_LINE_STRIP);
				glVertex3d(dig_vrt[or_2[di][0]][ax[di][0]], dig_vrt[or_2[di][0]][ax[di][1]], dig_vrt[or_2[di][0]][ax[di][2]]);  //a
				glVertex3d(dig_vrt[or_2[di][1]][ax[di][0]], dig_vrt[or_2[di][1]][ax[di][1]], dig_vrt[or_2[di][1]][ax[di][2]]);  //b
				glVertex3d(dig_vrt[or_2[di][2]][ax[di][0]], dig_vrt[or_2[di][2]][ax[di][1]], dig_vrt[or_2[di][2]][ax[di][2]]);  //d
				glVertex3d(dig_vrt[or_2[di][3]][ax[di][0]], dig_vrt[or_2[di][3]][ax[di][1]], dig_vrt[or_2[di][3]][ax[di][2]]);  //c
				glVertex3d(dig_vrt[or_2[di][4]][ax[di][0]], dig_vrt[or_2[di][4]][ax[di][1]], dig_vrt[or_2[di][4]][ax[di][2]]);  //e
				glVertex3d(dig_vrt[or_2[di][5]][ax[di][0]], dig_vrt[or_2[di][5]][ax[di][1]], dig_vrt[or_2[di][5]][ax[di][2]]);  //f
				glEnd();
				break;

			case '3':
				glBegin(GL_LINE_STRIP);
				glVertex3d(dig_vrt[or_3[di][0]][ax[di][0]], dig_vrt[or_3[di][0]][ax[di][1]], dig_vrt[or_3[di][0]][ax[di][2]]);  //a
				glVertex3d(dig_vrt[or_3[di][1]][ax[di][0]], dig_vrt[or_3[di][1]][ax[di][1]], dig_vrt[or_3[di][1]][ax[di][2]]);  //b
				glVertex3d(dig_vrt[or_3[di][2]][ax[di][0]], dig_vrt[or_3[di][2]][ax[di][1]], dig_vrt[or_3[di][2]][ax[di][2]]);  //d
				glVertex3d(dig_vrt[or_3[di][3]][ax[di][0]], dig_vrt[or_3[di][3]][ax[di][1]], dig_vrt[or_3[di][3]][ax[di][2]]);  //c
				glVertex3d(dig_vrt[or_3[di][4]][ax[di][0]], dig_vrt[or_3[di][4]][ax[di][1]], dig_vrt[or_3[di][4]][ax[di][2]]);  //d
				glVertex3d(dig_vrt[or_3[di][5]][ax[di][0]], dig_vrt[or_3[di][5]][ax[di][1]], dig_vrt[or_3[di][5]][ax[di][2]]);  //f
				glVertex3d(dig_vrt[or_3[di][6]][ax[di][0]], dig_vrt[or_3[di][6]][ax[di][1]], dig_vrt[or_3[di][6]][ax[di][2]]);  //e
				glEnd();
				break;

			case '4':
				glBegin(GL_LINE_STRIP);
				glVertex3d(dig_vrt[or_4[di][0]][ax[di][0]], dig_vrt[or_4[di][0]][ax[di][1]], dig_vrt[or_4[di][0]][ax[di][2]]);  //a
				glVertex3d(dig_vrt[or_4[di][1]][ax[di][0]], dig_vrt[or_4[di][1]][ax[di][1]], dig_vrt[or_4[di][1]][ax[di][2]]);  //c
				glVertex3d(dig_vrt[or_4[di][2]][ax[di][0]], dig_vrt[or_4[di][2]][ax[di][1]], dig_vrt[or_4[di][2]][ax[di][2]]);  //d
				glVertex3d(dig_vrt[or_4[di][3]][ax[di][0]], dig_vrt[or_4[di][3]][ax[di][1]], dig_vrt[or_4[di][3]][ax[di][2]]);  //b
				glVertex3d(dig_vrt[or_4[di][4]][ax[di][0]], dig_vrt[or_4[di][4]][ax[di][1]], dig_vrt[or_4[di][4]][ax[di][2]]);  //f
				glEnd();
				break;

			case '5':
				glBegin(GL_LINE_STRIP);
				glVertex3d(dig_vrt[or_5[di][0]][ax[di][0]], dig_vrt[or_5[di][0]][ax[di][1]], dig_vrt[or_5[di][0]][ax[di][2]]);  //b
				glVertex3d(dig_vrt[or_5[di][1]][ax[di][0]], dig_vrt[or_5[di][1]][ax[di][1]], dig_vrt[or_5[di][1]][ax[di][2]]);  //a
				glVertex3d(dig_vrt[or_5[di][2]][ax[di][0]], dig_vrt[or_5[di][2]][ax[di][1]], dig_vrt[or_5[di][2]][ax[di][2]]);  //c
				glVertex3d(dig_vrt[or_5[di][3]][ax[di][0]], dig_vrt[or_5[di][3]][ax[di][1]], dig_vrt[or_5[di][3]][ax[di][2]]);  //d
				glVertex3d(dig_vrt[or_5[di][4]][ax[di][0]], dig_vrt[or_5[di][4]][ax[di][1]], dig_vrt[or_5[di][4]][ax[di][2]]);  //f
				glVertex3d(dig_vrt[or_5[di][5]][ax[di][0]], dig_vrt[or_5[di][5]][ax[di][1]], dig_vrt[or_5[di][5]][ax[di][2]]);  //e
				glEnd();
				break;

			case '6':
				glBegin(GL_LINE_STRIP);
				glVertex3d(dig_vrt[or_6[di][0]][ax[di][0]], dig_vrt[or_6[di][0]][ax[di][1]], dig_vrt[or_6[di][0]][ax[di][2]]);  //b
				glVertex3d(dig_vrt[or_6[di][1]][ax[di][0]], dig_vrt[or_6[di][1]][ax[di][1]], dig_vrt[or_6[di][1]][ax[di][2]]);  //a
				glVertex3d(dig_vrt[or_6[di][2]][ax[di][0]], dig_vrt[or_6[di][2]][ax[di][1]], dig_vrt[or_6[di][2]][ax[di][2]]);  //e
				glVertex3d(dig_vrt[or_6[di][3]][ax[di][0]], dig_vrt[or_6[di][3]][ax[di][1]], dig_vrt[or_6[di][3]][ax[di][2]]);  //f
				glVertex3d(dig_vrt[or_6[di][4]][ax[di][0]], dig_vrt[or_6[di][4]][ax[di][1]], dig_vrt[or_6[di][4]][ax[di][2]]);  //d
				glVertex3d(dig_vrt[or_6[di][5]][ax[di][0]], dig_vrt[or_6[di][5]][ax[di][1]], dig_vrt[or_6[di][5]][ax[di][2]]);  //c
				glEnd();
				break;

			case '7':
				glBegin(GL_LINE_STRIP);
				glVertex3d(dig_vrt[or_7[di][0]][ax[di][0]], dig_vrt[or_7[di][0]][ax[di][1]], dig_vrt[or_7[di][0]][ax[di][2]]);  //a
				glVertex3d(dig_vrt[or_7[di][1]][ax[di][0]], dig_vrt[or_7[di][1]][ax[di][1]], dig_vrt[or_7[di][1]][ax[di][2]]);  //b
				glVertex3d(dig_vrt[or_7[di][2]][ax[di][0]], dig_vrt[or_7[di][2]][ax[di][1]], dig_vrt[or_7[di][2]][ax[di][2]]);  //e
				glEnd();
				break;

			case '8':
				glBegin(GL_LINE_STRIP);
				glVertex3d(dig_vrt[2][ax[di][0]], dig_vrt[2][ax[di][1]], dig_vrt[2][ax[di][2]]);  //c
				glVertex3d(dig_vrt[3][ax[di][0]], dig_vrt[3][ax[di][1]], dig_vrt[3][ax[di][2]]);  //d
				glVertex3d(dig_vrt[1][ax[di][0]], dig_vrt[1][ax[di][1]], dig_vrt[1][ax[di][2]]);  //b
				glVertex3d(dig_vrt[0][ax[di][0]], dig_vrt[0][ax[di][1]], dig_vrt[0][ax[di][2]]);  //a
				glVertex3d(dig_vrt[4][ax[di][0]], dig_vrt[4][ax[di][1]], dig_vrt[4][ax[di][2]]);  //e
				glVertex3d(dig_vrt[5][ax[di][0]], dig_vrt[5][ax[di][1]], dig_vrt[5][ax[di][2]]);  //f
				glVertex3d(dig_vrt[3][ax[di][0]], dig_vrt[3][ax[di][1]], dig_vrt[3][ax[di][2]]);  //d
				glEnd();
				break;

			case '9':
				glBegin(GL_LINE_STRIP);
				glVertex3d(dig_vrt[or_9[di][0]][ax[di][0]], dig_vrt[or_9[di][0]][ax[di][1]], dig_vrt[or_9[di][0]][ax[di][2]]);  //f
				glVertex3d(dig_vrt[or_9[di][1]][ax[di][0]], dig_vrt[or_9[di][1]][ax[di][1]], dig_vrt[or_9[di][1]][ax[di][2]]);  //b
				glVertex3d(dig_vrt[or_9[di][2]][ax[di][0]], dig_vrt[or_9[di][2]][ax[di][1]], dig_vrt[or_9[di][2]][ax[di][2]]);  //a
				glVertex3d(dig_vrt[or_9[di][3]][ax[di][0]], dig_vrt[or_9[di][3]][ax[di][1]], dig_vrt[or_9[di][3]][ax[di][2]]);  //c
				glVertex3d(dig_vrt[or_9[di][4]][ax[di][0]], dig_vrt[or_9[di][4]][ax[di][1]], dig_vrt[or_9[di][4]][ax[di][2]]);  //d
				glEnd();
				break;

			case '0':
				glBegin(GL_LINE_LOOP);
				glVertex3d(dig_vrt[0][ax[di][0]], dig_vrt[0][ax[di][1]], dig_vrt[0][ax[di][2]]);  //a
				glVertex3d(dig_vrt[1][ax[di][0]], dig_vrt[1][ax[di][1]], dig_vrt[1][ax[di][2]]);  //b
				glVertex3d(dig_vrt[5][ax[di][0]], dig_vrt[5][ax[di][1]], dig_vrt[5][ax[di][2]]);  //f
				glVertex3d(dig_vrt[4][ax[di][0]], dig_vrt[4][ax[di][1]], dig_vrt[4][ax[di][2]]);  //e
				glEnd();
				break;

			case '-':
				glBegin(GL_LINES);
				glVertex3d(dig_vrt[2][ax[di][0]], dig_vrt[2][ax[di][1]], dig_vrt[2][ax[di][2]]);  //c
				glVertex3d(dig_vrt[3][ax[di][0]], dig_vrt[3][ax[di][1]], dig_vrt[3][ax[di][2]]);  //d
				glEnd();
				break;

			case '.':
				glBegin(GL_LINES);
				glVertex3d(dig_vrt[4][ax[di][0]], dig_vrt[4][ax[di][1]], dig_vrt[4][ax[di][2]]);  //e
				glVertex3d(dig_vrt[5][ax[di][0]], dig_vrt[5][ax[di][1]], dig_vrt[5][ax[di][2]]);  //f
				glEnd();
				break;

			case 'e':
				glBegin(GL_LINE_STRIP);
				glVertex3d(dig_vrt[or_e[di][0]][ax[di][0]], dig_vrt[or_e[di][0]][ax[di][1]], dig_vrt[or_e[di][0]][ax[di][2]]);  //b
				glVertex3d(dig_vrt[or_e[di][1]][ax[di][0]], dig_vrt[or_e[di][1]][ax[di][1]], dig_vrt[or_e[di][1]][ax[di][2]]);  //a
				glVertex3d(dig_vrt[or_e[di][2]][ax[di][0]], dig_vrt[or_e[di][2]][ax[di][1]], dig_vrt[or_e[di][2]][ax[di][2]]);  //c
				glVertex3d(dig_vrt[or_e[di][3]][ax[di][0]], dig_vrt[or_e[di][3]][ax[di][1]], dig_vrt[or_e[di][3]][ax[di][2]]);  //d
				glVertex3d(dig_vrt[or_e[di][4]][ax[di][0]], dig_vrt[or_e[di][4]][ax[di][1]], dig_vrt[or_e[di][4]][ax[di][2]]);  //c
				glVertex3d(dig_vrt[or_e[di][5]][ax[di][0]], dig_vrt[or_e[di][5]][ax[di][1]], dig_vrt[or_e[di][5]][ax[di][2]]);  //e
				glVertex3d(dig_vrt[or_e[di][6]][ax[di][0]], dig_vrt[or_e[di][6]][ax[di][1]], dig_vrt[or_e[di][6]][ax[di][2]]);  //f
				glEnd();
				break;

			}
		}
	}
}
