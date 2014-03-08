varying vec2 pos;

void main(void)
{
	vec2 normalpos = pos * 10;
	float value = f(normalpos.x, normalpos.y)/100.f+1.f;
	gl_FragColor = vec4(value, value, value, 1.0);
}

/* vim: set ft=c : */
