#define __NO_STD_VECTOR
#define __CL_ENABLE_EXCEPTIONS

#include <iostream>
#include <fstream>
#include <climits>
#include "platform.h"

using namespace cl;
using std::clog;
using std::endl;

struct particle
{
	float x, y;
	float vx, vy;

	float pbest, px, py;
};

const int NUM_PARTICLES = 10;
const int PARTICLES_SIZE = NUM_PARTICLES*sizeof(particle);

int main(int argc, char **argv)
{
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

	std::ifstream sourceFile("particleswarm.cl");
	std::string sourceCode(
			std::istreambuf_iterator<char>(sourceFile),
			(std::istreambuf_iterator<char>()));
	Program::Sources source(1, std::make_pair(sourceCode.c_str(), sourceCode.length()+1));
	Program program = Program(context, source);

	try
	{
		program.build(devices);
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

	int gbest = INT_MIN, gx = 0, gy = 0;
	for (int i = 0; i < 1; i++)
	{
		queue.enqueueWriteBuffer(particleBuffer, CL_TRUE, 0, PARTICLES_SIZE, particles);
		kernel.setArg(0, &particleBuffer);
		kernel.setArg(1, &gbest);
		kernel.setArg(2, &gx);
		kernel.setArg(3, &gy);

		queue.enqueueNDRangeKernel(kernel, NullRange, global, local);

		queue.enqueueReadBuffer(particleBuffer, CL_TRUE, 0, PARTICLES_SIZE, particles, NULL, NULL);
		for (int j = 0; j < NUM_PARTICLES; j++)
			if (particles[j].pbest > gbest)
			{
				gbest = particles[i].pbest;
				gx = particles[i].px;
				gy = particles[i].py;
			}
	}
	return 0;
}
