#ifndef MODULE_H
#define MODULE_H

#include "Common.h"

/**
   @brief Module class. Pure virtual class;
*/
class Module{

 public:
  /**
     @brief Default constructor for Module.
   */
  Module() {};

  /**
     @brief Overloaded constructor for Module.

     @param param1 Module name
     @param param2 Number of channels in this module
     @param param3 Slot number of this module in CAMAC
     @param param4 Command (F) used for reading
     @param param5 Number of digits for data
  */
  Module( std::string, int, int, int, int) {};

  /**
     @brief Default destructor for Module
   */
  virtual ~Module() {};

  /**
     @brief Function to connect to MyDaq's SharedData

     @param param1 Pointer to shared data
  */
  //virtual void RegisterSharedData( SharedData* ) = 0;

  /**
     @brief Function adds reading with clear to stack.

     @param param1 Reference to stack being built
  */
  virtual void AddClearingReadToStack( std::vector<long>& ) = 0;

  /**
     @brief Function creates simple CAMAC (N,A,F) command.

     @param param1 N value
     @param param2 A value
     @param param3 F value
     @return long representing CAMAC command
  */
  virtual long CreateSimpleCommand( int, int, int ) = 0;

  /**
     @brief Function adds output for given module
  */
  virtual void AddOutput () = 0;

  /**
     @brief Function reads data from module

     @param param1 Address where module's data beings
  */
  virtual void ReadData( short*, TH1D *histo, TTree *tree, int *channels) = 0;

  /**
     @brief Function to fill histograms for module
   */
  virtual void FillHistograms(int test =0) = 0;

  /**
     @brief Function prints info about module
  */
  virtual void Print() = 0;

  /**
     @brief Accessor for number of channels
  */
    int GetNchannels(){ return m_nChannels; }

    short GetSum(){return m_adcsum;}
    std::string GetModuleName(){return m_moduleName;}

 protected:
    std::string m_moduleName;

    int m_nChannels;
    int m_slotNumber;
    int m_readCommand;

    int m_nDigitsData;
    short m_adcsum;
};

#endif
