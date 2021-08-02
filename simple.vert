#version 400 

attribute vec3 pos;
attribute vec3 normal;

uniform mat4 perspective;
uniform mat4 world;
uniform mat4 camera;

varying float Z;
varying vec3 Normal;
varying vec3 Pos;

void main() {
    Z = pos.y;
    Pos = vec3(pos.xz, 0.0);
    Normal = mat3(transpose(inverse(world))) * normal;
    gl_Position = perspective * inverse(camera) * world * vec4(pos.xyz, 1.0);
} 
