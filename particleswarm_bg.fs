varying vec2 pos;

void main(void)
{
	float value = f(pos.x, pos.y);
	gl_FragColor = vec4(value, value, value, 1.0);
}

/* vim: set ft=c : */
