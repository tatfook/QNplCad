#ifndef GLView_H
#define GLView_H

#include <QWidget>
#include <QPropertyAnimation>
#include <QtOpenGL/QGLWidget>
#include <QtOpenGL/QGLFunctions>
#include <QtOpenGL/QGLShaderProgram>
#include <QMatrix4x4>
#include <QColor>
#include "Camera.h"
class GLMesh;
class Mesh;
class Backdrop;

class GLView : public QGLWidget, protected QGLFunctions
{
    Q_OBJECT

public:
    GLView(const QGLFormat& format, QWidget* parent=0);

    void initializeGL();
    void paintEvent(QPaintEvent* event);
    ~GLView();

    void view_orthographic();
    void view_perspective();

public slots:
    void set_status(const QString& s);
    void clear_status();
    void load_mesh(Mesh* m, bool is_reload);


protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);
    void resizeGL(int width, int height);
    void set_perspective(float p);
    void view_anim(float v);


private:
    void draw_mesh();

    QMatrix4x4 transform_matrix() const;
    QMatrix4x4 view_matrix() const;

    QGLShaderProgram mesh_shader;
    QGLShaderProgram quad_shader;

    GLMesh* mesh;
    Backdrop* backdrop;

    QVector3D center;
    float scale;
    float zoom;
    float tilt;
    float yaw;

    float perspective;
    Q_PROPERTY(float perspective WRITE set_perspective);
    QPropertyAnimation anim;

    QPoint mouse_pos;
    QString status;
public:
	virtual void paintGL();
	virtual float getDPI() { return 1.0f; }
	void setCamera(const Camera &cam);
	void setupCamera();
private:
	Camera cam;
	double far_far_away;
	size_t m_width;
	size_t m_height;
	double aspectratio;
	bool orthomode;
	bool showaxes;
	bool showfaces;
	bool showedges;
	bool showcrosshairs;
	bool showscale;
private:
	void showCrosshairs();
	void showAxes(const QColor &col);
	void showSmallaxes(const QColor &col);
	void showScalemarkers(const QColor &col);
	void decodeMarkerValue(double i, double l, int size_div_sm);
};

#endif // GLView_H
