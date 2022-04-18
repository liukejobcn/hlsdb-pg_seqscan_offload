#include "fpga_server.h"
#include "query.h"
#include  <getopt.h>

#define FPGA_SERVER_PATH	"/tmp/fpga_seqscan_server"
#define VERSION		"v3.5.3"

static void usage(char *name)
{
	std::cout << "Usage: " << name <<" -k <xclbin> [-w <worker count>]" << std::endl;
}

static struct option const long_opts[] = {
        {"kernel", required_argument, NULL, 'k'},
        {"worker_cnt", required_argument, NULL, 'w'},
        {"help", no_argument, NULL, 'h'},
        {0, 0, 0, 0}
};

static uint64_t getopt_integer(char *optarg)
{
        int rc;
        uint64_t value;

        rc = sscanf(optarg, "0x%lx", &value);
        if (rc <= 0){
                rc = sscanf(optarg, "%lu", &value);
        }
        return value;
}

int main(int argc, char* argv[]) 
{
	char* xclbinFilename = NULL;
	int worker_cnt = 4;

	int cmd_opt;
        while ((cmd_opt = getopt_long(argc, argv, "hk:w:", long_opts, NULL)) != -1) {
                switch (cmd_opt) {
                        case 'k':
                                xclbinFilename  = strdup(optarg);
                                break;
                        case 'w':
                                worker_cnt = getopt_integer(optarg);
                                break;
                        case 'h':
                        default:
                                usage(argv[0]);
                                exit(0);
                                break;
                }
        }
	if(!xclbinFilename){
		usage(argv[0]);
		return 0;
	}

	std::cout << "version:" << VERSION << " xclbin=" << xclbinFilename <<" worker_cnt=" << worker_cnt << std::endl;

	fpga_server *fpga = new fpga_server();		

	fpga->open_server(FPGA_SERVER_PATH);

	fpga->start_server(xclbinFilename, "pg_filter", worker_cnt);

	fpga->close_server();

	delete fpga;

	return 0;
}
