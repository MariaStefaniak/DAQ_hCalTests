/**
 * @file GenericModule.cpp
 *
 * @brief
 *
 * @author Maria Stefaniak
 */

#include "GenericModule.h"

#include <stdio.h>

#include <TTree.h>
#include <TFile.h>
#include <TH1.h>
#include <TSystem.h>
#include <TCanvas.h>

#include <iostream>

//--------------------------------
//            GenericModule
//--------------------------------

GenericModule :: GenericModule() {
  m_moduleName = "NONE"; 
  m_nChannels = 0;
  m_slotNumber = 0;
  m_readCommand = 0;
  m_adcsum = 0;
}

GenericModule :: GenericModule( std::string moduleName, int nChannels, int slotNumber, int readCommand, int nDigitsData ) {
    m_moduleName  = moduleName;
    m_nChannels   = nChannels;
    m_slotNumber  = slotNumber;
    m_readCommand = readCommand;
    m_nDigitsData = nDigitsData;
    m_adcsum = 0;
}

GenericModule :: ~GenericModule(){

}

//void GenericModule :: RegisterSharedData( SharedData* sd ) {

//}

/**
   Adds series of read commands to stack.
   These depend on module's slot number and read command.
   Stop bits (16) are added before and after the commands.
 */
void GenericModule :: AddClearingReadToStack( std::vector<long>& stack ){

    stack.push_back(Q_STOP_BIT);
    for(int iCh = 0; iCh < m_nChannels; iCh++)
        stack.push_back(CreateSimpleCommand(m_slotNumber, iCh, m_readCommand)); //reading commands for our modules
    stack.push_back(CreateSimpleCommand(m_slotNumber,0,9));/// N,0,9 (N,A,F) 9-Clear module and LAM; requires N, and any A from A(0) to A(11)
    stack.push_back(Q_STOP_BIT);
}

/**
   CAMAC Commands are constructed as follows.
   command = N*512 + A*32  + F
   equivalent to
   command = N*2^9 + A*2^5 + F
 */
long GenericModule :: CreateSimpleCommand( int N, int A, int F){
  return ( N<<9 ) + ( A<<5 ) + F;
}


void GenericModule :: AddOutput(){
  
}

void GenericModule :: ReadData(short* p, TH1D *histo, TTree *tree, int *channels){
    int readData;
    m_adcsum = 0;
    for(int iCh = 0; iCh < m_nChannels; iCh++){
        readData = *(p+iCh);
        std::cerr << iCh+1 << ". Channel read: "<< *(p+iCh) << std::endl;
        if(iCh ==3)histo->Fill(*(p+iCh));
        channels[iCh]=readData;
        tree->Fill();

        //sum are different
        if( readData < m_nDigitsData && !m_adcsum ){
            m_adcsum += readData;
          if (0) {
            std::cout<<m_adcsum<<std::endl;
          }
        }
    }
}

void GenericModule :: FillHistograms(int test){
}

void GenericModule :: Print() {
    std::cerr << m_moduleName << " with "
          << m_nChannels  << " channels in "
          << m_slotNumber << " slot number." << std::endl;
}

