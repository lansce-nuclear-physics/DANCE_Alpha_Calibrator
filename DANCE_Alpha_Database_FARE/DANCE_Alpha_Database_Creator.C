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
string pathtorootfile = "/home/cprokop/FARE/FARE_v10_CP_57Fe/RootOutput/Fe57/";

//Prefix of File Name
string histofilenameprefix = "Fe57_Histos";   

//Name of the TDirectory that the alpha spectra are stored in
string alphafoldername = "Standard/AlphaSpectra";

//Name of the TDirectory that the gamma spectra are stored in
string gammafoldername = "Standard/Crystals";

//Prefix of Alpha Histogram Name
string alphahistonameprefix = "Alpha_calib_";

//Prefix of Gamma Histogram Name
string gammahistonameprefix = "Ecrystal_";    

//Number of detectors in DANCE
Int_t numberofdetectors=162; 

  
//End User Input.........................................//


void DANCE_Alpha_Database_Creator() {

  //Temporary 1D histogram
  TH1D *htemp;
  
  //Template is stored in a 2D Histogram
  TH2D *hDanceAlphaDatabase = new TH2D("hDanceAlphaDatabase","hDanceAlphaDatabase",500,0,5,numberofdetectors,0,numberofdetectors);
  
  //Gamma Matrix is also stored in a 2D Histogram to see how good the calibration is
  TH2D *hDanceGammaDatabase = new TH2D("hDanceGammaDatabase","hDanceGammaDatabase",1000,0,10,numberofdetectors,0,numberofdetectors);
  
  //Loop over the nruns
  for(int i=0; i<nruns; i++) {
    
    //stringstream the root filename
    stringstream fname;
    fname.str();
    fname<<pathtorootfile.c_str();
    fname<<histofilenameprefix.c_str();
    if(run_numbers[i]<100000) {
      fname<<"0";
    }
    fname<<run_numbers[i];
    fname<<".root";
    
    cout<<"Processing: "<<fname.str()<<endl;

    //Open the file
    TFile *fin = new TFile(fname.str().c_str());
    
    //Get the TDirectories
    TDirectory *alphadir = (TDirectory *)fin->Get(alphafoldername.c_str());
    TDirectory *gammadir = (TDirectory *)fin->Get(gammafoldername.c_str());
    
    for(int j=0; j<numberofdetectors; j++) {
      
      //ALPHAS
      //Make the alpha histogram name stringstream
      stringstream histname;
      histname.str();
      histname<<alphahistonameprefix.c_str();
      if(j<10) {
	histname<<"00"<<j;
      }
      else if(j<100) {
	histname<<"0"<<j;
      }
      else {
	histname<<j;
      }
      
      //Get the histogram
      htemp = (TH1D*)alphadir->FindObjectAny(histname.str().c_str());

      //Fill the 2D histogram with the information from htemp 
      for(int k=1; k<htemp->GetNbinsX()+1; k++) {
	hDanceAlphaDatabase->Fill(htemp->GetXaxis()->GetBinCenter(k),j,htemp->GetBinContent(k)/(1.0*nruns));
	
      }  //End loop over number of bins
      
      //GAMMAS
      //Make the gamma histogram name stringstream
      stringstream histname;
      histname.str();
      histname<<gammahistonameprefix.c_str();
      if(j<10) {
	histname<<"00"<<j;
      }
      else if(j<100) {
	histname<<"0"<<j;
      }
      else {
	histname<<j;
      }
      
      //Get the histogram
      htemp = (TH1D*)gammadir->FindObjectAny(histname.str().c_str());
      
      //Fill the 2D histogram with the information from htemp 
      for(int k=1; k<htemp->GetNbinsX()+1; k++) {
	hDanceGammaDatabase->Fill(htemp->GetXaxis()->GetBinCenter(k),j,htemp->GetBinContent(k));
	
      }  //End loop over number of bins
      
    } //End loop over number of detectors
    
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
