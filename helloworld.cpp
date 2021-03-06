#include <iostream>
#include <vector>

#include "platform.h"

void checkErr(cl_int err, const char *name)
{
	if (err == CL_SUCCESS)
		return;

	std::cerr << "[ERROR] In " << name << ", with code: " << err << std::endl;
	exit(1);
}

int main(int argc, char **argv)
{
	cl_int err;
	std::vector<cl::Platform> platformList;
	cl::Platform::get(&platformList);

	checkErr(platformList.size() != 0 ? CL_SUCCESS : -1, "cl::Platform::get");
	std::clog << "Platform number is: " << platformList.size() << std::endl;

	std::string platformVendor;
	platformList[0].getInfo((cl_platform_info)CL_PLATFORM_VENDOR, &platformVendor);
	std::clog << "Platform is by: " << platformVendor << "\n";

	std::string platformVersion;
	platformList[0].getInfo((cl_platform_info)CL_PLATFORM_VERSION, &platformVersion);
	std::clog << "Supported version is: " << platformVersion << "\n";

	cl_context_properties cprops[] = {CL_CONTEXT_PLATFORM, (cl_context_properties)(platformList[0])(), 0};
	cl::Context context(GET_TARGET_PLATFORM, cprops, NULL, NULL, &err);

	checkErr(err, "Context::Context()");

	return 0;
}
