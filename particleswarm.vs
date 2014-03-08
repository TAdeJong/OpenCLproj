varying vec2 pos;

void main(void)
{
	pos = gl_Vertex.xy/10.f;
	gl_Position = vec4(pos, 0, gl_Vertex.w);
}

/* vim: set ft=c : */
