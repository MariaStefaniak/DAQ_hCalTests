/**
 * @file MyDaq.cpp
 * Implementation for MyDaq class
 *
 * @brief
 *
 * @author Maria Stefaniak
 */

#include "MyDaq.h"
#include "Module.h"
//#include "MyMainFrame.h"

#include <stdio.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <atomic>
#include <chrono>
#include <ctime>

#include <TTree.h>
#include <TFile.h>
#include <TH1.h>
#include <TSystem.h>
#include <TCanvas.h>

#include <TApplication.h>
#include <TGClient.h>


#include "TF1.h"
#include "TApplication.h"
#include "TCanvas.h"
#include "TRootCanvas.h"

#include <iostream>

static const int STACK_VERBOSE = 1;

MyDaq :: MyDaq () {
    m_nWordsExpectedPerEvent = 0;
    stopFlag = false;
}

MyDaq :: ~MyDaq() {

}

void MyDaq :: AddModule( Module* module ) {
    m_Modules.push_back(module);
  // here was sth about share data [?]
}

void MyDaq :: PrintModules(){
    std::cerr << " Connected and listed modules: " <<std::endl;
    for(auto& module : m_Modules) module->Print();
    std::cerr << "\n" << std::endl;
}

int MyDaq :: Run(){
/*
    /// opening the window application
    m_Appliaction = new TApplication("App", NULL, NULL);
    m_myMainFrame = new MyMainFrame(gClient->GetRoot(), 400, 400, this);
    int ccc = 1;
    while(ccc){
        m_Appliaction->Run( kTRUE );
        std::cout << "ccc: " << std::endl;
        std::cin >> ccc;
    }
   // m_t_app = new std::thread( [this] { m_Appliaction->Run( kTRUE ); } );
    
 //   m_t_app->join();
    
    */
    
    

    /// We start the run //
    std::cerr << " ******  \n Start the Run! \n MyDaq::Run \n ****** " << std::endl;
    /// Check if we have XX_USB connected:
    if(!InitializeCAMAC()) return 0;
    
    std::cerr << "\n 1. CAMAC initialized !\n" <<std::endl;
    /// Print Info about connected divices:
    PrintModules();
    /// Preparation and loading the STACK:
    PrepareAndLoadStack();
    InitializeModulesOutput();
    
    
    ///output preparetion
    histogram = new TH1D("adc ch 4", " adc; adc; counts", 200, 0, 200);
    CreateOutputTree();
    ///test of canvas
    c_adc         = new TCanvas();
    int ready = 0;
    std::atomic<bool> stopFlag(false);
    std::cout<<"Ready? press 1: " << std::endl;
    std::cin>>ready;
    if(ready){
        
        InitializeDAQ();
        m_t_app = new std::thread( [this] {TurnOff();  } );
        m_t_daq = new std::thread( [this] {TakeData(); } );
        // Wait for user input to stop the threads
        m_t_daq->join();
        m_t_app->join();
        
    }

    histogram -> Draw();
    // current date/time based on current system
    time_t now = time(0);
    // convert now to string form
    char* dt = ctime(&now);
    
    c_adc->SaveAs(Form("h_channel4_%s.pdf", dt));
    ///end of canvas test
    Finalize();
    
   
    /*
    bool start_flag = 1;
    while(start_flag){
        /// Starting DAQ:
        InitializeDAQ();
        /// Data taking:
        int readBytes = -1;
        int nAttemptsToRead = 1;
        while(readBytes < 0){
            /// stupid attempt START STOP
            std::cerr << "Click the triggers ~5 times, and type 5 when you done it " << std::endl;
            int start = 0;
            std::cin >> start;
            if(start==5){
                readBytes = TakeData();
                std::cerr << "Attempt to read: " << nAttemptsToRead << std::endl;
                nAttemptsToRead++;
            } //end of if start
        }// endo of while
        std::cerr<<"STOP = 0; Continue = 1"<<std::endl;
        std::cin>>start_flag;
    }
    Finalize();*/

    
}


int MyDaq :: InitializeCAMAC(){
    std::cerr << " ******  \n MyDaq::InitializeCAMAC \n ****** " << std::endl;
    
    int Qcamac, Xcamac; /// the return values from CAMAC:
    ///X - signal idnication that command was accepted
    ///Q - signal depending on the function of code of the command
    
    long DummyCamac; // CAMAC_read should not read anything, so the output is whatever
    
    std::cerr << "Find the XX_USB device" << std::endl;
    //Find XX_USB devices and open the first one found
    xxusb_devices_find(devices);
    dev = devices[0].usbdev;
    //check if found anything:
    if(dev==nullptr){
        std::cerr<<"\n No XX_USB device found \n" << std::endl;
    }
    else{
        std::cerr<<"\n XX_UBS found \n" << std::endl;
    }
    // this will be the obcject which handels the Controler
    uHandle = xxusb_device_open(dev);
    //check if it is opened:
    if(!uHandle){
        std::cerr << "\n Opening of CC_USB controler failed \n" << std::endl;
        return 0;
    }
    else{
        CAMAC_read(uHandle, 25, 0, 0, &m_firmwareId, &Qcamac, &Xcamac); // 25-0-0 Read Firmware ID
        std::cerr << " \n Found and opened the connection with CC_USB (firmware ID): " << m_firmwareId << "\n " << std::endl;
    }
    /// checking basic functions:
    /// Camac Initializaton Z:
    short someReturn = -1;
    someReturn = CAMAC_Z(uHandle);
    std::cerr << "Initailization return: " << someReturn << "\n " << std::endl;
    if(someReturn < 0){
        std::cerr << "\n Initialization failed  \n " <<std::endl; return 0;
    }
    /// Camac Inhibit (device, 0-off, 1-on) :
    someReturn = CAMAC_I(uHandle, 1);
    /// Camac write 24bit word to Camac address (device ,N- station nr, A- subaddress, F - function, data written to CCUSB, Q,X) :
    if(someReturn > 0)someReturn = CAMAC_write(uHandle, 1,0,16,0xa,&Qcamac,&Xcamac); //dont know F=16?, return 2
    /// Camac read similar to write:
    if(someReturn > 0)someReturn = CAMAC_read(uHandle, 1,0,0,&DummyCamac,&Qcamac,&Xcamac); // donr know F=0?, return 0
    ///Camac Clear
    if(someReturn > 0)someReturn = CAMAC_C(uHandle);
    ///Camac Inhibit  off (like above):
    if(someReturn > 0)someReturn = CAMAC_I(uHandle, 0);
    ///Camac initialize:
    if(someReturn > 0)someReturn = CAMAC_Z(uHandle);
    
    if(someReturn <=0){
        std::cerr << " sth did not work " << std::endl;
        return 0;
    }
    std::cout << " \n Basic commands sent to Controler works \n " << std::endl;
    
    /// According to the instruction from Manual page 31
    /// Setting LAM mask and delays (table with functions on p23 in manual)
    /// Define LAM mask (to which modules Cotroler should respond):
    /// 0xFFFFFFFF - all of modules
    /// 0x00000001 - in 1st position
    /// 0x00000080 - in 7th position
    /// 0x0 - "when zero, the readout is triggered by a signal applied to the NIM input."
    /// page 20
    CAMAC_write(uHandle, 25, 9, 16, 0x0, &Qcamac,&Xcamac);
   
    someReturn = CAMAC_read_LAM_mask(uHandle,&m_LAMmask);
    std::cerr << "\n LAM mask: " << m_LAMmask <<std::endl;
    /// Define trigger delay
    /// ???????
    std::cerr << std::endl << "What about trigger delays ?  " << std::endl;
    someReturn = CAMAC_write(uHandle, 25, 2, 16, 0x64,&Qcamac,&Xcamac); //0x64 => 100  ; Set Delays
    
    long data;
    std::cout << "Delays: \n";
    int nbytes = CAMAC_register_read(uHandle, 0x2, &data);
    if(nbytes < 0) {
      std::cout << "Error reading delays.\n";
      return nbytes;
    }
    std::cout << data << " \n";
    
    
    
    ///Set buffer size to xxxx BuffOpt in Global Mode register:
    ///0x02 - 1024
    ///0x06 - 64
    long setBuffLength = 0x06;//0x07;
    someReturn = CAMAC_write(uHandle,25,1,16,setBuffLength,&Qcamac,&Xcamac);
    int buffSizes[8] = {4096, 2048, 1024, 512, 256, 128, 64, 1};
    std::cerr << "\n Buffer lenght: " <<buffSizes[int(setBuffLength)]<<std::endl;
    return 1;
}

int MyDaq :: PrepareAndLoadStack(){
    std::cerr << "\n     MyDaq :: PrepareAndLoadStack\n" << std::endl;
    ///list of stack commands:
    std::vector< long > stackCommands;
    ///read stack (max is defined in Common.h):
    long readStack[MAX_STACK_SIZE];
    
    ///writing stack commands:
    stackCommands.push_back(0);
    stackCommands.push_back(Q_STOP_BIT);
    ///Q_STOP_BIT: command is to be repeated as long as Q=1 (the response from CAMAC) but not more than the number specified in the following stack line
    ///now really adding the commands (look for description in GenericModule.cpp):
    for(auto& module : m_Modules){
        module->AddClearingReadToStack( stackCommands );
    }
    
    ///Finish of writing:
    stackCommands.push_back(STACK_END_MARKER);
    long numberOfStackCommands  = stackCommands.size()-1;
    stackCommands[0] = numberOfStackCommands;// stackCommands.at(0)=...
    std::cerr << "We have " << numberOfStackCommands << " commands in Stack \n" << std::endl;
    
    /// Loading all of the commands to Controler:
    short nBytesSent, nBytesRecieved;
    /// write commands to stack (device, type of the XXUSB stack:
    ///  2 - regular one,
    ///  3 - periodic (scaler) readout stack,
    ///  pointer to a variable array that contains (number of rows) the data to be stored in the target stack )
    nBytesSent = xxusb_stack_write(uHandle, 2, &(stackCommands[0]));
    
    /// verification, reading the stack
    nBytesRecieved = xxusb_stack_read(uHandle, 2, readStack);
    long nrReadStackCommands = readStack[0];
    if(numberOfStackCommands != nrReadStackCommands || (nBytesSent -2 -nBytesRecieved != 0)){
        std::cerr << "\n Failure in stack loading! \n " << std::endl;
        return 0;
    }
 
    if( STACK_VERBOSE){
        std::cerr << "Stack Size = " << numberOfStackCommands << std::endl;
        std::cerr << "Written to stack: " << std::endl;
        for(int iComm = 1; iComm < numberOfStackCommands; iComm++) std::cerr << iComm <<". command = "<< stackCommands[iComm] << std::endl;
        std::cerr << "What is read from stack: " << std::endl;
        for(int iComm = 1; iComm < numberOfStackCommands; iComm++) std::cerr << iComm <<". command = "<< readStack[iComm] << std::endl;
        std::cerr<< "\n END of verification of writing/reading to stack \n"<< std::endl;
        long readRegister;
       // int nBytes = xxusb_register_read(uHandle, 1, &readRegister);
       // if(nBytes < 0 ) std::cerr << "Error reading global mode "<< std::endl;
        
    }//end of STACK VERBOSE
    
    short someReturn;
    someReturn = xxusb_stack_execute(uHandle, &(stackCommands[0]));
    std::cerr<<"Stack executed \n" << std::endl;
    
    return 1; //if everything is ok :)
}

void MyDaq :: InitializeModulesOutput(){
    std::cerr << "\n     MyDaq :: InitializeModulesOutput\n" << std::endl;
    for( auto& module : m_Modules ) {
      module->AddOutput();
      m_nWordsExpectedPerEvent += (module->GetNchannels() + 1); //returns number of channels
    }
    // one for first word (nwords expected)
    // and one for negative one at end
    m_nWordsExpectedPerEvent += 2; // number of channels +2

    std::cerr << "Number of words expected per even is " << m_nWordsExpectedPerEvent << std::endl;
}


void MyDaq :: InitializeDAQ(){
    std::cerr << "\n     MyDaq :: InitializeDAQ\n" << std::endl;
    /// STARTING THE DAQ
    /// start acquisition 1 - the only register we haver, 0x1 - data to be stored,
    short someReturn = xxusb_register_write(uHandle, 1, 0x1);
    if(someReturn<0) std::cerr<<"\n Starting DAQ failed \n" << std::endl;
    else    std::cerr << "DAQ started " << std::endl;

    
}


int MyDaq :: TakeData() {
    
    std::cerr << "\n     MyDaq :: TakeData" << std::endl;
    short dataStream[100000];
    unsigned int nModules = m_Modules.size();
    int nbytes = 0;
   // TH1D *hist = new TH1D("adc ", "adc ; adc; counts", 1500, 0, 1500);
    while(!stopFlag){
        
        //device, pointer to store readout, Number of bytes to read, Time in milliseconds, after which the I/O operation is canceled, should there be no data available for the readout:
        nbytes = xxusb_bulk_read(uHandle, dataStream, 128, 200); //256
        
        
        if(nbytes<0){
           // std::cerr<<"Buffer is not full - need more entries, click faster :) "<<std::endl;
            sleep(6);
            continue;
            //return -1;
        }
        else{
            std::cerr << "Read nr bytes: " << nbytes << std::endl;
            short* dataPtr = dataStream; // pointer to the [0] element of read data, so number of events
            int nEvents = (dataStream[0] & 0xffff); // changing to 16 [M&E: ?]
            std::cerr << "nEvents " << dataStream[0] << std::endl;
            
            /// Verification if the number of read words in packet matches the expected number
            if((m_nWordsExpectedPerEvent*nEvents+2) != nbytes/2){
                std::cerr << "Bad packet read! Leaving (not yet, I need to check stuff) :( " << std::endl;
                return 0;
            }
            dataPtr++; // pointer to [1] element
            
            ///Good! We have possibly good packet.
            ///Let's go through the collected data:
            ///[0] - nr events
            ///[1] - nr of data in the event ( guess if it is 14, it is 12 channels + 2: Q_STOP_BIT  and -1 trailer)
            ///[2] Q_STOP_BIT - should be!
            ///Now check if events inside are ok
            ///Every event should have [1] element corresponding to nr of words
            ///Check if m_nWordsExpectedPerEvent -1 correspond what is in the [1] element. (-1 because the elemet [1] is the word itself )
            bool isBadEvent = false; ///positive assumption :)
            for( int iEv = 0; iEv < nEvents; iEv++){
                if(isBadEvent) break;
                if(*(dataPtr) == (m_nWordsExpectedPerEvent-1)
                   &&
                   *(dataPtr + m_nWordsExpectedPerEvent -1) == -1
                   &&
                   *(dataPtr+1) == Q_STOP_BIT){
                    /// this is good event! ///
                    std::cerr << iEv+1 << " is a good event " << std::endl;
                    dataPtr += 2; //skipping the Q_STOP_BIT move to first channel readout
                    for( unsigned int moduleNumber = 0; moduleNumber < nModules; moduleNumber++){
                        
                        m_Modules[moduleNumber] -> ReadData(dataPtr, histogram, tree, ch); //in GenericModule.cpp
                        std::cout<< m_Modules[moduleNumber] ->GetModuleName() << " sum: " << m_Modules[moduleNumber]->GetSum() << std::endl;
                        
                        dataPtr += m_Modules[moduleNumber]->GetNchannels();
                        dataPtr++;
                        
                    }//end of for moduleNumber loop
                }//end of if good event
                else{
                    std::cerr << iEv+1 << " is a BAD EVENT - leaving" << std::endl; //[E what is the bad event ?]
                    isBadEvent = true;
                }
            }//end for iEv loop
            c_adc->cd();
            histogram->Draw();
            c_adc->Update();
            std::cout<<"END of one set of measurements " << std::endl;
            
        }// end of else condition
    }//end of while loop
        /// leave DAQ mode
     xxusb_register_write(uHandle, 1, 0x0);
     /// CLEAN UP!!
     /// drain all data from fifo
     int ret=1;
     int loop = 0;
     while(ret > 0) {
         ret = xxusb_bulk_read(uHandle, dataStream, 128, 100); // [M&E: wtf?]
         std::cerr << " ret " << ret << std::endl;
         if(ret > 0){
             printf("drain loops: %i (result %i)\n", loop, ret);
             loop++;
             if (loop > 100) ret=0;
         }
     }
        xxusb_register_write(uHandle, 1, 0x0);
        return nbytes;
    
}



void MyDaq :: Finalize() {
  std::cerr << "\n     MyDaq :: Finalize \n" << std::endl;
    int someReturnF;
    someReturnF = CAMAC_C(uHandle);
    someReturnF = CAMAC_I(uHandle, false);
    
    outFile->Write();
    std::cerr << "\n     ~~ DONE :) ~~ \n" << std::endl;
}

int MyDaq :: TurnOff(){

    std::cout << "Press 0 to stop the threads..." << std::endl;
    while (true) {

        char input;
        std::cin >> input;
        if (input == '0') {
        // Set the stop flag to true, which will terminate both threads
            stopFlag = true;
            break;
        }
    }
    return 0;
}

void MyDaq::CreateOutputTree(){
 
    /// TTree making
    // current date/time based on current system
    time_t now = time(0);
    // convert now to string form
    char* dt = ctime(&now);

    outFile = outFile = TFile::Open("adc_output.root","RECREATE");
    tree = new TTree("T","adc_measurements");
    tree->Branch("ch1",&ch[0],"channel 1/I");
    tree->Branch("ch2",&ch[1],"channel 2/I");
    tree->Branch("ch3",&ch[2],"channel 3/I");
    tree->Branch("ch4",&ch[3],"channel 4/I");
    tree->Branch("ch5",&ch[4],"channel 5/I");
    tree->Branch("ch6",&ch[5],"channel 6/I");
    tree->Branch("ch7",&ch[6],"channel 7/I");
    tree->Branch("ch8",&ch[7],"channel 8/I");
    tree->Branch("ch9",&ch[8],"channel 9/I");
    tree->Branch("ch10",&ch[9],"channel 10/I");
    tree->Branch("ch11",&ch[10],"channel 11/I");
    tree->Branch("ch12",&ch[11],"channel 12/I");
}
