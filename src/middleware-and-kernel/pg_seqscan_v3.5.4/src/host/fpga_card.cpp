#include "fpga_card.h"

fpga_card::fpga_card()
{

}

fpga_card::~fpga_card()
{
	delete xclbin_buf;
	for(int i=0; i<cu_cnt; i++){
		delete kernels[i];
	}
	delete kernels;
	delete program;
	delete q;
	delete context;
	return ;
}

int fpga_card::load_xclbin(const char *xclbin)
{
	found_device = false;
	cl::Platform::get(&platforms);
        for(size_t i = 0; (i < platforms.size() ) & (found_device == false) ;i++){
		platform = platforms[i];
		std::string platformName = platform.getInfo<CL_PLATFORM_NAME>();
		if ( platformName == "Xilinx"){
			devices.clear();
			platform.getDevices(CL_DEVICE_TYPE_ACCELERATOR, &devices);
			if (devices.size()){
				device = devices[0];
				found_device = true;
				break;
			}
		}
        }
        if (found_device == false){
                std::cout << "Error: Unable to find Target Device " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
                return EXIT_FAILURE;
        }

	context = new cl::Context(device);
	q = new cl::CommandQueue(*context, device, CL_QUEUE_PROFILING_ENABLE|CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE);

	// Load xclbin
        std::cout << "Loading: '" << xclbin<< "'\n";
        std::ifstream bin_file(xclbin, std::ifstream::binary);
        bin_file.seekg (0, bin_file.end);
        unsigned nb = bin_file.tellg();
        bin_file.seekg (0, bin_file.beg);
        xclbin_buf = new char [nb];
        bin_file.read(xclbin_buf, nb);

        // Creating Program from Binary File
        bins.push_back({xclbin_buf,nb});
        devices.resize(1);
        program = new cl::Program(*context, devices, bins);

	DBG_INFO("init ok\n");

	return 0;
}

int fpga_card::get_kernels(const char *name, int cnt)
{
	cl_int err;
	char name_buf[128];
	strncpy(cu_name, name, strlen(name)+1);	

	kernels = new cl::Kernel*[cnt];
	cu_cnt = cnt;;	
	for(int i=0; i<cnt; i++){
		sprintf(name_buf, "%s:{%s_%d}", name, name,  i+1);
		kernels[i] = new cl::Kernel(*program, name_buf, &err);
	}

	return 0;
}

int fpga_card::load_xclbin(const char *xclbin, const char *krnl_name, int krnl_cnt)
{
	int ret;
	ret = load_xclbin(xclbin);
	if(ret){
		return ret;
	}

	ret = get_kernels(krnl_name, krnl_cnt);
	if(ret){
		return ret;
	}
	return 0;
}


