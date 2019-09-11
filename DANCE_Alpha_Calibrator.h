//**************DANCE_Alpha_Calibrator.h*****************//

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
//                                                       //
//  Usage:  ./DANCE_Alpha_Calibrator(RNUM1,RNUM2,...)    //
//                                                       //
//        Christopher J. Prokop      12/19/2016          //
//*******************************************************//


#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TSystem.h"

using namespace std;

const int numberofdetectors=162;   //Number of detectors in DANCE


//Variables
int rnum; //Run Number

double Slope[numberofdetectors];
double Offset[numberofdetectors];

vector<int> failedfits;

vector<int> rnums;  //RunNumbers


double starting_slope[numberofdetectors];
double starting_offset[numberofdetectors];

//Files
TFile *fdatabase;  //database input file
TFile *fout[20];  //output files 

//Histograms
TH1D *hTemp;  //temporary container for histos
TH1D *hProj[numberofdetectors]; //1D calibrated template spectra to fit to
TH1D *hDet[numberofdetectors]; //1D uncalibrated spectrum for each detector

TH2D *hDatabase;

//Fits
TF1 *ftot[numberofdetectors]; //Fit for each detector



