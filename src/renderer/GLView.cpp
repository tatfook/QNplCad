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
	m_camera = new Camera(45, 1.75, 0.1f, 150.0f);
}

GLView::~GLView()
{
	delete m_camera;
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

	QMatrix4x4 transformation;
	transformation.translate(QVector3D(10, 10, 10));

	QMatrix4x4 projection_matrix = m_camera->projectionMatrix();
	QMatrix4x4 view_matrix = m_camera->viewMatrix();
	mesh_shader.bind();

	glUniformMatrix4fv(mesh_shader.uniformLocation("projection_matrix"), 1, GL_FALSE, projection_matrix.data());
	glUniformMatrix4fv(mesh_shader.uniformLocation("view_matrix"), 1, GL_FALSE, view_matrix.data());

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

void GLView::mousePressEvent(QMouseEvent* event)
{
}

void GLView::mouseReleaseEvent(QMouseEvent* event)
{
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
	float aspectratio = 1.0*width / height;
	m_camera->setAspectRatio(aspectratio);
}
