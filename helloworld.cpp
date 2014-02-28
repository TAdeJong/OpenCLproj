#include <CL/cl.h>
#include <iostream>

int main(int argc, char **argv)
{
	std::cout << "Hello, world!" << std::endl;
	cl_int err;
	cl::vector< cl::Platform > platformList;
	cl::Platform::get(&platformList);
	checkErr(platformList.size()!=0 ? CL_SUCCESS : -1, "cl::Platform::get");
	std::cerr << "Platform number is: " << platformList.size() << std::endl;std::string platformVendor;
	platformList[0].getInfo((cl_platform_info)CL_PLATFORM_VENDOR, &platformVendor);
	std::cerr << "Platform is by: " << platformVendor << "\n";
	cl_context_properties cprops[3] =
	{CL_CONTEXT_PLATFORM, (cl_context_properties)(platformList[0])(), 0};cl::Context context(
	CL_DEVICE_TYPE_CPU,
	cprops,
	NULL,
	NULL,
	&err);

	checkErr(err, "Conext::Context()"); 
	
	return 0;
}
