#include <QMouseEvent>
#include <QDebug>

#include <cmath>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "GLView.h"
#include "backdrop.h"
#include "GLMesh.h"
#include "Mesh.h"
GLView::GLView(const QGLFormat& format, QWidget *parent)
    : QGLWidget(format, parent), mesh(NULL),
      scale(1), zoom(1), tilt(90), yaw(0),
      perspective(0.25), status(" ")
{
	QFile styleFile(":res/qt/style.qss");
	styleFile.open(QFile::ReadOnly);
	setStyleSheet(styleFile.readAll());

}

GLView::~GLView()
{
    delete mesh;
}

void GLView::view_anim(float v)
{
    
}

void GLView::view_orthographic()
{
    view_anim(0);
}

void GLView::view_perspective()
{
    view_anim(0.25);
}

void GLView::load_mesh(Mesh* m, bool is_reload)
{
    mesh = new GLMesh(m);

    if (!is_reload)
    {
        QVector3D lower(m->xmin(), m->ymin(), m->zmin());
        QVector3D upper(m->xmax(), m->ymax(), m->zmax());
        center = (lower + upper) / 2;
        scale = 2 / (upper - lower).length();

        // Reset other camera parameters
        zoom = 1;
        yaw = 0;
        tilt = 90;
    }

    update();

    delete m;
}

void GLView::set_status(const QString &s)
{
    status = s;
    update();
}

void GLView::set_perspective(float p)
{
    perspective = p;
    update();
}

void GLView::clear_status()
{
    status = "";
    update();
}

void GLView::initializeGL()
{
    initializeGLFunctions();

    mesh_shader.addShaderFromSourceFile(QGLShader::Vertex, ":res/gl/mesh.vert");
    mesh_shader.addShaderFromSourceFile(QGLShader::Fragment, ":res/gl/mesh.frag");
    mesh_shader.link();

    backdrop = new Backdrop();
}

void GLView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
	paintGL();
    /*glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    backdrop->draw();
    if (mesh)  draw_mesh();

    if (status.isNull())    return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawText(10, height() - 10, status);*/
}


void GLView::draw_mesh()
{
    mesh_shader.bind();

    // Load the transform and view matrices into the shader
    glUniformMatrix4fv(
                mesh_shader.uniformLocation("transform_matrix"),
                1, GL_FALSE, transform_matrix().data());
    glUniformMatrix4fv(
                mesh_shader.uniformLocation("view_matrix"),
                1, GL_FALSE, view_matrix().data());

    // Compensate for z-flattening when zooming
    glUniform1f(mesh_shader.uniformLocation("zoom"), 1/zoom);

    // Find and enable the attribute location for vertex position
    const GLuint vp = mesh_shader.attributeLocation("vertex_position");
    glEnableVertexAttribArray(vp);

    // Then draw the mesh with that vertex position
    mesh->draw(vp);

    // Clean up state machine
    glDisableVertexAttribArray(vp);
    mesh_shader.release();
}

QMatrix4x4 GLView::transform_matrix() const
{
    QMatrix4x4 m;
    m.rotate(tilt, QVector3D(1, 0, 0));
    m.rotate(yaw,  QVector3D(0, 0, 1));
    m.scale(-scale, scale, -scale);
    m.translate(-center);
    return m;
}

QMatrix4x4 GLView::view_matrix() const
{
    QMatrix4x4 m;
    if (width() > height())
    {
        m.scale(-height() / float(width()), 1, 0.5);
    }
    else
    {
        m.scale(-1, width() / float(height()), 0.5);
    }
    m.scale(zoom, zoom, 1);
    m(3, 2) = perspective;
    return m;
}

void GLView::paintGL()
{
	glDisable(GL_LIGHTING);

	QColor bgcol(255, 255, 0, 255);
	QColor axescolor(128, 128, 128, 255);
	float r = bgcol.redF();
	float g = bgcol.greenF();
	float b = bgcol.blueF();
	glClearColor(r, g, b, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	setupCamera();
	if (this->cam.type == Camera::GIMBAL) {
		// Only for GIMBAL cam
		// The crosshair should be fixed at the center of the viewport...
		if (showcrosshairs) GLView::showCrosshairs();
		glTranslated(cam.object_trans.x(), cam.object_trans.y(), cam.object_trans.z());
		// ...the axis lines need to follow the object translation.
		if (showaxes) GLView::showAxes(axescolor);
		// mark the scale along the axis lines
		if (showaxes && showscale) GLView::showScalemarkers(axescolor);
	}

	glEnable(GL_LIGHTING);
	glDepthFunc(GL_LESS);
	glCullFace(GL_BACK);
	glDisable(GL_CULL_FACE);
	glLineWidth(2);
	glColor3d(1.0, 0.0, 0.0);


//	if (this->renderer) {
//#if defined(ENABLE_OPENCSG)
//		// FIXME: This belongs in the OpenCSG renderer, but it doesn't know about this ID yet
//		OpenCSG::setContext(this->opencsg_id);
//#endif
//		this->renderer->draw(showfaces, showedges);
//	}

	// Only for GIMBAL
	glDisable(GL_LIGHTING);
	if (showaxes) GLView::showSmallaxes(axescolor);
}

void GLView::setCamera(const Camera &cam)
{
	this->cam = cam;
}

void GLView::setupCamera()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	switch (this->cam.type) {
	case Camera::GIMBAL: {
		double dist = cam.zoomValue();
		switch (this->cam.projection) {
		case Camera::PERSPECTIVE: {
			gluPerspective(cam.fov, aspectratio, 0.1*dist, 100 * dist);
			break;
		}
		case Camera::ORTHOGONAL: {
			double height = dist * tan(cam.fov / 2 * M_PI / 180);
			glOrtho(-height*aspectratio, height*aspectratio,
				-height, height,
				-100 * dist, +100 * dist);
			break;
		}
		}
		gluLookAt(0.0, -dist, 0.0,
			0.0, 0.0, 0.0,
			0.0, 0.0, 1.0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glRotated(cam.object_rot.x(), 1.0, 0.0, 0.0);
		glRotated(cam.object_rot.y(), 0.0, 1.0, 0.0);
		glRotated(cam.object_rot.z(), 0.0, 0.0, 1.0);
		break;
	}
	case Camera::VECTOR: {
		double dist = (cam.center - cam.eye).lengthSquared();
		switch (this->cam.projection) {
		case Camera::PERSPECTIVE: {
			gluPerspective(cam.fov, aspectratio, 0.1*dist, 100 * dist);
			break;
		}
		case Camera::ORTHOGONAL: {
			double height = dist * tan(cam.fov / 2 * M_PI / 180);
			glOrtho(-height*aspectratio, height*aspectratio,
				-height, height,
				-100 * dist, +100 * dist);
			break;
		}
		}
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		QVector3D dir(cam.eye - cam.center);
		QVector3D up(0.0, 0.0, 1.0);
		if (QVector3D::crossProduct(dir,up).lengthSquared() < 0.001) { // View direction is ~parallel with up vector
			up  = QVector3D(0.0, 1.0, 0.0);
		}

		gluLookAt(cam.eye[0], cam.eye[1], cam.eye[2],
			cam.center[0], cam.center[1], cam.center[2],
			up[0], up[1], up[2]);
		break;
	}
	default:
		break;
	}
}


void GLView::showCrosshairs()
{
	// FIXME: this might not work with Vector camera
	glLineWidth(this->getDPI());
	QColor col(255, 0, 0, 255);
	glColor3f(col.red(),col.green(),col.blue());
	glBegin(GL_LINES);
	for (double xf = -1; xf <= +1; xf += 2)
		for (double yf = -1; yf <= +1; yf += 2) {
			double vd = cam.zoomValue() / 8;
			glVertex3d(-xf*vd, -yf*vd, -vd);
			glVertex3d(+xf*vd, +yf*vd, +vd);
		}
	glEnd();
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
    if (event->button() == Qt::LeftButton ||
        event->button() == Qt::RightButton)
    {
        mouse_pos = event->pos();
        setCursor(Qt::ClosedHandCursor);
    }
}

void GLView::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton ||
        event->button() == Qt::RightButton)
    {
        unsetCursor();
    }
}

void GLView::mouseMoveEvent(QMouseEvent* event)
{
    auto p = event->pos();
    auto d = p - mouse_pos;


    if (event->buttons() & Qt::LeftButton)
    {
        yaw = fmod(yaw - d.x(), 360);
        tilt = fmod(tilt - d.y(), 360);
        update();
    }
    else if (event->buttons() & Qt::RightButton)
    {
        center = transform_matrix().inverted() *
                 view_matrix().inverted() *
                 QVector3D(-d.x() / (0.5*width()),
                            d.y() / (0.5*height()), 0);
        update();
    }
    mouse_pos = p;
}

void GLView::wheelEvent(QWheelEvent *event)
{
    // Find GL position before the zoom operation
    // (to zoom about mouse cursor)
    auto p = event->pos();
    QVector3D v(1 - p.x() / (0.5*width()),
                p.y() / (0.5*height()) - 1, 0);
    QVector3D a = transform_matrix().inverted() *
                  view_matrix().inverted() * v;

    if (event->delta() < 0)
    {
        for (int i=0; i > event->delta(); --i)
            zoom *= 1.001;
    }
    else if (event->delta() > 0)
    {
        for (int i=0; i < event->delta(); ++i)
            zoom /= 1.001;
    }

    // Then find the cursor's GL position post-zoom and adjust center.
    QVector3D b = transform_matrix().inverted() *
                  view_matrix().inverted() * v;
    center += b - a;
    update();
}

void GLView::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
}
