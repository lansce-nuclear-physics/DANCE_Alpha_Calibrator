//***********DANCE_Alpha_Database_Creator.C**************//

//*******************************************************//
//    This is a very simple code to make a database of   //
// template alpha spectra for runs which a trusted gamma //
//     calibration has been done using 88Y or 22Na       //  
//                                                       //
//  FARE must have been run with Particle ID enabled and //
//   the run-by-run calibration performed from gammas.   //
//  The calibrated alpha spectra are then read in and    // 
//                summed for each detector.              //
//                                                       //
//        Christopher J. Prokop      12/15/2016          //
//*******************************************************//

#include <iostream>
#include <fstream>
#include <sstream>

//User Input.............................................//

//Number of runs to include
const int nruns = 3;

//These are the runs that will be included in the database
//Int_t run_numbers[nruns] = {38108 , 38109 , 38110 };  
//Int_t run_numbers[nruns] = {41985 , 41986 , 41987 , 41988 , 41989};  
Int_t run_numbers[nruns] = {59078, 59079, 59080};  

//Path to the root files
string pathtorootfile = "/home/cprokop/CJP/DANCE_Analysis/stage1_root/";

//Prefix of File Name
string histofilenameprefix = "Stage1_Histograms_Run_";

string histofilenamesuffix = "_10ns_CW_2000ns_CBT_0ns_DEBT.root"

//End User Input.........................................//


void DANCE_Alpha_Database_Creator() {

  //Template is stored in a 2D Histogram
  TH2D *hDanceAlphaDatabase;
  
  //Gamma Matrix is also stored in a 2D Histogram to see how good the calibration is
  TH2D *hDanceGammaDatabase;
  
  //Loop over the nruns
  for(int i=0; i<nruns; i++) {
    
    //stringstream the root filename
    stringstream fname;
    fname.str();
    fname<<pathtorootfile.c_str();
    fname<<histofilenameprefix.c_str();
    fname<<run_numbers[i];
    fname<<".root";
    
    cout<<"Processing: "<<fname.str()<<endl;

    //Open the file
    TFile *fin = new TFile(fname.str().c_str());
    
    if(i==0) {
      hDanceAlphaDatabase = (TH2D*)fin->Get("hAlphaCalib");
      hDanceGammaDatabase = (TH2D*)fin->Get("hGammaCalib");
    }
    else {
      hDanceAlphaDatabase->Add((TH2D*)fin->Get("hAlphaCalib"));
      hDanceGammaDatabase->Add((TH2D*)fin->Get("hGammaCalib"));
    }
    
    //Progress Statement
    cout<<"Completed Run: "<<run_numbers[i]<<endl;
  }
  
  //Make output root file
  TFile *fout = new TFile("DANCE_Alpha_Database.root","RECREATE");

  //Write 2D histos
  hDanceAlphaDatabase->Write();
  hDanceGammaDatabase->Write();
  
  //Write and Close
  fout->Write();
  fout->Close();

}
