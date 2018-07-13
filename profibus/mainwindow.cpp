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
#include <parser.h>

// #### global variable definitions #######
 QString console = "";
 Dp_device default_device =  Dp_device(6, 1, 1, 1, 1, 1);
 int socket_fd;
 int connect_value1 =0; // needed to compare weather the clicked cell has changed or not
 int connect_value2 =0;
 enum currentAction{device,header,composite_directory,view,data};
// enum tableWidget{tableWidget_0,tableWidget_1,tableWidget_2,tableWidget_3} if we have too much time
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
    else if (0== QString::compare(tableWidget, "tableWidget_0", Qt::CaseInsensitive)){
        table =  ui->tableWidget_0;
    }
    else if (0== QString::compare(tableWidget, "tableWidget_3", Qt::CaseInsensitive)){
        table =  ui->tableWidget_3;
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
     else if (0== QString::compare(tableWidget, "tableWidget_0", Qt::CaseInsensitive)){
         table =  ui->tableWidget_0;
     }
     else if (0== QString::compare(tableWidget, "tableWidget_3", Qt::CaseInsensitive)){
         table =  ui->tableWidget_3;
     }
    int column = table->currentColumn();
    int row = table->currentRow();
    for (int i=0;i<coloumns;i++){
    table->setItem(row, column+i, new QTableWidgetItem(coloumn_names.at(i)));
    }

    switch (pointer) {
    case device: connect(table, SIGNAL( cellDoubleClicked (int, int) ),
                         this, SLOT(requestHeader( int, int ) ) );
        break;
    case composite_directory: connect(table, SIGNAL( cellDoubleClicked (int, int) ),
                                      this, SLOT( requestParameters( int, int ) ) );
        break;
    case  view: connect(table, SIGNAL( cellDoubleClicked (int, int) ),
                       this, SLOT( requestValues( int, int ) ) );
        break;

    default:
        break;
    }


    table->setSortingEnabled(false);
    table->setCurrentCell(row+1,0,QItemSelectionModel::ClearAndSelect);
    table->adjustSize();
    table->show();
}

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


void MainWindow::requestHeader( int nRow, int nCol ) {
    if ((nRow == connect_value1) &&(nCol== connect_value2)) {}
    else {
        connect_value1=nRow;
        connect_value2=nCol;
        bool ok;
        printf("requesting row = %d",nRow);
         unsigned char add = (ui->tableWidget_0->item(nRow,0)->text()).toInt(&ok,10);

        unsigned char slo = 1;
        unsigned char ind = 0; //(ui->tableWidget_2->item(nRow,2)->text()).toInt(&ok,10);
        srand(time(NULL));
        unsigned char fla = 0xff & rand();
        printf("f = %d, a = %d, s = %d, i = %d\n", fla, add, slo, ind);
        setName( "trying to open a connection to the gateway ... ");
        //read DO header
        std::pair<std::vector<int>, int> res = readparam(socket_fd,fla, add, slo, ind);
        parse_doheader(res, default_device);
        make_dynamic_table("tableWidget_3",2,6,{"type","value"});
        insert_row_into_table("tableWidget_3",2,{"Device ID",QString::number(default_device.getId())});
        insert_row_into_table("tableWidget_3",2,{"Revision Number",QString::number(default_device.getRev_no())});
        insert_row_into_table("tableWidget_3",2,{"Number of Directory Objects", QString::number(default_device.getNo_do())});
        insert_row_into_table("tableWidget_3",2,{"Number of Objects", "" +QString::number(default_device.getNo_obj())});
        insert_row_into_table("tableWidget_3",2,{"Index of First Object", "" +QString::number(default_device.getFst_idx())});
        insert_row_into_table("tableWidget_3",2,{"Number of Types", "" +QString::number(default_device.getNo_typ())});
        ui->label_5->setText("Header information of device " + add);
        ind =1;
        pointer = composite_directory;
        res = readparam(socket_fd,fla, add, slo, ind);
        parse_composite_directory(res, default_device);



    }
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
    setName("Value of FB: " + QString::number(value));
    }
}



void MainWindow::requestParameters(int nRow, int nCol){
    blockIdentifier blockIdent;
    if ((nRow == connect_value1) &&(nCol== connect_value2)) {}
    else
    {
        setName("Row" + QString::number(nRow) + "and Coloumn: " + QString::number(nCol) + "was clicked");
        // to be continued here !

        //request block object
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
        std::pair<std::vector<int>, int> blockObj = readparam(socket_fd,fla, add, slo, ind);
        //interpret block object content
        QList<QString> list = blockIdent.getBlockIdentity(blockObj.first.at(2),blockObj.first.at(3),blockObj.first.at(4));

        QList<QPair<QString,QString>> addParams;

        //request standard param "mode" (rel. idx. 6)
        srand(time(NULL));
        fla = 0xff & rand();
        std::pair<std::vector<int>, int> mode = readparam(socket_fd,fla, add, slo, ind+6);
        QPair<QString,QString> res("Actual Mode", Parser::ActMode(mode.first.at(1)));
        addParams.append(res);

        //request standard param "alarms" (rel. idx. 7)
        srand(time(NULL));
        fla = 0xff & rand();
        std::pair<std::vector<int>, int> alarms = readparam(socket_fd,fla, add, slo, ind+7);
        res.first = "Alarms";
        res.second = Parser::ActAlarm(alarms.first.at(1));
        addParams.append(res);

        //request PB params
        if (list.at(2).compare("PB") == 0)
        {
            //request standard param "manID" (rel. idx. 10)
            srand(time(NULL));
            fla = 0xff & rand();
            std::pair<std::vector<int>, int> val = readparam(socket_fd,fla, add, slo, ind+10);
            val.first.erase(val.first.begin());
            int manId = Parser::US16toInt(val.first);
            //TODO: parse manID (using xml)
            res.first = "Manufacturer";
            res.second = QString::number(manId);    //replace this with the manufacturer name String
            addParams.append(res);

            //request standard param "DevID" (rel. idx. 11)
            srand(time(NULL));
            fla = 0xff & rand();
            val = readparam(socket_fd,fla, add, slo, ind+11);
            val.first.erase(val.first.begin());
            res.first = "Device Name";
            res.second = Parser::VisibleStringToString(val.first);    //not sure, how to interpret Visible String --> this function reads only gibberish
            addParams.append(res);
        }
        else if (list.at(2).compare("FB") == 0)
        {
            if (list.at(1).compare("Input") == 0 || list.at(1).compare("Output") == 0 || list.at(1).compare("Calculation") == 0){
                //request standard param "OUT" (rel. idx. 10)
                srand(time(NULL));
                fla = 0xff & rand();
                std::pair<std::vector<int>, int> val = readparam(socket_fd,fla, add, slo, ind+10);
                val.first.erase(val.first.begin());
                float value = Parser::Float101toFloat(val.first);
                res.first = "Output Value";
                res.second = QString::number(value);
                addParams.append(res);
                //interpret value status
                res.first = "Value Status";
                res.second = Parser::Float101Status(val.first.at(4));
                addParams.append(res);

                //request engineering unit
                if (list.at(0).compare("Analog") == 0) {
                    //request standard param "scale" (rel. idx. 12)
                    srand(time(NULL));
                    fla = 0xff & rand();
                    printf("f = %d, a = %d, s = %d, i = %d\n", fla, add, slo, ind);
                    val = readparam(socket_fd,fla, add, slo, ind+12);
                    val.first.erase(val.first.begin());
                    res.first = "Unit";
                    res.second = Parser::parseDS36Unit(val.first);
                    addParams.append(res);
                }
                else if (list.at(0).compare("Totaliser") == 0) {
                    //request standard param "unit" (rel. idx. 11)
                    srand(time(NULL));
                    fla = 0xff & rand();
                    printf("f = %d, a = %d, s = %d, i = %d\n", fla, add, slo, ind);
                    val = readparam(socket_fd,fla, add, slo, ind+11);
                    val.first.erase(val.first.begin());
                    res.first = "Unit";
                    res.second = Parser::parseUnitId(Parser::US16toInt(val.first));
                    addParams.append(res);
                }
            }
        }

        //print results
        ui->label_3->setText("Requested Slot " + QString::number(slo) + " and Index " + QString::number(ind));
        unsigned char list_views = blockObj.first.at(blockObj.second-1);
        //printf("2= %d,1= %d,1= %d ",blockObj.first.at(1),blockObj.first.at(2),blockObj.first.at(3));
        //make_dynamic_table("tableWidget_2",6,1,{"block","slot","index","parent","class",""});
        make_dynamic_table("tableWidget_2",2,5+addParams.size(),{"Parameter","Value"});
        setName("found " + list.first() + QString::number(list_views) + " List views. List view 1 can be referenced by Index " + QString::number(ind) + " and Slot " + QString::number(slo));
        //insert_row_into_table("tableWidget_2",6,{list.at(2),QString::number(blockObj.first.at(blockObj.second-3)),QString::number(blockObj.first.at(blockObj.second-2)),list.at(1),list.at(0),"readValue"});
        insert_row_into_table("tableWidget_2",2,{"Block Type",list.at(2)});
        insert_row_into_table("tableWidget_2",2,{"View Slot",QString::number(blockObj.first.at(blockObj.second-3))});
        insert_row_into_table("tableWidget_2",2,{"View Index",QString::number(blockObj.first.at(blockObj.second-2))});
        insert_row_into_table("tableWidget_2",2,{"Parent Class",list.at(1)});
        insert_row_into_table("tableWidget_2",2,{"Subclass",list.at(0)});
        for(int i = 0; i < addParams.size(); i++)
        {
            insert_row_into_table("tableWidget_2",2,{addParams.at(i).first,addParams.at(i).second});
        }
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

/**
 @docbookonly function to loop through the address space of a connected profibus appikation.
    The answer returned by the device should contain exactly 13 Byte which represent the header
**/
void MainWindow::searchFieldBusDevices (){
    QList<int>  devices;
    unsigned char slo = 1;  //slot of directory object header
    unsigned char ind = 0;
    srand(time(NULL));
    unsigned char fla = 0xff & rand();
    printf("f = %d, a = %d, s = %d, i = %d\n", fla, slo, ind);
   // setName( "trying to open a connection to the gateway ... ");
    socket_fd = create_socket();

    for (int i =0; i<128; i++){

        std::pair<std::vector<int>, int> res = readparam(socket_fd,fla, i, slo, ind);
        if (res.second ==13)
        {
           devices.append(i);
           setName(QString::number(i));
        }

    }
   make_dynamic_table("tableWidget_0",2,devices.size(),{"Address",""});
    for (int i=0;i<devices.size();i++){
        insert_row_into_table("tableWidget_0",2,{QString::number(devices.at(i)),"examine"});
    }
}

void MainWindow::on_gateway_connect_clicked()
{
pointer = device;
searchFieldBusDevices ();
}



void MainWindow:: on_read_xml_clicked(){
    QFile xml_file ("./Man_ID_Table_scaled.xml");
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


    //parse block view address
    dev.getPb().setView_slot(int(input.first.at(18)));
    dev.getPb().setView_index(int(input.first.at(19)));
}

/*
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
*/
