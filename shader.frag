uniform sampler2D texture;

varying vec2 fragmentUV;

void main(void)
{
    gl_FragColor = texture2D(texture, fragmentUV);
    //gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}