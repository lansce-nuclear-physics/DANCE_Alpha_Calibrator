
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



