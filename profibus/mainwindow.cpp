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
#include "pb.h"
#include "tb.h"
#include "fb.h"
//#### xml file handling ###
#include <QFile>
#include <QXmlStreamReader>
#include <QApplication>
#include <blockidentifier.h>

// #### global variable definitions #######
 QString console = "";
 Dp_device default_device =  Dp_device(6, 1, 1, 1, 1, 1);
 int socket_fd;
 int connect_value1 =0; // needed to compare weather the clicked cell has changed or not
 int connect_value2 =0;
 enum currentAction{header,composite_directory,view,data};
 currentAction pointer;
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
    ui->textEdit->scrollToAnchor(name);

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
std::pair<std::vector<int>, int> readparam (int iSockFd, unsigned char flag, unsigned char address, unsigned char slot, unsigned char index)
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
    std::vector<int> vector(buff, buff+iRcvdBytes);
    std::pair<std::vector<int>, int> res(vector, iRcvdBytes);
    return res;
}





void MainWindow::make_dynamic_table (QString tableWidget,int coloumns,int row, QStringList coloumn_names) {
   QTableWidget* table;
    if (0== QString::compare(tableWidget, "tableWidget", Qt::CaseInsensitive)) {
      table =  ui->tableWidget;
    }
    else if (0== QString::compare(tableWidget, "tableWidget_2", Qt::CaseInsensitive)){
        table =  ui->tableWidget_2;
    }
    table->clear();
    table->setRowCount(row);
    table->setColumnCount(coloumns);
    for (int i = 0; i<coloumns;i++){

        table->setHorizontalHeaderItem(i,new QTableWidgetItem(coloumn_names.at(i)));
    }
    table->setSortingEnabled(false);
    table->setCurrentCell(0,0,QItemSelectionModel::ClearAndSelect);
    table->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    table->setStyleSheet("background:#778899;");
}

void MainWindow::insert_row_into_table(QString tableWidget,int coloumns,QStringList coloumn_names){
    QTableWidget* table;
     if (0== QString::compare(tableWidget, "tableWidget", Qt::CaseInsensitive)) {
       table =  ui->tableWidget;
     }
     else if (0== QString::compare(tableWidget, "tableWidget_2", Qt::CaseInsensitive)){
         table =  ui->tableWidget_2;
     }
    int column = table->currentColumn();
    int row = table->currentRow();
    for (int i=0;i<coloumns;i++){
    table->setItem(row, column+i, new QTableWidgetItem(coloumn_names.at(i)));
    }
    if (pointer==composite_directory) {
    connect(table, SIGNAL( cellDoubleClicked (int, int) ),
     this, SLOT( requestParameters( int, int ) ) );}

    if (pointer==view) {
    connect(table, SIGNAL( cellDoubleClicked (int, int) ),
     this, SLOT( requestValues( int, int ) ) );}

    table->setSortingEnabled(false);
    table->setCurrentCell(row+1,0,QItemSelectionModel::ClearAndSelect);
    table->adjustSize();
    table->show();
}
void MainWindow::requestValues(int nRow, int nCol){

    if ((nRow == connect_value1) &&(nCol== connect_value2)) {}
    else {
    printf("clickedValues");
    setName("Row" + QString::number(nRow) + "and Coloumn: " + QString::number(nCol) + "was clicked");
    // to be continued here !
    connect_value1=nRow;
    connect_value2=nCol;

    unsigned char add = 6;  //address of first device
    bool ok;
    unsigned char slo = (ui->tableWidget_2->item(nRow,1)->text()).toInt(&ok,10);
    unsigned char ind = (ui->tableWidget_2->item(nRow,2)->text()).toInt(&ok,10);
    srand(time(NULL));
    unsigned char fla = 0xff & rand();
    printf("f = %d, a = %d, s = %d, i = %d\n", fla, add, slo, ind);
    setName( "trying to open a connection to the gateway ... ");
    //read DO header
    std::pair<std::vector<int>, int> res = readparam(socket_fd,fla, add, slo, ind);
    // regarding page 150: 18 Blocks are going to be received, the last 5 blocks belong to the out (value + status in Data Type 101)
    // regarding to page 52:  Data Type 101 consists of 4 Byte value + 1 byte status
    int status = res.first.at(res.second-1);
    int value = res.first.at(res.second-5) + res.first.at(res.second-4) + res.first.at(res.second-3) + res.first.at(res.second-2);
    setName("Value of FB1: " + QString::number(value));
    }


}


void MainWindow::requestParameters(int nRow, int nCol){
    blockIdentifier blockIdent;
    if ((nRow == connect_value1) &&(nCol== connect_value2)) {}
    else {

    setName("Row" + QString::number(nRow) + "and Coloumn: " + QString::number(nCol) + "was clicked");
    // to be continued here !
    connect_value1=nRow;
    connect_value2=nCol;
    pointer = view;
    unsigned char add = 6;  //address of first device
    bool ok;
    unsigned char slo = (ui->tableWidget->item(nRow,1)->text()).toInt(&ok,10);
    unsigned char ind = (ui->tableWidget->item(nRow,2)->text()).toInt(&ok,10);
    srand(time(NULL));
    printf("clickedadasdasdasdsad");
    unsigned char fla = 0xff & rand();
    printf("f = %d, a = %d, s = %d, i = %d\n", fla, add, slo, ind);
    setName( "trying to open a connection to the gateway ... ");
    //read DO header
    std::pair<std::vector<int>, int> res = readparam(socket_fd,fla, add, slo, ind);
    ui->label_3->setText("Requestest Slot" + QString::number(slo) + " and Index" + QString::number( ind));
    slo = res.first.at(res.second-3);
    ind = res.first.at(res.second-2);
    unsigned char list_views = res.first.at(res.second-1);
    QList<QString> list = blockIdent.getBlockIdentity(2,1,1);
    make_dynamic_table("tableWidget_2",6,1,{"block","slot","index","parent","class",""});
    setName( "found" +list.first() + QString::number(list_views) + "List views. List view 1 can be referenced by  Index" + QString::number(ind) + "and Slot" + QString::number(slo));
    insert_row_into_table("tableWidget_2",6,{list.at(2),QString::number(slo),QString::number(ind),list.at(1),list.at(0),"readValue"});
    pointer = view;
    }

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
void parse_doheader(std::pair<std::vector<int>, int>& input, Dp_device& new_device){
MainWindow main;
    if (input.second == 13)
    {

        printf("% 3d"+ input.first.at(12));
        new_device.setId ( ( (int)input.first.at(1))*pow(2,8) + (int)input.first.at(2)) ;
        new_device.setRev_no(((int)input.first.at(3))*pow(2,8) + (int)input.first.at(4));
        new_device.setNo_do(((int)input.first.at(5))*pow(2,8) + (int)input.first.at(6));
        new_device.setNo_obj(((int)input.first.at(7))*pow(2,8) + (int)input.first.at(8));
        new_device.setFst_idx(((int)input.first.at(9))*pow(2,8) + (int)input.first.at(10));
        new_device.setNo_typ(((int)input.first.at(11))*pow(2,8) + (int)input.first.at(12));
    }
}

//---parse_composite_directory doc---
//function interpreting all given values by input array of slot 1 index 1 request
//input:    value pair:
//          first - received bytes in an array of length 1024
//          second - number of received bytes

void MainWindow:: parse_composite_directory (std::pair<std::vector<int>, int>& input, Dp_device& dev){
    // static information first ( see page 124)
    pointer=composite_directory;
    //Physical block
    int PB_num = int(input.first.at(4)) ;  //usually not required, because there's always exactly one PB
    int TB_num = int(input.first.at(7))*pow(2,8) + int(input.first.at(8));
    int FB_num = int(input.first.at(11))*pow(2,8) + int(input.first.at(12));
    make_dynamic_table("tableWidget",5,FB_num+TB_num+PB_num,{"BlockType","Slot","Index","Parameters",""});
    ui->label_2->setText("Composite Directory: SLOT 1 INDEX 1");
    dev.getPb().setDo_index(input.first.at(1));
    dev.getPb().setDo_offset(input.first.at(2));
    dev.getPb().setSlot(int(input.first.at(12+1)));
    dev.getPb().setIndex(int(input.first.at(12+2)));
    dev.getPb().setNo_params(int(input.first.at(12+3))*pow(2,8) + int(input.first.at(12+4)));

    //printf("PB_slot = % 3d,PB_index= % 3d,PB_num= % 3d ",dev.getPb().getSlot(),dev.getPb().getIndex() ,dev.getPb().getNo_params());

    setName("PB_offset = " + QString::number(dev.getPb().getSlot()));
    setName("PB_index = " + QString::number(dev.getPb().getIndex()));
    setName("Physical blocks = " + QString::number(PB_num));

   insert_row_into_table("tableWidget",5,{"PB",QString::number(dev.getPb().getSlot()),QString::number(dev.getPb().getIndex()),QString::number(dev.getPb().getNo_params()),"read param"});
    int current = 12+4;
    //Transducer blocks
    int i;

    for (i = 0; i < TB_num; i++)
    {
        TB tb;
        tb.setDo_index(int(input.first.at(5)));
        tb.setDo_offset(int(input.first.at(6)));
        tb.setSlot(int(input.first.at(current+i*4+1)));
        tb.setIndex(int(input.first.at(current+i*4+2)));
        tb.setNo_params(int(input.first.at(current+i*4+3))*pow(2,8) + int(input.first.at(current+i*4+4)));
        dev.getTbs().push_back(tb);
        printf("TB_slot = % 3d,TB_index= % 3d,TB_num= % 3d ",tb.getSlot(),tb.getDo_index(),tb.getNo_params());
      insert_row_into_table("tableWidget",5,{"TB",QString::number(tb.getSlot()),QString::number(tb.getIndex()),QString::number(tb.getNo_params()),"read param"});
    }
    current += i*4;

    //Function blocks
    //int FB_num = int(input.first.at(11))*pow(2,8) + int(input.first.at(12));
    FB fb;
    for (i = 0; i < FB_num; i++)
    {

        fb.setDo_index(int(input.first.at(9)));
        fb.setDo_offset(int(input.first.at(10)));
        fb.setSlot(int(input.first.at(current+i*4+1)));
        fb.setIndex(int(input.first.at(current+i*4+2)));
        fb.setNo_params(int(input.first.at(current+i*4+3))*pow(2,8) + int(input.first.at(current+i*4+4)));
        dev.getFbs().push_back(fb);
        printf("FB_slot = % 3d,FB_index= % 3d,FB_num= % 3d ",fb.getSlot(),fb.getDo_index(),fb.getNo_params());
        //insertThreeIntoTableRow(QString::number(fb.getSlot()),QString::number(fb.getIndex()),QString::number(fb.getNo_params()));
        insert_row_into_table("tableWidget",5,{"FB",QString::number(fb.getSlot()),QString::number(fb.getIndex()),QString::number(fb.getNo_params()),"read param"});

    }
    setName("Transducer Blocks = " + QString::number(TB_num));
    setName("Function Blocks = " + QString::number(FB_num));

    //insert_row_into_table(5,{"TB",QString::number(fb.getSlot()),QString::number(fb.getIndex()),QString::number(fb.getNo_params()),"click here"});
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

    //read DO header
    std::pair<std::vector<int>, int> res = readparam(socket_fd,fla, add, slo, ind);
    setName( "established connection" );
    setName( "starting to interpret ..." );
    parse_doheader(res, default_device);
    setName("\n");
    setName("#################################");
    setName("please press details to see device details");
    //print DO read results

    //read DO object
    setName("#################################");
    setName( "composite list is requested" );
    printf("composite");
    srand(time(NULL));
    fla = 0xff & rand();
    ind = 1;
    res = readparam(socket_fd,fla, add, slo, ind);
    parse_composite_directory(res, default_device);
    printf("f = %d, a = %d, s = %d, i = %d\n", fla, add, slo, ind);
    setName("done ... ");
    ui->gateway_connect->setStyleSheet("background-color: green;");
    ui->gateway_connect->setText("connected");


    //create drop down menus for buttons "read TB", "read FB"
}


void MainWindow::on_composite_list_directory_clicked(){    
}

void MainWindow::on_disconnect_gateway_clicked(){ // do not get confused by the name. this button was originally made to handle a disconnect from the gateway
    setName( "disconnect gateway was clicked" );
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

    printf("reading fb1");
    unsigned char add = 6;  //address of first device
    unsigned char slo = 1;  //slot of directory object header
    unsigned char ind = 80;
    srand(time(NULL));
    unsigned char fla = 0xff & rand();
    printf("f = %d, a = %d, s = %d, i = %d\n", fla, add, slo, ind);
    setName( "trying to open a connection to the gateway ... ");


    //read DO header
    std::pair<std::vector<int>, int> res = readparam(socket_fd,fla, add, slo, ind);
}


void parse_PB(std::pair<std::vector<int>, int>& input, Dp_device& dev){

    //parse device class
    switch(input.first.at(3)) {
    case 1:
        dev.getPb().setCls("Transmitter");
        break;
    case 2:
        dev.getPb().setCls("Actuator");
        break;
    case 3:
        dev.getPb().setCls("Discrete I/O");
        break;
    case 4:
        dev.getPb().setCls("Controller");
        break;
    case 5:
        dev.getPb().setCls("Analyser");
        break;
    case 6:
        dev.getPb().setCls("Lab Device");
        break;
    default:
        if(input.first.at(3)>=128 && input.first.at(3) <= 249)
            dev.getPb().setCls("vendor specific");
        else
            dev.getPb().setCls("unknown");
        break;
    }

    //parse block view address
    dev.getPb().setView_slot(int(input.first.at(18)));
    dev.getPb().setView_index(int(input.first.at(19)));
}

void MainWindow::on_readPb_clicked()
{
    srand(time(NULL));
    unsigned char fla = 0xff & rand();
    unsigned char add = (unsigned char)default_device.getAdd();
    unsigned char slo = (unsigned char)default_device.getPb().getSlot();
    unsigned char ind = (unsigned char)default_device.getPb().getIndex();
    std::pair<std::vector<int>, int> input = readparam(socket_fd,fla, add, slo, ind);
    parse_PB(input, default_device);
    //show results
}
