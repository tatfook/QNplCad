#version 120
attribute vec3 vertex_position;

uniform mat4 mvp;

varying vec3 ec_pos;

void main() {
    gl_Position = mvp*vec4(vertex_position, 1.0);
    ec_pos = gl_Position.xyz;
}
