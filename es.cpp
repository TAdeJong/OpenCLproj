#define __CL_ENABLE_EXCEPTIONS

// C++ stdlib
#include <iostream>
#include <vector>
#include <limits>
#include <algorithm>

// C stdlib
#include <ctime>

// OpenCL
#include "platform.h"
#include "es_common.h"

#define PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062f

using namespace cl;
using std::clog;
using std::endl;
using std::vector;

const int LAMBDA = 3000;
const int MU = LAMBDA / 8;
const int INDIVIDUALS_SIZE = LAMBDA*sizeof(individual);

bool individual_comp(const individual &a, const individual &b)
{
	return a.fitness < b.fitness;
}

int main(int argc, char **argv)
{
	srand((unsigned)time(NULL));

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
			0
		};
		context = Context(GET_TARGET_PLATFORM, cprops);

		vector<Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();
		queue = CommandQueue(context, devices[0]);

		std::string sourceCode = "#include \"es.cl\"\n";
		Program::Sources source(1, std::make_pair(sourceCode.c_str(), sourceCode.length()+1));
		Program program = Program(context, source);

		try
		{
			program.build(devices, "-I.");
		}
		catch (Error &)
		{
			std::string errors;
			program.getBuildInfo(devices[0], CL_PROGRAM_BUILD_LOG, &errors);
			std::cerr << "Build log: " << endl << errors << endl;
			return 1;
		}

		kernel = Kernel(program, "es");
	}

	individual *individuals = new individual[LAMBDA];
	for (int i = 0; i < LAMBDA; i++)
	{
		for (int j = 0; j < DIM; ++j)
		{
			individuals[i].x[j] = (rand()/((float)RAND_MAX)) * (XMAX-XMIN) + XMIN;
			individuals[i].s[j] = (XMAX-XMIN) / 6.f;
		}
		for (int j = 0; j < DIM_A; ++j)
		{
			individuals[i].a[j] = (rand()/((float)RAND_MAX)) * (2*PI) - PI;
		}
		
		individuals[i].fitness = 0;
	}

	float gbest = std::numeric_limits<float>::infinity(), xbest[DIM];
	
	Buffer esBuffer = Buffer(context, NULL, INDIVIDUALS_SIZE);
	Event ev;
	queue.enqueueMapBuffer(esBuffer, CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, 0, INDIVIDUALS_SIZE);
	
	for (int i = 0; i < 1000; i++)
	{
		queue.enqueueWriteBuffer(esBuffer, CL_TRUE, 0, INDIVIDUALS_SIZE, individuals);
		kernel.setArg(1, (cl_ulong)rand());
		kernel.setArg(0, esBuffer);
		queue.enqueueNDRangeKernel(kernel, NullRange, NDRange(LAMBDA), NDRange(1), NULL, &ev);
		ev.wait();
		queue.enqueueReadBuffer(esBuffer, CL_TRUE, 0, INDIVIDUALS_SIZE, individuals);
		
		std::sort(individuals, individuals + LAMBDA, individual_comp);
		for (int j = MU; j < LAMBDA; ++j)
		{
			individuals[j] = individuals[rand() % MU];
		}
	}
	gbest = individuals[0].fitness;
	for (int i = 0; i < DIM; ++i) xbest[i] = individuals[0].x[i];
	clog << "Best value " << gbest << " found at (";
	for (int i = 0; i < DIM; ++i) clog << xbest[i] << (i == DIM-1 ? ")" : ", ");
	clog << "\n";
	clog << "Our computation estemates it: f(" << xbest[0] << ", ..., " << xbest[DIM-1] << ") = " << es_f(xbest) << endl;

	delete[] individuals;

	return 0;
}
