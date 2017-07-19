#version 120
attribute vec3 vertex_position;
attribute vec3 normal;
attribute vec3 color;
varying vec4 frag_color;
uniform mat4 mvp;
void main() {
    gl_Position = mvp*vec4(vertex_position, 1.0);

	vec4 ambient = vec4(0.26,0.26,0.26,1.0); 
	vec3 light = vec3(1.0, 2.0, 3.0) / 3.741657386773941;
	float diffuse = max(0.0, dot(light, normal));
    float specular = pow(max(0.0, -reflect(light, normal).z), 10.0) * sqrt(diffuse);
    frag_color = ambient + vec4(mix(color * (0.3 + 0.7 * diffuse), vec3(1.0), specular), 1.0);

}
