#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include <pthread.h>
#include <dlfcn.h>
#include <cmath>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <ctype.h>
#include <unistd.h>

#include <time.h>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dp_device.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setName(const QString &name)
 {
  ui->lineEdit->setText(name);
 }

void MainWindow::connectGateway()
{
//    ui->gateway_connect->click();
}


//---parse_doheader doc---
//function initialising all attributes of a Dp_device object with the values received from a
//directory object header read request
//input:    value pair:
//          first - received bytes in an array of length 1024
//          second - number of received bytes
void parse_doheader(std::pair<unsigned char*, int> input, Dp_device* new_device){
    if (input.second != 13)
    {
        new_device = nullptr;
    }
    else
    {
        *new_device->setId(((int)input.first[1])*pow(2,8) + (int)input.first[2]);
        *new_device->setRev_no(((int)input.first[3])*pow(2,8) + (int)input.first[4]);
        //to be continued here: set the other attributes of new_device
    }
}


//---readparam doc---
//function to read parameters from profibus device via UDP socket and internet proxy at
//141.76.82.170:12345
//input:    flag - message id
//          address - profibus address of the device one wants to read from
//          slot - slot of the object to read
//          index - index of the object to read
//output:   value pair:
//          first - received bytes in an array of length 1024
//          second - number of received bytes
std::pair<unsigned char *, int> readparam (unsigned char flag, unsigned char address, unsigned char slot, unsigned char index)
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
        printf("bind fail. Addess: %i, SocketFD: %i, Errno: %i\n", INADDR_ANY, iSockFd, errno);
        int help = close(iSockFd);
        printf("Socked closed: %i. Errno: %i\n", help, errno);
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
        close(iSockFd);
        exit(1);
    }
    printf("ok.\n");


    struct sockaddr_in cliAddr;
    memset(&cliAddr, 0, sizeof(cliAddr));
    socklen_t cliLen;

    unsigned char buff[1024];

    printf("receiving...\n");
    int iRcvdBytes = recvfrom(iSockFd, buff, 1024, 0, (struct sockaddr*)&cliAddr, &cliLen);

    printf("received %d Byte(s)\n", iRcvdBytes);	//AW

    if (iRcvdBytes > 0)
    {
        if (buff[0] == sbuf[0])
        {
            int i;
            for (i = 0; i < iRcvdBytes; i++)
                printf(" % 3d,", buff[i]);      //im Array buff[] steht die Antwort des PROFIbus-Geräts
                printf("== \n");
//            for (i = 0; i < iRcvdBytes; i++)
//                printf("0x%02x,", buff[i]);
//            printf("\n");
        }
//        else
//            printf("receive fail: frame marker differs - sent: %d != rcvd: %d\n", sbuf[0], buff[0]);
    }
    else
//        printf("receive fail %d\n", iRcvdBytes);

    close(iSockFd);

    std::pair<unsigned char*, int> res(buff, iRcvdBytes);
    return res;
}






void MainWindow::on_gateway_connect_clicked()
{
    //TODO: maybe we can loop through addresses 0 to 255 or so instead of hardcoding addresses
    unsigned char add = 6;  //address of first device
    unsigned char slo = 1;  //slot of directory object header
    unsigned char ind = 0;

    srand(time(NULL));
    unsigned char fla = 0xff & rand();

    printf("f = %d, a = %d, s = %d, i = %d\n", fla, add, slo, ind);

    std::pair<unsigned char*, int> res(readparam(fla, add, slo, ind));
    parse_doheader(res);
}
