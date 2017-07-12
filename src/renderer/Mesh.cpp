#include <QFile>
#include <QDataStream>
#include <QVector3D>

#include <cmath>

#include "Mesh.h"

////////////////////////////////////////////////////////////////////////////////
using namespace QNplCad;
Mesh::Mesh(std::vector<GLfloat> v, std::vector<GLuint> i)
    : vertices(v), indices(i)
{
    // Nothing to do here

	box.m_min = QVector3D(xmin(), ymin(), zmin());
	box.m_max = QVector3D(xmax(), ymax(), zmax());
}

float Mesh::min_(size_t start) const
{
    if (start >= vertices.size())
    {
        return -1;
    }
    float v = vertices[start];
    for (size_t i=start; i < vertices.size(); i += 3)
    {
        v = fmin(v, vertices[i]);
    }
    return v;
}

float Mesh::max_(size_t start) const
{
    if (start >= vertices.size())
    {
        return 1;
    }
    float v = vertices[start];
    for (size_t i=start; i < vertices.size(); i += 3)
    {
        v = fmax(v, vertices[i]);
    }
    return v;
}

bool Mesh::empty() const
{
    return vertices.size() == 0;
}
