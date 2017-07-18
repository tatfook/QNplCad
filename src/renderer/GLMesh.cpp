#include "GLMesh.h"
#include "Mesh.h"
using namespace QNplCad;

GLMesh::GLMesh(const Mesh* const mesh)
    : vertices(QGLBuffer::VertexBuffer), indices(QGLBuffer::IndexBuffer)
{
	box = mesh->box;
    initializeGLFunctions();

    vertices.create();
    indices.create();

    vertices.setUsagePattern(QGLBuffer::StaticDraw);
    indices.setUsagePattern(QGLBuffer::StaticDraw);

    vertices.bind();
    vertices.allocate(mesh->vertices.data(),
                      mesh->vertices.size() * sizeof(float));
    vertices.release();

    indices.bind();
    indices.allocate(mesh->indices.data(),
                     mesh->indices.size() * sizeof(uint32_t));
    indices.release();
}

void GLMesh::draw(GLuint vp, GLuint vn, GLuint vc)
{
    vertices.bind();
    indices.bind();

	glVertexAttribPointer(vp, 3, GL_FLOAT, false, 9 * sizeof(float), 0);
	glVertexAttribPointer(vn, 3, GL_FLOAT, false, 9 * sizeof(float), (GLvoid*)(3 * sizeof(GLfloat)));
	glVertexAttribPointer(vc, 3, GL_FLOAT, false, 9 * sizeof(float), (GLvoid*)(6 * sizeof(GLfloat)));
    glDrawElements(GL_TRIANGLES, indices.size() / sizeof(uint32_t),
                   GL_UNSIGNED_INT, NULL);

    vertices.release();
    indices.release();
}
