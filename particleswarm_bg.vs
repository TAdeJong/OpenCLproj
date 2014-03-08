varying vec2 pos;

void main(void)
{
	pos = gl_Vertex.xy*2.f - vec2(1.f, 1.f);
	gl_Position = vec4(pos, 0, gl_Vertex.w);
}

/* vim: set ft=c : */
