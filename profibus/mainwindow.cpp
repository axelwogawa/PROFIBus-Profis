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
 int socket_fd;


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

void MainWindow :: insertThreeIntoTableRow(QString item1, QString item2,QString item3){
    int column = ui->tableWidget->currentColumn();
    int row = ui->tableWidget->currentRow();
    ui->tableWidget->setItem(row, column, new QTableWidgetItem(item1));
    ui->tableWidget->setItem(row, column+1, new QTableWidgetItem(item2));
    ui->tableWidget->setItem(row, column+2, new QTableWidgetItem(item3));
    ui->tableWidget->setCurrentCell(row+1,0,QItemSelectionModel::ClearAndSelect);
    ui->tableWidget->show();
}

void MainWindow::connectGateway()
{
//    ui->gateway_connect->click();
}

int MainWindow::create_socket() {
   setName ( "entering parameterread... " );
    int iSockFd = -1;
    iSockFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (iSockFd == -1)
    {
        printf("socket fail\n");
        setName ("socket fail\n");
        setName("socket failed: are you connected to a vpn ?");
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
       // int help = close(iSockFd);
        printf("Socked closed: %i. Errno: %i\n", errno);
         setName("Socked closed: %i. Errno: %i\n"  + errno);
         exit(1);
    }
    setName( "SUCCESFULLY INITIALIZED UDP SOCKET" );
    return iSockFd;
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

        printf("% 3d"+ input.first[12]);
        new_device->setId ( ( (int)input.first[1])*pow(2,8) + (int)input.first[2]) ;
        new_device->setRev_no(((int)input.first[3])*pow(2,8) + (int)input.first[4]);
        new_device->setNo_do(((int)input.first[5])*pow(2,8) + (int)input.first[6]);
        new_device->setNo_obj(((int)input.first[7])*pow(2,8) + (int)input.first[8]);
        new_device->setFst_idx(((int)input.first[9])*pow(2,8) + (int)input.first[10]);
        new_device->setNo_typ(((int)input.first[11])*pow(2,8) + (int)input.first[12]);
    }
}

//---parse_composite_directory doc---
//function interpreting all given values by input array of slot 1 index 1 request
//input:    value pair:
//          first - received bytes in an array of length 1024
//          second - number of received bytes

void MainWindow:: parse_composite_directory (std::pair<unsigned char*, int> input){
    int current = 0;
    int current1 =0;
    // static information first ( see page 124)
    unsigned char PB[1024];
    unsigned char TB[1024];
    unsigned char FB[1024];
    int total = input.second;
    int PB_slot = input.first[1];
    int PB_index  = int(input.first[2]);
    int PB_num    = int(input.first[4]) ;
    printf("PB_slot = % 3d,PB_index= % 3d,PB_num= % 3d ",PB_slot,PB_index ,PB_num);
    setName("PB_offset = " + QString::number(PB_slot));
    setName("PB_index = " + QString::number(PB_index));
   setName("Physical blocks = " + QString::number(PB_num));
    int TB_slot = input.first[5];
    int TB_index = input.first[6];
    int TB_num = input.first[8];
    int FB_slot = input.first[9];
    int FB_index = input.first[10];
    int FB_num = input.first[12];
    setName("Transducer Blocks = " + QString::number(TB_num));
    setName("Function Blocks = " + QString::number(FB_num));
    make_table(PB_num+TB_num+FB_num,3);
    // dynmaic content is going to be interpreted here
    for (int i = 0; (i/4) <PB_num;i=i+4){           // 4 Byte belong exactly to one block
        PB[i] = input.first[12+i+1]; // offset
        PB[i+1] = input.first [12 + i+2]; //index
        PB[i+2] = input.first [12+i+3]; // zero
        PB[i+3] = input.first [12+i+4]; // number of parameters in physical block
        insertThreeIntoTableRow(QString::number(PB[i]),QString::number(PB[i+1]),QString::number(PB[i+3]));
        current = 12+i+4;
    }
    for (int a = 0; (a/4)<TB_num;a=a+4){           // 4 Byte belong exactly to one block
        TB[a] = input.first[current+a+1]; // offset
        TB[a+1] = input.first [current +a +2]; //index
        TB[a+2] = input.first [current +a+3]; // zero
        TB[a+3] = input.first [current +a+4]; // number of parameters in transducer block
        insertThreeIntoTableRow(QString::number(TB[a]),QString::number(TB[a+1]),QString::number(TB[a+3]));
        current1 = current +a+4;
    }
    for (int b = 0; (b/4)<FB_num;b=b+4){           // 4 Byte belong exactly to one block
        FB[b] = input.first[current1+b+1]; // offset
        FB[b+1] = input.first [current1+b+2]; //index
        FB[b+2] = input.first [current1+b+3]; // zero
        FB[b+3] = input.first [current1+b+4]; // number of parameters in function block
        insertThreeIntoTableRow(QString::number(FB[b]),QString::number(FB[b+1]),QString::number(FB[b+3]));
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
std::pair<unsigned char *, int> readparam (int iSockFd, unsigned char flag, unsigned char address, unsigned char slot, unsigned char index)
{
    MainWindow main;
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
                printf(" % 3d,", buff[i]);      //im Array buff[] steht die Hex - Antwort des PROFIbus-Geräts
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
    socket_fd = create_socket();
    std::pair<unsigned char*, int> res(readparam(socket_fd,fla, add, slo, ind));
    setName( "established connection" );
    setName( "starting to interpret ..." );
    parse_doheader(res,&default_device);
    setName("\n");
    setName("#################################");
    setName("please press details to see device details");

}


void MainWindow::on_composite_list_directory_clicked(){
    setName("#################################");
    setName( "composite list is requested" );
    printf("composite");
    unsigned char add = 6;  //address of first device
    unsigned char slo = 1;  //slot of directory object header
    unsigned char ind = 1;
    srand(time(NULL));
    unsigned char fla = 0xff & rand();

    std::pair<unsigned char*, int> input = readparam(socket_fd,fla, add, slo, ind);
    parse_composite_directory(input);
      printf("f = %d, a = %d, s = %d, i = %d\n", fla, add, slo, ind);
      setName("done ... ");
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


QFile xml_file ("/Man_ID_Table_scaled.xml");
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
   // else {"starting element is: " + xml_reader.name();}
}

}



