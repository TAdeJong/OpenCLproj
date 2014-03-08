#define __NO_STD_VECTOR
#define __CL_ENABLE_EXCEPTIONS

#include <iostream>
#include <iomanip>
#include <fstream>
#include <climits>
#include <cmath>
#include <ctime>
#include <sstream>
#include "platform.h"

using namespace cl;
using std::clog;
using std::endl;
using std::stringstream;

struct particle
{
	float x, y;
	float vx, vy;

	float pbest, px, py;
};

const int NUM_PARTICLES = 100000;
const int PARTICLES_SIZE = NUM_PARTICLES*sizeof(particle);

int main(int argc, char **argv)
{
	srand(time(NULL));

	cl_int err;
	vector<Platform> platformList;
	Platform::get(&platformList);

	clog << "Platform number is: " << platformList.size() << endl;

	std::string platformVendor;
	platformList[0].getInfo((cl_platform_info)CL_PLATFORM_VENDOR, &platformVendor);
	clog << "Platform is by: " << platformVendor << "\n";

	cl_context_properties cprops[] = {CL_CONTEXT_PLATFORM, (cl_context_properties)(platformList[0])(), 0};
	Context context(GET_TARGET_PLATFORM, cprops, NULL, NULL, &err);

	vector<Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();
	CommandQueue queue = CommandQueue(context, devices[0]);

	particle *particles = new particle[NUM_PARTICLES];
	for (int i = 0; i < NUM_PARTICLES; i++)
	{
		particles[i].x = particles[i].px = rand()/((float)RAND_MAX)*2-1;
		particles[i].y = particles[i].py = rand()/((float)RAND_MAX)*2-1;
		particles[i].vx = rand()/((float)RAND_MAX)*2-1;
		particles[i].vy = rand()/((float)RAND_MAX)*2-1;
		particles[i].pbest = INT_MIN;
	}

	std::ifstream sourceFile("particleswarm.cl");
	std::string sourceCode(
			std::istreambuf_iterator<char>(sourceFile),
			(std::istreambuf_iterator<char>()));
	Program::Sources source(1, std::make_pair(sourceCode.c_str(), sourceCode.length()+1));
	Program program = Program(context, source);

	try
	{
		stringstream s;
		s << std::showpoint;
		s << "-DX_MIN=" << -10.f << "f ";
		s << "-DY_MIN=" << -10.f << "f ";
		s << "-DX_MAX=" << 10.f << "f ";
		s << "-DY_MAX=" << 10.f << "f ";
		program.build(devices, s.str().c_str());
	}
	catch (Error &e)
	{
		std::string errors;
		program.getBuildInfo(devices[0], CL_PROGRAM_BUILD_LOG, &errors);
		std::cerr << "Build log: " << endl << errors << endl;
		return 1;
	}

	Kernel kernel(program, "particleswarm");

	Buffer particleBuffer = Buffer(context, CL_MEM_READ_WRITE, PARTICLES_SIZE);
	NDRange global(NUM_PARTICLES);
	NDRange local(1);
	Event event;

	float gbest = -42, gx = 0, gy = 0;
	float phi1 = 1.5, phi2 = 1.5;
	for (int i = 0; i < 1000; i++)
	{
		queue.enqueueWriteBuffer(particleBuffer, CL_TRUE, 0, PARTICLES_SIZE, particles, NULL, &event);
		kernel.setArg(0, particleBuffer);
		kernel.setArg(1, gbest);
		kernel.setArg(2, gx);
		kernel.setArg(3, gy);
		kernel.setArg(4, phi1);
		kernel.setArg(5, phi2);

		queue.enqueueNDRangeKernel(kernel, NullRange, global, local, NULL, &event);
		event.wait();

		queue.enqueueReadBuffer(particleBuffer, CL_TRUE, 0, PARTICLES_SIZE, particles, NULL, &event);
		event.wait();
		for (int j = 0; j < NUM_PARTICLES; j++)
			if (particles[j].pbest > gbest)
			{
				gbest = particles[j].pbest;
				gx = particles[j].px;
				gy = particles[j].py;
			}
	}

	clog << "Best value " << gbest << " found at (" << gx << ", " << gy << ")\n";

	delete[] particles;
	return 0;
}
