#ifndef GLView_H
#define GLView_H

#include <QWidget>
#include <QtOpenGL/QGLWidget>
#include <QtOpenGL/QGLFunctions>
#include <QtOpenGL/QGLShaderProgram>
#include <QMatrix4x4>
#include <QColor>
#include "Camera.h"

class GLView : public QGLWidget, protected QGLFunctions
{
    Q_OBJECT

public:
    GLView(const QGLFormat& format, QWidget* parent=0);

    void initializeGL();
    ~GLView();

protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);
    void resizeGL(int width, int height);
	void paintGL();
private:
    QGLShaderProgram mesh_shader;
public:
	float getDPI() { return 1.0f; }
private:
	Camera* m_camera;
private:
	void showCrosshairs();
	void showAxes(const QColor &col);
	void showSmallaxes(const QColor &col);
	void showScalemarkers(const QColor &col);
	void decodeMarkerValue(double i, double l, int size_div_sm);
	void normalizeAngle(float& angle);
};

#endif
