#include "qfloat.h"

int main(int argc, char* argv[])
{
	/*if (argc == 1)
	{
		consoleProcess();
	}
	else
	{
		std::ifstream inp(argv[1]);
		std::ofstream outp(argv[2]);
		fileProcess(inp, outp);
		inp.close();
		outp.close();
	}*/

	std::ifstream inp("Qfloat_input.txt");
	std::ofstream outp("output.txt");
	fileProcess(inp, outp);
	inp.close();
	outp.close();
	return 0;
}