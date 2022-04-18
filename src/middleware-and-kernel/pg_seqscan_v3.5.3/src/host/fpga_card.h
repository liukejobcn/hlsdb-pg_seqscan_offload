#ifndef FPGA_CARD_H
#define FPGA_CARD_H
#include "fpga_common.h"

class fpga_card
{
public:
	fpga_card();
	~fpga_card();
	int load_xclbin(const char *xclbin);
	int load_xclbin(const char *xclbin, const char *krnl_name, int krnl_cnt);
	int get_kernels(const char *name, int cnt);
public:
	std::vector<cl::Platform> platforms;
	cl::Platform platform;
	std::vector<cl::Device> devices;
	cl::Device device;
	bool found_device;

	cl::Context	*context;
	cl::CommandQueue	*q;
	char *xclbin_buf;
	cl::Program::Binaries	bins;
	cl::Program	*program;
	cl::Kernel	**kernels;
	std::mutex	kernel_mutex;
	int	cu_cnt;
	char	cu_name[128];

};

#endif // FPGA_CARD_H
