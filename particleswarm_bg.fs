varying vec2 pos;

void main(void)
{
	float value = f(pos.x, pos.y)/100.f+1.f;
	gl_FragColor = vec4(value, value, value, 1.0);
}

/* vim: set ft=c : */
