#include <iostream>
#include <fstream>


void ParamPlotter() {

  
  //Global exlcudes
  std::vector<int> Exclude_Run;
  std::vector<int> Exclude_Reason;
  int exc_run;
  int exc_reason;

  //Read in the exlcuded runs file
  ifstream exclude;
  exclude.open("exclude.txt");
  
  if(exclude.is_open()){ 
    while(true) {
      exclude >> exc_run >> exc_reason;
      if(exc_run < 0 || exc_reason < 0) {
	break;
      }
      Exclude_Run.push_back(exc_run);
      Exclude_Reason.push_back(exc_reason);
    }
    
    cout<<"There are a total of "<<Exclude_Run.size()<<" Runs in all of the data to exclude from adding"<<endl;
  }


  const int nruns=322;
  int starting_run =104551;

  TH2D *hSlope = new TH2D("hSlope","hSlope",nruns,starting_run,starting_run+nruns,162,0,162);
  TH2D *hOffset = new TH2D("hOffset","hOffset",nruns,starting_run,starting_run+nruns,162,0,162);
  
  double offsets[162][nruns];
  double slopes[162][nruns];
  
  for(int eye=starting_run; eye<starting_run+nruns; eye++) {
  
    bool exclude_this = false;
    for(int jay=0; jay<Exclude_Run.size(); jay++) {
      if(eye == Exclude_Run[jay]) {
	exclude_this=true;
	cout<<"Excluding Run: "<<eye<<endl;
      }
    }

    if(exclude_this) {
      for(int jay=0; jay<162; jay++) {
	hSlope->Fill(eye,jay,0);
	hOffset->Fill(eye,jay,0);
      }
    }
    else {
      stringstream fname;
      fname.str();
      
      fname<<"param_out_";
      fname<<eye<<".txt";
      
      ifstream file;
      file.open(fname.str().c_str());
      
      double doublecrap;
      int detnum;
      double slope;
      double offset;
      
      for(int jay=0; jay<162; jay++) {
	file>>detnum>>offset>>slope>>doublecrap>>doublecrap>>doublecrap;
	hSlope->Fill(eye,detnum,slope);
	hOffset->Fill(eye,detnum,offset);
      }
    }
  }

  
  TCanvas *cOffset = new TCanvas("Offset","Offset");
  hOffset->Draw("colz");
  
  TCanvas *cSlope = new TCanvas("Slope","Slope");
  hSlope->Draw("colz");
  

}
