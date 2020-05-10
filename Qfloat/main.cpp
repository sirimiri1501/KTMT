#include "qfloat.h"

int main(int argc, char* argv[])
{
	if (argc == 1)
	{
		process(std::cin, std::cout);
	}
	else
	{
		std::ifstream inp(argv[1]);
		std::ofstream outp(argv[2]);
		process(inp, outp);
		inp.close();
		outp.close();
	}
	return 0;
}