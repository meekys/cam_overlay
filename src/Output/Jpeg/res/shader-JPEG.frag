#ifdef GL_ES
#  ifdef GL_FRAGMENT_PRECISION_HIGH
#    define maxfragp highp
#  else
#    define maxfragp mediump
#  endif
#else
#  define maxfragp
#endif

uniform sampler2D textureY;
uniform sampler2D textureU;
uniform sampler2D textureV;

varying maxfragp vec2 fragmentUV;

maxfragp mat3 yuvColor = mat3(
  1.0,    1.0,     1.0,
  0.0,   -0.34414, 1.772,
  1.402, -0.71414, 0.0
);

void main(void)
{
    maxfragp float y = texture2D(textureY, fragmentUV).r;
    maxfragp float u = texture2D(textureU, fragmentUV).r;
    maxfragp float v = texture2D(textureV, fragmentUV).r;

    maxfragp vec3 yuv = vec3(y, u, v);

    maxfragp vec3 rgb = yuvColor * vec3(y, u - 0.5, v - 0.5);
    gl_FragColor = vec4(rgb, 1.0);
}