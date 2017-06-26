#ifndef MESH_H
#define MESH_H

#include <QString>
#include <QtOpenGL/QtOpenGL>

#include <vector>

class Mesh
{
public:
    Mesh(std::vector<GLfloat> vertices, std::vector<GLuint> indices);

    float min_(size_t start) const;
    float max_(size_t start) const;

    float xmin() const { return min_(0); }
    float ymin() const { return min_(1); }
    float zmin() const { return min_(2); }
    float xmax() const { return max_(0); }
    float ymax() const { return max_(1); }
    float zmax() const { return max_(2); }

    bool empty() const;

private:
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;

    friend class GLMesh;
};

#endif // MESH_H
