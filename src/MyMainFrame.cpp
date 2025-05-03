/**
 * @file MyMainFrame.cpp
 * Implementation for MyMainFrame class
 *
 * @brief
 *
 * @author Maria Stefaniak based on  Yakov Kulinich code
 * @version 1.0
 */

#include "MyMainFrame.h"
#include "MyDaq.h"

#include <stdio.h>

#include <TTree.h>
#include <TFile.h>
#include <TH1.h>
#include <TString.h>
#include <TSystem.h>
#include <TCanvas.h>
#include <TApplication.h>

#include <TGClient.h>
#include <TGButton.h>
#include <TGTextEntry.h>
#include <TRootEmbeddedCanvas.h>

#include <TTimer.h>

#include <iostream>
#include <unistd.h>
#include <limits.h>
#include <string>


MyMainFrame::MyMainFrame(const TGWindow *p, UInt_t w, UInt_t h, MyDaq* daq) :
  TGMainFrame(p, w, h), m_daq(daq) {
    
    // Create canvas widget
    m_fEcanvas = new TRootEmbeddedCanvas("Ecanvas", this, w/2, h);
    AddFrame(m_fEcanvas, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,
                         10,10,10,1));
      
      TCanvas *fCanvas = new TCanvas();
    // Create a horizontal frame widget with buttons
    m_hframe = new TGHorizontalFrame(this,200,40);
    m_startB = new TGTextButton(m_hframe,"&Start");
 //   m_startB->Connect("Clicked()","MyMainFrame",this,"DoStart()");
    m_hframe->AddFrame(m_startB, new TGLayoutHints( kLHintsLeft, 5, 5, 3, 4 ) );
      
    m_stopB = new TGTextButton( m_hframe,"&Stop");
   // m_stopB->Connect("Clicked()","MyMainFrame",this,"DoStop()");
    m_hframe->AddFrame(m_stopB, new TGLayoutHints( kLHintsRight, 5, 5, 3, 4 ) );
    
    AddFrame(m_hframe, new TGLayoutHints( kLHintsCenterX, 2, 2, 2, 2 ) );

      
    // Map all subwindows of main frame
    MapSubwindows();
      
    // Initialize the layout algorithm
    Resize(GetDefaultSize());

    // Map main frame
    MapWindow();
    int xx;
    std::cout<<"0-end: " << std::endl;
    std::cin>>xx;
    if(xx==0)DoStop();
 }

MyMainFrame::~MyMainFrame() {
    /// Clean up used widgets: frames, buttons, layout hints
    Cleanup();
}

void MyMainFrame::DoStart() {
    
}
void MyMainFrame::DoStop() {
    gApplication->Terminate(0);
}


