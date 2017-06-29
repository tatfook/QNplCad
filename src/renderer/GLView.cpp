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
GLView::GLView(const QGLFormat& format, QWidget *parent)
    : QGLWidget(format, parent)
{
	m_camera = new Camera();
}

GLView::~GLView()
{
	delete m_camera;
	delete m_mesh;
}
void GLView::loadMesh(Mesh* m)
{
	m_mesh = new GLMesh(m);
	delete m;
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

	QColor bgcol(255, 0, 0, 255);
	QColor axescolor(128, 128, 128, 255);
	float r = bgcol.redF();
	float g = bgcol.greenF();
	float b = bgcol.blueF();
	glClearColor(r, g, b, 1.0);
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

		QMatrix4x4 mvp = proj * trans * rot_x * rot_y * rot_z * view;

		//bind
		mesh_shader.bind();

		glUniformMatrix4fv(mesh_shader.uniformLocation("mvp"), 1, GL_FALSE, mvp.data());

		// Find and enable the attribute location for vertex position
		const GLuint vp = mesh_shader.attributeLocation("vertex_position");
		glEnableVertexAttribArray(vp);

		// Then draw the mesh with that vertex position
		m_mesh->draw(vp);

		// Clean up state machine
		glDisableVertexAttribArray(vp);
		mesh_shader.release();
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
		if (event->buttons() & Qt::LeftButton
#ifdef Q_OS_MAC
			&& !(event->modifiers() & Qt::MetaModifier)
#endif
			) {
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

}

void GLView::showSmallaxes(const QColor &col)
{

}

void GLView::showScalemarkers(const QColor &col)
{

}

void GLView::decodeMarkerValue(double i, double l, int size_div_sm)
{

}
