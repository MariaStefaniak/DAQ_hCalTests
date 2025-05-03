#ifndef MYDAQ_H
#define MYDAQ_H

#include "Common.h"

#include <libxxusb.h>
#include <thread>
#include <atomic>

#include <TString.h>
#include <TCanvas.h>

/**
   @brief MyDaq is the main class for the DAQ system.
   It can be though of as the CAMAC crate and controller.
   From here, we do the control of the whole system.
   This class is instanciated by the user, and modules are added
   by the user. The user can then Run the DAQ with the rest of
   the interface hidden from them.
*/
class MyDaq{

  friend class MyMainFrame;

 public:
  /**
     @brief Default constructor for MyDaq.
   */
  MyDaq();

  /**
     @brief Default destructor for MyDaq.
   */
  ~MyDaq();

 public:
  /**
     @brief Function to add modules to MyDaq

     @param param1 Pointer to module being added
   */
  void AddModule( Module* );

  /**
     @brief Main function in MyDaq. Called by user.
  */
  int  Run();

 private:
  /**
     @brief Function to print info about added modules
  */
  void PrintModules();

  /**
     @brief Function to initialize CAMAC
  */
  int InitializeCAMAC();

  /**
     @brief Function to Prepare and load stack into DAQ
  */
  int PrepareAndLoadStack();

  /**
     @brief Function to Initialize output data for modules
  */
  void InitializeModulesOutput();

  /**
     @brief Function to initialize DAQ
  */
  void InitializeDAQ();

  /**
     @brief Function to take data
  */
  int TakeData();

  /**
     @brief Function processes data bank into ROOT
  */
  void ProcessDataIntoROOT();

  /**
     @brief Function to finalize data taking
  */
  void Finalize();

  int   TurnOff();
  void  CreateOutputTree();
    
 private:
    xxusb_device_type devices[100];
    struct usb_device *dev;      // Device
    usb_dev_handle *uHandle;     // Device Handle
    long m_firmwareId;
    long m_LAMmask;
    
    /// collection of modules in DAQ:
    std::vector<Module*> m_Modules;
    long m_nWordsExpectedPerEvent;
   
    // Application Stuff
    TApplication* m_Appliaction;
    MyMainFrame*  m_myMainFrame;
    
    // thread things
    std::thread* m_t_app;
    std::thread* m_t_daq;
    std::atomic<bool> stopFlag;
    
 public:
    TH1D  *histogram;
    TFile *outFile;
    TTree *tree;
    int ch[12];
    TCanvas *c_adc;

};


#endif
