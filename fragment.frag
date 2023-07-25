#version 330 core

in vec4 f_clr;
in vec2 f_txc;
uniform sampler2D tex;

void main()
{
    gl_FragColor = texture(tex, f_txc);
}