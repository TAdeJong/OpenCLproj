#define __NO_STD_VECTOR
#define __CL_ENABLE_EXCEPTIONS

// C++ stdlib
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <stdexcept>

// C stdlib
#include <climits>
#include <cmath>
#include <ctime>

// OpenGL / SDL / GLEW
#include <GL/glew.h>
#include <GL/glx.h>
#include <SDL/SDL.h>

// OpenCL
#include "platform.h"

// Particleswarm internal
#include "particleswarm_common.h"

using namespace cl;
using std::clog;
using std::endl;
using std::stringstream;

const int NUM_PARTICLES = 10000;
const int PARTICLES_SIZE = NUM_PARTICLES*sizeof(particle);

GLuint bgShaderProgram;
GLuint particleShaderProgram;
GLuint particleBuffer_gl;
particle *particles;

void drawFrame()
{
	glUseProgram(bgShaderProgram);
	glClear(GL_COLOR_BUFFER_BIT);

	glBegin(GL_QUADS);
	glVertex2f(0, 0);
	glVertex2f(1, 0);
	glVertex2f(1, 1);
	glVertex2f(0, 1);
	glEnd();

	glVertexPointer(2, GL_FLOAT, sizeof(particle), &particles[0].x);
	glBindBuffer(GL_ARRAY_BUFFER, particleBuffer_gl);

	glUseProgram(particleShaderProgram);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDrawArrays(GL_POINTS, 0, NUM_PARTICLES);
	glDisableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	SDL_GL_SwapBuffers();
	glFlush();
	SDL_Delay(30);
}

std::string readFile(const char *filename)
{
	std::ifstream f(filename);
	std::string str((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
	f.close();
	return str;
}

void compileShaders(GLuint &shaderProgram, const char *vertex, const char *fragment)
{
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

	std::string common = readFile("particleswarm_common.h");
	std::string fragmentSource = readFile(fragment);
	std::string vertexSource = readFile(vertex);

	const char *fsSource[] = {"#version 120\n", common.c_str(), fragmentSource.c_str()};
	const char *vsSource[] = {"#version 120\n", common.c_str(), vertexSource.c_str()};

	glShaderSource(fragmentShader, sizeof(fsSource)/sizeof(const char*), fsSource, 0);
	glCompileShader(fragmentShader);
	GLint success = 0;
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		std::cerr << "Fragment shader compilation failed!" << endl;
		GLint bufferSize;
		glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &bufferSize);
		char *buffer = new char[bufferSize];
		glGetShaderInfoLog(fragmentShader, bufferSize, NULL, buffer);
		std::string errstr(buffer);
		delete[] buffer;

		throw std::runtime_error(errstr);
	}

	glShaderSource(vertexShader, sizeof(vsSource)/sizeof(const char*), vsSource, 0);
	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		std::cerr << "Vertex shader compilation failed!" << endl;
		GLint bufferSize;
		glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &bufferSize);
		char *buffer = new char[bufferSize];
		glGetShaderInfoLog(vertexShader, bufferSize, NULL, buffer);
		std::string errstr(buffer);
		delete[] buffer;

		throw std::runtime_error(errstr);
	}

	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, fragmentShader);
	glAttachShader(shaderProgram, vertexShader);
	glLinkProgram(shaderProgram);
}

int main(int argc, char **argv)
{
	srand(time(NULL));

	SDL_Init(SDL_INIT_VIDEO);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_SetVideoMode(800, 600, 32, SDL_OPENGL | SDL_DOUBLEBUF);
	glewInit();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 800, 600, 0, 0, 1);
	glMatrixMode(GL_MODELVIEW);
	glDisable(GL_DEPTH_TEST);
	glClearColor(1.f, 0.f, 0.f, 1.f);

	compileShaders(bgShaderProgram, "particleswarm_bg.vs", "particleswarm_bg.fs");
	compileShaders(particleShaderProgram, "particleswarm.vs", "particleswarm.fs");

	Kernel kernel;
	CommandQueue queue;
	Context context;

	{
		vector<Platform> platformList;
		Platform::get(&platformList);

		clog << "Platform number is: " << platformList.size() << endl;

		std::string platformVendor;
		platformList[0].getInfo((cl_platform_info)CL_PLATFORM_VENDOR, &platformVendor);
		clog << "Platform is by: " << platformVendor << "\n";

		cl_context_properties cprops[] = {
			CL_CONTEXT_PLATFORM, (cl_context_properties) platformList[0](),
			CL_GL_CONTEXT_KHR,   (cl_context_properties) glXGetCurrentContext(),
			CL_GLX_DISPLAY_KHR,  (cl_context_properties) glXGetCurrentDisplay(),
			0
		};
		context = Context(GET_TARGET_PLATFORM, cprops);

		vector<Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();
		queue = CommandQueue(context, devices[0]);

		std::string sourceCode = "#include \"particleswarm.cl\"\n";
		Program::Sources source(1, std::make_pair(sourceCode.c_str(), sourceCode.length()+1));
		Program program = Program(context, source);

		try
		{
			stringstream s;
			s << std::showpoint;
			s << "-DX_MIN=" << -1.f << "f ";
			s << "-DY_MIN=" << -1.f << "f ";
			s << "-DX_MAX=" << 1.f << "f ";
			s << "-DY_MAX=" << 1.f << "f ";
			program.build(devices, s.str().c_str());
		}
		catch (Error &e)
		{
			std::string errors;
			program.getBuildInfo(devices[0], CL_PROGRAM_BUILD_LOG, &errors);
			std::cerr << "Build log: " << endl << errors << endl;
			return 1;
		}

		kernel = Kernel(program, "particleswarm");
	}


	particles = new particle[NUM_PARTICLES];
	for (int i = 0; i < NUM_PARTICLES; i++)
	{
		particles[i].x = particles[i].px = rand()/((float)RAND_MAX)*2-1;
		particles[i].y = particles[i].py = rand()/((float)RAND_MAX)*2-1;
		particles[i].vx = rand()/((float)RAND_MAX)*2-1;
		particles[i].vy = rand()/((float)RAND_MAX)*2-1;
		particles[i].pbest = INT_MIN;

		particles[i].state.w = rand();
		particles[i].state.x = rand();
		particles[i].state.y = rand();
		particles[i].state.z = rand();
	}

	BufferGL particleBuffer;
	{
		glGenBuffers(1, &particleBuffer_gl);
		glBindBuffer(GL_ARRAY_BUFFER, particleBuffer_gl);
		glBufferData(GL_ARRAY_BUFFER, PARTICLES_SIZE, particles, GL_STATIC_DRAW);
		glVertexPointer(2, GL_FLOAT, sizeof(particle), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		particleBuffer = BufferGL(context, CL_MEM_READ_WRITE, particleBuffer_gl);
	}

	KernelFunctor kernelF(kernel, queue, NullRange, NDRange(NUM_PARTICLES), NDRange(1));

	drawFrame();

	vector<Memory> glObjects(1, particleBuffer);

	float gbest = -42, gx = 0, gy = 0;
	float phi1 = 0.15, phi2 = 0.25;
	for (int i = 0; i < 100; i++) /* FIXME */
	{
		queue.enqueueAcquireGLObjects(&glObjects);
		queue.enqueueWriteBuffer(particleBuffer, CL_TRUE, 0, PARTICLES_SIZE, particles, NULL);
		kernelF(particleBuffer, gbest, gx, gy, phi1, phi2);
		queue.enqueueReadBuffer(particleBuffer, CL_TRUE, 0, PARTICLES_SIZE, particles, NULL);
		for (int j = 0; j < NUM_PARTICLES; j++)
		{
			if (particles[j].pbest > gbest && std::isfinite(particles[j].pbest))
			{
				gbest = particles[j].pbest;
				gx = particles[j].px;
				gy = particles[j].py;
			}
			//clog << "[" << i << ", " << j << "] (" << particles[j].x << ", " << particles[j].y << ")\n";
		}

		clog << "[" << i << "] Best value " << gbest << " found at (" << gx << ", " << gy << ")\n";
		queue.enqueueReleaseGLObjects(&glObjects);
		queue.flush();
		drawFrame();
	}

	clog << "Best value " << gbest << " found at (" << gx << ", " << gy << ")\n";

	delete[] particles;

	SDL_Quit();
	return 0;
}
