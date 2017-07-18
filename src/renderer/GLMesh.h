#ifndef GLMESH_H
#define GLMESH_H

#include <QtOpenGL/QGLBuffer>
#include <QtOpenGL/QGLFunctions>

#include "BoundingBox.h";
namespace QNplCad
{
	class Mesh;

	class GLMesh : protected QGLFunctions
	{
	public:
		GLMesh(const Mesh* const mesh);
		void draw(GLuint vp, GLuint vn, GLuint vc);

		BoundingBox box;
	private:
		QGLBuffer vertices;
		QGLBuffer indices;
	};
}

#endif // GLMESH_H
