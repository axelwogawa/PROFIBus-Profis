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
#include <model.h>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dp_device.h"
//#### xml file handling ###
#include <QFile>
#include <QXmlStreamReader>

 QString console = "";
 Dp_device default_device =  Dp_device(6, 1, 1, 1, 1, 1);

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
    console = console + "\n";
    console = console +  name;

    ui->textEdit->setText(console);

 }

/**
@docbookonly create a new table **/
void MainWindow::make_table(int max_row, int max_column){
    ui->tableWidget->setRowCount(max_row);
    ui->tableWidget->setColumnCount(max_column);
    ui->tableWidget->setHorizontalHeaderItem(0,new QTableWidgetItem("Setting"));
    ui->tableWidget->setHorizontalHeaderItem(1,new QTableWidgetItem("Value"));
    ui->tableWidget->setSortingEnabled(false);
    ui->tableWidget->setCurrentCell(0,0,QItemSelectionModel::ClearAndSelect);

}

void MainWindow :: insertIntoTableRow(QString item1, QString item2){
    int column = ui->tableWidget->currentColumn();
    int row = ui->tableWidget->currentRow();
    ui->tableWidget->setItem(row, column, new QTableWidgetItem(item1));
    ui->tableWidget->setItem(row, column+1, new QTableWidgetItem(item2));
    ui->tableWidget->setCurrentCell(row+1,0,QItemSelectionModel::ClearAndSelect);
    ui->tableWidget->show();
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
MainWindow main;
    if (input.second != 13)
    {
        new_device = nullptr;
    }
    else
    {

        new_device->setId ( ( (int)input.first[1])*pow(2,8) + (int)input.first[2]) ;
        new_device->setRev_no(((int)input.first[3])*pow(2,8) + (int)input.first[4]);
        new_device->setNo_do(((int)input.first[5])*pow(2,8) + (int)input.first[6]);
        new_device->setNo_obj(((int)input.first[7])*pow(2,8) + (int)input.first[8]);
        new_device->setFst_idx(((int)input.first[9])*pow(2,8) + (int)input.first[10]);
        new_device->setNo_typ(((int)input.first[11])*pow(2,8) + (int)input.first[12]);



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
    MainWindow main;
    main.setName ( "entering parameterread... " );
    int iSockFd = -1;

   // iSockFd = shutdown (iSockFd, SHUT_RDWR) ;//  Returns 0 on success, -1 for errors, No more receptions or transmissions

    iSockFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (iSockFd == -1)
    {
        printf("socket fail\n");
        main.setName ("socket fail\n");
        main.setName("socket failed: are you connected to a vpn ?");
        //exit(1);
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
     //   main.setName( "bind fail. Addess: %i, SocketFD: %i, Errno: %i\n" );
        int help = close(iSockFd);
        printf("Socked closed: %i. Errno: %i\n", help, errno);
     //   main.setName("Socked closed: %i. Errno: %i\n", help, errno);
     //   exit(1);
    }

     main.setName( "SUCCESFULLY INITIALIZE UDP SOCKET" );

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
    main.setName("number of received bytes" + QString::number( iRcvdBytes));

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
    setName("#################################");
    setName( "gateway connect was entered... " );
    //TODO: maybe we can loop through addresses 0 to 255 or so instead of hardcoding addresses
    unsigned char add = 6;  //address of first device
    unsigned char slo = 1;  //slot of directory object header
    unsigned char ind = 0;

    srand(time(NULL));
    unsigned char fla = 0xff & rand();

    printf("f = %d, a = %d, s = %d, i = %d\n", fla, add, slo, ind);
    setName( "trying to open a connection to the gateway ... ");
    std::pair<unsigned char*, int> res(readparam(fla, add, slo, ind));
    setName( "established connection" );

    setName( "starting to interpret ..." );
    parse_doheader(res,&default_device);
    setName("\n");
    setName("#################################");
    setName("please press details to see device details");

}

void MainWindow::on_disconnect_gateway_clicked(){ // do not get confused by the name. this button was originally made to handle a disconnect from the gateway
    setName( "disconnect gateway was clicked" );
    make_table(6,2);
   // Dp_device(int id_, int rev_no_, int no_do_, int no_obj_, int fst_idx_, int no_typ_);
   insertIntoTableRow("id", "" +QString::number(default_device.getId()));
   insertIntoTableRow("rev_no", "" +QString::number(default_device.getRev_no()));
   insertIntoTableRow("no_do", "" +QString::number(default_device.getNo_do()));
   insertIntoTableRow("no_obj", "" +QString::number(default_device.getNo_obj()));
   insertIntoTableRow("fst_idx", "" +QString::number(default_device.getFst_idx()));
   insertIntoTableRow("no_typ", "" +QString::number(default_device.getNo_typ()));

}

void MainWindow:: on_read_xml_clicked(){
QFile xml_file ("/home/zida20/Downloads/PROFIBus-Profis/profibus/Man_ID_Table_scaled.xml");
if(!xml_file.open(QFile::ReadOnly | QFile::Text)){

      setName("ERROR: cannot read file ...");
  }
else {setName("file was read succesfully .. ");}
QXmlStreamReader xml_reader (&xml_file);

if (xml_reader.readNextStartElement()) {
    if (xml_reader.name()=="Man_ID_Table") {
        setName("found the starting element");

         while(xml_reader.readNextStartElement()){
             if (xml_reader.name()!= "Manufacturer") {
                setName (xml_reader.name().toString());
                xml_reader.skipCurrentElement();

             }else {
                 setName (QString:: number( xml_reader.attributes().value("ID").toInt()));
             }



         } setName("ENd of if ");
    }
    else {"starting element is: " + xml_reader.name();}
}

}




