
////////////////////////////////////////////////////////////////////////
//                                                                    //
//   Software Name: DANCE Data Acquisition and Analysis Package       //
//     Subpackage: DANCE_Analysis                                     //
//   Identifying Number: C18105                                       // 
//                                                                    //
////////////////////////////////////////////////////////////////////////
//                                                                    //
//                                                                    //
// Copyright 2019.                                                    //
// Triad National Security, LLC. All rights reserved.                 //
//                                                                    //
//                                                                    //
//                                                                    //
// This program was produced under U.S. Government contract           //
// 89233218CNA000001 for Los Alamos National Laboratory               //
// (LANL), which is operated by Triad National Security, LLC          //
// for the U.S. Department of Energy/National Nuclear Security        //
// Administration. All rights in the program are reserved by          //
// Triad National Security, LLC, and the U.S. Department of           //
// Energy/National Nuclear Security Administration. The Government    //
// is granted for itself and others acting on its behalf a            //
// nonexclusive, paid-up, irrevocable worldwide license in this       //
// material to reproduce, prepare derivative works, distribute        //
// copies to the public, perform publicly and display publicly,       //
// and to permit others to do so.                                     //
//                                                                    //
// This is open source software; you can redistribute it and/or       //
// modify it under the terms of the GPLv2 License. If software        //
// is modified to produce derivative works, such modified             //
// software should be clearly marked, so as not to confuse it         //
// with the version available from LANL. Full text of the GPLv2       //
// License can be found in the License file of the repository         //
// (GPLv2.0_License.txt).                                             //
//                                                                    //
////////////////////////////////////////////////////////////////////////


//**************DANCE_Alpha_Calibrator.cpp***************//

//*******************************************************//
//    This is a code to use the database of template     //
//   alpha spectra to calibrate each detector for every  //
//  run and generate a new calibration file for that run // 
//                                                       //
//  FARE must have been run with Particle ID enabled and //
//   the run-by-run calibration can be performed or not. //
//  The calibrated alpha spectra are then read in and    // 
//  fit for each detector extracting a slope and offset. //
//   The resulting slope and offset modifies and creates //
//    a calibration for use with a specfied run          //
//  Currently treats all subruns in a run together       //
//                                                       //
//                                                       //
//  Usage:  ./DANCE_Alpha_Calibrator RNUM1 RNUM2 ...     //
//                                                       //
//        Christopher J. Prokop      12/19/2016          //
//        Cathleen E. Fry            04/03/2020          //
//*******************************************************//


#include "DANCE_Alpha_Calibrator.h"

//Define Fit Function
//par[0] is the slope
//par[1] is the offset
//par[2] is the scaling factor of the template
Double_t ftotal(Double_t *x, Double_t *par) {
  Double_t xx = x[0]*par[0]+par[1];
  Double_t value = par[2]*hTemp->Interpolate(xx);
  return value;
}

int main(int argc, char *argv[]) {
  
  
  //User Input.............................................//

  //Path to the parm_out_RUNNUMBER files
  string pathtofile = "/home/cfry/FARE/FARE_v10_ajc/Calibrations/";  //CURRENTLY NOT USED! THIS WOULD OVERWRITE CALIBRATIONS IN FARE

  //Prefix of Param File Name
  string filenameprefix = "/home/cfry/DANCE_Alpha_Calibrator/ParamOutput/param_out_";   

  //Suffix of Param File Name
  string filenamesuffix = ".txt"; 

  //Path to the root files
  string pathtorootfile = "/home/cfry/DANCE_Analysis/stage0_root_automated/";
  
  //Prefix of File Name
  string histofilenameprefix = "Stage0_Histograms_Run_";   

  //Suffix of File Name
  string histofilenamesuffix = "_500ns_CW_0ns_CBT_0ns_DEBT.root";   

  //Name of the TDirectory that the alpha spectra are stored in
  string alphafoldername = "Standard/AlphaSpectra";

  //Prefix of Raw uncalibrated Alpha Histogram Name
  string alphahistonameprefix = "hAlpha";

  string pathtorootoutput = "/home/cfry/DANCE_Alpha_Calibrator/RootOutput/";
    
  //Path to the "DANCE_Alpha_Database.root" file
  string pathtodatabase="./DANCE_Alpha_Database/";

  //Warn the user if the chisquare/NDF goes above this value
  Double_t chisquarewarning = 15; //3.5

  //Number of refit attempts allowed per detector if the chisquare is above chisquarewarning
  int refitattemptlimit=5; 

  int rebin_factor = 4;

  //End User Input.........................................//

  TCanvas *cFitResults = new TCanvas("FitResults","FitResults");
  cFitResults->Divide(18,9,0,0);
  TCanvas *cTemp = new TCanvas("TEmp","Temp");

  TH2D *hAlpha;
  TH1D *hChiSquare_NDF = new TH1D("hChiSquare_NDF","hChiSquare_NDF",162,0,162);
  
  //Get the run numbers 
  for(int i=1; i<argc; i++) {
    rnums.push_back(atoi(argv[i]));
    //cout << rnums.back() << " " << rnums.size() << endl;
  }

  //Read in the starting parameters
  
  ifstream start;
  start.open("last_params.dat"); 
  
  double junkvalue=0;
  for(int i=0; i<162; i++) {
    start>>junkvalue>>starting_offset[i]>>starting_slope[i]>>starting_quad[i]>>junkvalue>>junkvalue;
    starting_offset[i] /= 1000.0;
    starting_slope[i] /= 1000.0;
      //  cout<<i<<" offset: "<<starting_offset[i]<<"  slope: "<<starting_slope[i]<<endl;
  }
  
  bool failedfit=false;
  bool usingsubruns=false;
  int subrunnum=0;
  

  stringstream fname;
  fname.str();
  fname<<pathtorootfile.c_str();
  fname<<histofilenameprefix.c_str();
  fname<<rnums[0];
  fname<<"_0";
  fname<<histofilenamesuffix.c_str();
  char* alphahistname="ISlow_ID_alphaNoPU";
  TFile* fin;// = new TFile (fname.str().c_str());

  if (!gSystem->AccessPathName(fname.str().c_str())) {
    usingsubruns=true;
  }
  else {
    fname.str("");
    fname<<pathtorootfile.c_str();
    fname<<histofilenameprefix.c_str();
    fname<<rnums[0];
    fname<<histofilenamesuffix.c_str();
  }

  //Read in the files
  for(int i=0; i<(int)rnums.size(); i++) {
  
   if (usingsubruns){
    while (!gSystem->AccessPathName(fname.str().c_str())){

        cout<<"File: "<<fname.str().c_str()<<endl;
        //Open the file
        fin = new TFile(fname.str().c_str());
        //get the alpha 2D
        if(i==0 && subrunnum==0) {
          if(!fin->GetListOfKeys()->Contains(alphahistname)){
            alphahistname="hAlpha";
            if (!fin->GetListOfKeys()->Contains(alphahistname)){
              alphahistname="ISlow_ID_alpha";
            }
          }
          hAlpha = (TH2D*)fin->Get(alphahistname);
        }
        else {
          hAlpha->Add((TH2D*)fin->Get(alphahistname));
        }
        subrunnum++;
        fname.str("");
        fname<<pathtorootfile.c_str();
        fname<<histofilenameprefix.c_str();
        fname<<rnums[i];
        fname<<"_";
        fname<<subrunnum;  
        fname<<histofilenamesuffix.c_str();
      }
      subrunnum=0;
      fname.str("");
      fname<<pathtorootfile.c_str();
      fname<<histofilenameprefix.c_str();
      fname<<rnums[i+1];
      fname<<"_";
      fname<<subrunnum;  
      fname<<histofilenamesuffix.c_str();
    }
    else {
      fname.str("");
      fname<<pathtorootfile.c_str();
      fname<<histofilenameprefix.c_str();
      fname<<rnums[i];
      fname<<histofilenamesuffix.c_str();
      //Open the file
      fin = new TFile(fname.str().c_str());
      //get the alpha 2D
      if(i==0) {
        if(!fin->GetListOfKeys()->Contains(alphahistname)){
          alphahistname="hAlpha";;
          if (!fin->GetListOfKeys()->Contains(alphahistname)){
            alphahistname="ISlow_ID_alpha";
          }
        }
        hAlpha = (TH2D*)fin->Get(alphahistname);
      }
      else {
        hAlpha->Add((TH2D*)fin->Get(alphahistname));
      }
    }
  }
  //cout<<"Alpha 2D: "<<hAlpha<<endl;

  for(int jay=0; jay<numberofdetectors; jay++) {
    hDet[jay] = (TH1D*)hAlpha->ProjectionX(Form("Alpha_%d",jay),jay+1,jay+1);
    //cout<<jay<<"  "<<hDet[jay]->Integral()<<endl;
  }
  
  //Rebin them
  for(int j=0; j<numberofdetectors; j++) {
    hDet[j]->Rebin(rebin_factor);
  }

  //stringstream the database filename
  stringstream dbfname;
  dbfname.str();
  dbfname<<pathtodatabase.c_str();
  dbfname<<"DANCE_Alpha_Database.root";
  
  //Get the template file
  fdatabase = new TFile(dbfname.str().c_str());
  
  //Get the 2D database histogram
  hDatabase = (TH2D*)fdatabase->Get("hDanceAlphaDatabase");
  
  //Make the output files for each run inlcuded in the fit
  for(int i=0; i<(int)rnums.size(); i++) {
    fout[i] = new TFile(Form("%sCalibrations_Run_%d.root",pathtorootoutput.c_str(),rnums[i]),"RECREATE");
  }
  
  // cout<<"hD  "<<hDatabase<<endl;
  // cout<<"Starting Loop"<<endl;
  
  //Loop over number of detectors
  for(int j=0; j<numberofdetectors; j++) {

    cTemp->cd();

    failedfit=false;
    //  cout<<"j  "<<j<<endl;

    //Project the template 2D to 1Ds
    hProj[j] = (TH1D*)hDatabase->ProjectionX(Form("Template_Detector_%d",j),j+1,j+1); //bins start at 1 dets at 0
    hProj[j]->Rebin(rebin_factor);
    hProj[j]->GetXaxis()->SetRangeUser(0.25,4.75);  //set the range over the useful part of the alpha spectra
    // cout<<hProj[j]->GetEntries()<<endl;  //cout the number of entries in the template
    
    //  cout<<hProj[j]<<endl;
      
    /*
      The idea is now that we have the template and the uncalibrated spectrum for each detector
      we fit the template with the raw and the slope and offset needed to get the uncalibrated 
      spectrum to "match" the template is then used to adjust the calib_ideal file and make a 
      unique calibration for this specific run. 
    */

    if(hDet[j]->GetEntries() > 0) {

      //Autodetermine the fit range
      int maxheight = hDet[j]->GetBinContent(hDet[j]->GetMaximumBin());

      int fit_threshold = 0.1*maxheight;
      int lower_limit=0;
      int upper_limit=0;
    
      for(int k=10; k<hDet[j]->GetNbinsX(); k++) {
	if(hDet[j]->GetBinContent(k) > fit_threshold) {
	  lower_limit = hDet[j]->GetXaxis()->GetBinCenter(k-1);
	  break;
	}
      }

      for(int k=hDet[j]->GetNbinsX()-10; k>0; k--) {
	if(hDet[j]->GetBinContent(k) > fit_threshold) {
	  upper_limit = hDet[j]->GetXaxis()->GetBinCenter(k+1);
	  break;
	}
      }
    
      //  cout<<j<<"  "<<maxheight<<"  L : "<<lower_limit<<"  U : "<<upper_limit<<endl;

    

      ftot[j] = new TF1(Form("ftot_%d",j),ftotal,lower_limit,upper_limit,3);  //one fit function for each detector

      ftot[j]->SetLineColor(2);  //make it red
      ftot[j]->SetNpx(hProj[j]->GetNbinsX());  //make the number of points one-to-one with the bins for good graphics

      hDet[j]->GetXaxis()->SetRangeUser(100,20000);
      ftot[j]->SetParameter(0,(1.0*hProj[j]->GetMean()/(1.0*hDet[j]->GetMean())));   
      ftot[j]->SetParameter(0,starting_slope[j]);   
      //ftot[j]->SetParameter(1,starting_offset[j]);   

      ftot[j]->SetParLimits(1,-0.1,0.1);
      //cout << j << "\t" << starting_slope[j] << "\t" << 1.0*hProj[j]->GetMean()/(1.0*hDet[j]->GetMean()) << endl;
      ftot[j]->SetParLimits(0,0.00030,0.00042);//this one is for 2019 onward 
      //ftot[j]->SetParLimits(0,0.00021,0.00035); //comment the line above and go back here for 2018 data

      //ftot[j]->SetParameter(2,hProj[j]->Integral()/hDet[j]->Integral());
      //   ftot[j]->FixParameter(2,hProj[j]->Integral()/hDet[j]->Integral());
      // ftot[j]->FixParameter(2,hDet[j]->Integral()/hProj[j]->Integral());
       double ratio = hDet[j]->Integral()/hProj[j]->Integral();
       // cout<<"j: "<<j<<"  Det: "<<hDet[j]->Integral()<<"  Proj: "<<hProj[j]->Integral()<<" ratio: "<<ratio<<endl;
       ftot[j]->SetParameter(2,ratio/2.0);

      if(hTemp)  hTemp->Reset();  //reset the histogram
      hTemp = (TH1D*)hProj[j]->Clone();  //clone the template because the fit function didnt like arrays :/
      // hTemp->Rebin(rebin_factor);
      // cout<<hTemp->GetEntries()<<endl;
    
      //Fit a few times initially
      hDet[j]->Fit(Form("ftot_%d",j),"RQMN");
      hDet[j]->Fit(Form("ftot_%d",j),"RQMN");

      //Check to see if the chisquare/NDF is reasonable
      if((ftot[j]->GetChisquare()/(1.*ftot[j]->GetNDF()))>chisquarewarning) {
	//if not refit a few times
	for(int k=0; k<refitattemptlimit; k++) {
	  //	cout<<"Refitting Detector "<<j<<" Attempt "<<k+1<<endl;
	  hDet[j]->Fit(Form("ftot_%d",j),"RQMN");
	  //check to see if the fit is good now
	  if((ftot[j]->GetChisquare()/(1.*ftot[j]->GetNDF()))<chisquarewarning) {
	    break;
	  }
	  //if it keeps failing give up and add it to the failedfits vector
	  else {
	    if (k==(refitattemptlimit-1)) {
	      failedfits.push_back(j);
	      failedfit=true;
	    }
	  }
	}
      }

      hDet[j]->Fit(Form("ftot_%d",j),"RQM+");

      hChiSquare_NDF->Fill(j,(ftot[j]->GetChisquare()/(1.*ftot[j]->GetNDF())));
			   
      //Output fit results 
      //    cout<<"Detector: "<<j<<"  "<<ftot[j]->GetChisquare()/(1.*ftot[j]->GetNDF())<<"   Slope: "<<1000*ftot[j]->GetParameter(0)<<"   Offset: "<<1000*ftot[j]->GetParameter(1)<<endl;
    
      //Store results in output rootfiles
      for(int k=0; k<(int)rnums.size(); k++) {
	fout[k]->cd();
	hDet[j]->Write();
	hProj[j]->Write();
	ftot[j]->Write();
      }
    
      //And in arrays
      Slope[j] = ftot[j]->GetParameter(0);
      Offset[j] = ftot[j]->GetParameter(1);

      /*
	if(failedfit) {
	Slope[j] = starting_slope[j];
	Offset[j] = starting_offset[j];
	}
      */

    cFitResults->cd(j+1);
    hDet[j]->Draw();
    }

    else {
      if(j!=76 && j!=86) {
	failedfits.push_back(j);
	//            failedfit=true;
	Slope[j] = starting_slope[j];
	Offset[j] = starting_offset[j];
      } 
    }



  } 
  //  cout<<"Fitting concluded."<<endl;
  
  //Display number of fits with too large of chisquare/NDF
  //  cout<<"There were "<<failedfits.size()<<" failed fits"<<endl;
  
  //Bools for the interface
  bool makecal=true;
  bool ask=true;
  bool badresponse=false;
  bool careaboutfails=false;
 
  //Ask if having failed fits is acceptable
  if(failedfits.size()>0 && careaboutfails) {
    while(ask) {
      if(badresponse) {
	cout<<"That was not a valid response"<<endl;
	badresponse=false;
      }
      cout<<"Continue making the calibration? (y/n)"<<endl;
      string cont;
      cin>>cont;
      
      if(cont=="y") {
	ask=false;
	makecal=true;
      }
      else if(cont=="n") {
	ask=false;
	makecal=false;
      }
      else {
	badresponse=true;
      }
    }
  }
  
  //If you are satisified make the calibration for this run
  if(makecal) {    
    cout<<"Making calibration file"<<endl;
    
    for(int i=0; i<(int)rnums.size(); i++) {
      
      //Make the calibration file
      stringstream cfname;
      cfname.str();
      //   cfname<<pathtofile;
      cfname<<filenameprefix;
      cfname<<rnums[i];
      cfname<<filenamesuffix; 
      
      //open output file
      ofstream calibout;
      calibout.open(cfname.str().c_str());

      ofstream lastout;
      lastout.open("last_params.txt");

      ofstream ofs;
      ofs.open ("Calibration_Log.txt", std::ofstream::out | std::ofstream::app);
      
      //output the results
      for(int j=0; j<numberofdetectors; j++) {
	if(j==76 || j== 86) {
	  calibout<<j<<"  "<<0<<"  "<<0<<"  "<<0<<"  "<<0<<"  "<<0<<"\n";  //Missing detectors
	  lastout<<j<<"  "<<0<<"  "<<0<<"  "<<0<<"  "<<0<<"  "<<0<<"\n";  //Missing detectors
	  
	}
	else { 
	  //	  calibout<<j<<"  "<<1000.0*Offset[j]<<"  "<<1000.0*Slope[j]<<"  "<<0<<"  "<<0<<"  "<<12000.0*Slope[j]<<"\n";  //DANCE detectors
	  //The short PSD gate was shortened before this block of runs
	  if(rnums[i] > 100639) {
	    calibout<<j<<"  "<<1000.0*Offset[j]<<"  "<<1000.0*Slope[j]<<"  "<< starting_quad[j] <<"  "<<0<<"  "<<6000.0*Slope[j]<<"\n";  //DANCE detectors
	    lastout<<j<<"  "<<1000.0*Offset[j]<<"  "<<1000.0*Slope[j]<<"  "<<0<<"  "<<0<<"  "<<6000.0*Slope[j]<<"\n";  //DANCE detectors
	  }
	  //Consistent PSD gate for all of CAEN 2015 and 2018 that I know (or care) about
	  else {
	    calibout<<j<<"  "<<1000.0*Offset[j]<<"  "<<1000.0*Slope[j]<<"  "<<0<<"  "<<0<<"  "<<3500.0*Slope[j]<<"\n";  //DANCE detectors
	    lastout<<j<<"  "<<1000.0*Offset[j]<<"  "<<1000.0*Slope[j]<<"  "<<0<<"  "<<0<<"  "<<3500.0*Slope[j]<<"\n";  //DANCE detectors
	  } 
	}
      }
      for(int j=numberofdetectors; j<175; j++) {
	calibout<<j<<"  "<<0<<"  "<<0<<"  "<<0<<"  "<<0<<"  "<<0<<"\n";  //Not sure what these channels are.  This is just legacy
	lastout<<j<<"  "<<0<<"  "<<0<<"  "<<0<<"  "<<0<<"  "<<0<<"\n";  //Not sure what these channels are.  This is just legacy
      }
      
      cout<<"Calibration "<<rnums[i]<<" Complete"<<endl;
      
      
      cout<<"There were "<<failedfits.size()<<" failed fits"<<endl;
      
      ofs<<"Run: "<<rnums[i]<<"  "<<failedfits.size()<<" Failed Fits";
      if(failedfits.size()>0) {
	ofs<<" (Detectors:";
	for(int kay=0; kay<failedfits.size(); kay++) {
	  ofs<<" "<<failedfits[kay];
	}
	ofs<<")\n";
      }
      else {
	ofs<<"\n";
      }
      ofs.close();
      calibout.close();
      lastout.close();
    }
  }
  //If the calibration wasnt made then it failed
  else {
    cout<<"Calibration(s) Failed"<<endl;
  }
  
  for(int i=0; i<(int)rnums.size(); i++) {
    fout[i]->cd();
    cFitResults->Write();
    hChiSquare_NDF->Write();
  }

  //Write the output root file
  //  fout->Write();
}
