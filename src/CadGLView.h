#pragma once
#include <QtOpenGL/QGLWidget>
class CadGLView : public QGLWidget {
	Q_OBJECT

public:
	CadGLView(QWidget * parent = Q_NULLPTR);
	~CadGLView();
protected: 
	virtual void paintEvent(QPaintEvent *pEvent);
	virtual void resizeEvent(QResizeEvent *rEvent);
	virtual void update();
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseMoveEvent(QMouseEvent *event);
private:
};
