#ifndef MYMAINFRAME_H
#define MYMAINFRAME_H

#include "Common.h"
#include <TString.h>
#include <TGFrame.h>
#include <string>

class TGHorizontalFrame;
class TGTextButton;
class TGTextButton;
class TGTextEntry;

class TRootEmbeddedCanvas;
class TTimer;

/**
   @brief This class is made to control the program from a GUI.
   This way, the user can start and stop the run at any time with just
   a click of the mouse. Taken from online.
 */
class MyMainFrame : public TGMainFrame {
 private:
  TRootEmbeddedCanvas *m_fEcanvas;
  TTimer* fTimer;

  TGHorizontalFrame* m_hframe;
  TGTextButton* m_startB;
  TGTextButton* m_stopB;
    
  MyDaq* m_daq;


 public:
  /**
     @brief Constructor for MyMainFrame
   */
    MyMainFrame(const TGWindow*, UInt_t , UInt_t, MyDaq* daq);

   // MyMainFrame(){}
  /**
     @brief Default destructor for MyMainFrame
   */
  virtual ~MyMainFrame();
    /**
       @brief Function to start Run
     */
  void   DoStart();

    /**
       @brief Function to stop Run
     */
  void   DoStop();

};

#endif
