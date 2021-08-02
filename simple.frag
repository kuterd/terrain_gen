#version 400 


varying float Z;
varying vec3 Normal;
varying vec3 Pos;

void main() {
    vec3 lightDir = normalize(vec3(1, 1, 0));
    float light = max(0.0, dot(lightDir, normalize(Normal)));
    
    int xS = int(sign(Pos.x)) == -1 ? 0 : 1;
    int yS = int(sign(Pos.y)) == -1 ? 0 : 1;
    
    bool special = bool(int((int(Pos.x / 10.0) % 2 == xS)) ^ int((int(Pos.y / 10.0) % 2 == yS)));

    gl_FragColor = vec4(float(special), float(!special), 1.5, 1.0) * light;
}
