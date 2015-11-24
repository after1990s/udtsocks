#include "udtforwardclient.h"
#include "udtsocksserver.h"

int printusage()
{
	return printf("usage： \n"
			"\tudtsocks s(erver) listen_port\n"
			"\tudtsocks c(lient) listen_port server port\n"
	);
}
void test()
{
	char src[] = "f3df";
	src[0] = 0x01;
	char buf[8] = {0};
	sprintf(buf, "0x%.2x", src[0]);
	std::cout << buf;
}

int main(int argc, char* argv[])
{
	//test();
	if (argc==3 && argv[1][0] == 's')
	{
		udtconfig::setlistenport(argv[2]);
		udtforwardclient::udtforwardclient_init();
		for(;;)
				sleep(1000);
	}
	if (argc==5 && argv[1][0] == 'c')
	{
		udtconfig::setlistenport(argv[2]);
		udtconfig::setserveraddr(argv[3], argv[4]);
		udtsocksserver::udtsocksserver_init();
		for(;;)
			sleep(1000);
	}

	printusage();
	return 0;
}


