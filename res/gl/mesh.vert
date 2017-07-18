#version 120
attribute vec3 vertex_position;
attribute vec3 normal;
attribute vec3 color;
varying vec4 frag_color;
uniform mat4 mvp;
void main() {
    gl_Position = mvp*vec4(vertex_position, 1.0);
	frag_color  = vec4(color[0],color[1],color[2],1.0);
}
