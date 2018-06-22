#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <dlfcn.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>


#include <ctype.h>
#include <unistd.h>

#include <time.h>

int readparam (unsigned char flag, unsigned char address, unsigned char slot, unsigned char index)
{
	int iSockFd = -1;
	
	iSockFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (iSockFd == -1)
	{
		printf("socket fail\n");
		exit(1);
	}
	
	// udp sock init
	struct sockaddr_in myAddr;
	memset(&myAddr, 0, sizeof(myAddr));
	
	myAddr.sin_family = AF_INET;
	myAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myAddr.sin_port = htons(12344);
	
	int bindRet = bind(iSockFd, (struct sockaddr*)&myAddr, sizeof(myAddr));

	if (bindRet == -1)
	{
		printf("bind fail\n");
		exit(1);
	}
	
	unsigned char sbuf[4];
	sbuf[0] = flag;
	sbuf[1] = address;
	sbuf[2] = slot;
	sbuf[3] = index;
	
	struct sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("141.76.82.170");
	serverAddr.sin_port = htons(12345);
	
	int serverLen = sizeof(serverAddr);
	
	printf("sending...\n");
	
	int sendRes = sendto(iSockFd, sbuf, 4, 0, (struct sockaddr*)&serverAddr, serverLen);
	
	if (sendRes != 4)
	{
		printf("send fail %d\n", sendRes);
		exit(1);
	}
	printf("ok.\n");

	
	struct sockaddr_in cliAddr;
	memset(&cliAddr, 0, sizeof(cliAddr));
	int cliLen;
	
	unsigned char buff[1024];

	printf("receiving...\n");
	int iRcvdBytes = recvfrom(iSockFd, buff, 1024, 0, (struct sockaddr*)&cliAddr, &cliLen);
	
	if (iRcvdBytes > 0)
	{
		if (buff[0] == sbuf[0])
		{
			int i;
			for (i = 1; i < iRcvdBytes; i++)
				printf(" % 3d,", buff[i]);
			printf("== \n");
			for (i = 1; i < iRcvdBytes; i++)
				printf("0x%02x,", buff[i]);
			printf("\n");
		}
		else
			printf("receive fail: frame marker differs - sent: %d != rcvd: %d\n", sbuf[0], buff[0]);
	}
	else
		printf("receive fail %d\n", iRcvdBytes);
	
	close(iSockFd);
}

int main (int argc, char **argv)
{
	int aset = 0;
	int sset = 0;
	int iset = 0;
	int ad;
	int sl;
	int in;
	unsigned char add = 0;
	unsigned char slo = 0;
	unsigned char ind = 0;
	int index = 0;
	int c;

	opterr = 0;
	
	while ((c = getopt (argc, argv, "a:s:i:")) != -1)
		switch (c)
		{
		case 'a':
			if (sscanf(optarg, "%d", &ad) == 1)
			{
				if (( ad < 0) || (ad > 126))
				{
					fprintf (stderr, "address (-a) must be between 0 and 126.\n", optopt);
					return 1;
				}
				else
				{
					add = ad;
					aset++;
				}
			}
			break;
		case 's':
			if (sscanf(optarg, "%d", &sl) == 1)
			{
				if (( sl < 0) || (sl > 255))
				{
					fprintf (stderr, "slot (-s) must be between 0 and 255.\n", optopt);
					return 1;
				}
				else
				{
					slo = sl;
					sset++;
				}
			}
			break;
		case 'i':
			if (sscanf(optarg, "%d", &in) == 1)
			{
				if (( in < 0) || (in > 255))
				{
					fprintf (stderr, "index (-i) must be between 0 and 255.\n", optopt);
					return 1;
				}
				else
				{
					ind = in;
					iset++;
				}
			}
			break;
		case '?':
			if ((optopt == 'a') || (optopt == 's') || (optopt == 'i'))
			fprintf (stderr, "Option -%c requires an argument.\n", optopt);
			else if (isprint (optopt))
			fprintf (stderr, "Unknown option `-%c'.\n", optopt);
			else
			fprintf (stderr,
					"Unknown option character `\\x%x'.\n",
					optopt);
			return 1;
		default:
			abort ();
		}

	for (index = optind; index < argc; index++)
		fprintf (stderr, "Non-option argument %s\n", argv[index]);
	

	if ((aset != 1) || (sset != 1) || (iset != 1))
	{
		fprintf (stderr, "Arguments missing:\n");
		if (aset != 1)
			fprintf (stderr, "-a address\n");
		if (sset != 1)
			fprintf (stderr, "-s slot\n");
		if (iset != 1)
			fprintf (stderr, "-i index\n");
		return 1;
	}
	
	srand(time(NULL));
	unsigned char fla = 0xff & rand();
	
	printf ("run ...\n");
	
	printf ("f = %d, a = %d, b = %d, c = %d\n", fla, add, slo, ind);

	readparam(fla, add, slo, ind);
	
	return 0;
}
