#include <iostream>
#include <vector>
#include <cstdlib>
#include "TFile.h"
#include "TTree.h"
#include "TF1.h"
#include "TGraph.h"
#include "TH1D.h"

int main( int nargs, char** argv ) {

  std::string inputfile  = argv[1];
  std::string outputfile = argv[2];

  std::cout << "Input:  " << inputfile << std::endl;
  std::cout << "Output: " << outputfile << std::endl;

  int maxchs   = 900;
  float lowerbounds[3] = { 50, 30, 50}; // assuming everything below is noise
  float midbounds[3]   = { 70, 70, 70}; // between lower and midbounds, looking for dip before peak
  float upperbounds[3] = {400,400,400}; // upper bound
  int nbins = 100;
  float maxpeak = 500.0;

  TFile* fin  = new TFile(inputfile.c_str());
  TTree* tree = (TTree*)fin->Get("adc");
  int plane,wireid;
  float peak;
  tree->SetBranchAddress("planeid",&plane);
  tree->SetBranchAddress("wireid", &wireid);
  tree->SetBranchAddress("peak",   &peak);

  TFile* fout = new TFile(outputfile.c_str(),"recreate");
  
  TH1D** hists[3] = {NULL};
  for (int p=0; p<3; p++) {
    hists[p] = new TH1D*[maxchs];
    for ( int ch=0; ch<maxchs; ch++ ) {
      char histname[500];
      sprintf( histname, "hadc_p%d_ch%d", p, ch );
      char histtitle[500];
      sprintf( histtitle, "Plane %d, Channel %d", p, ch );
      hists[p][ch] = new TH1D(histname, histtitle, nbins, 0, maxpeak);
    }
  }

  size_t entry = 0;
  size_t numbytes = tree->GetEntry(entry);

  while ( numbytes>0 ) {
    if ( entry%100000==0 )
      std::cout << "Entry " << entry << " p=" << plane << " ch=" << wireid << " peak=" << peak << std::endl;
    hists[plane][wireid]->Fill( peak );
    entry++;
    numbytes = tree->GetEntry(entry);
  }

  for (int p=0; p<3; p++) {
    for ( int ch=0; ch<maxchs; ch++ ) {
      hists[p][ch]->Write();
    }
  }

  int nbadfits = 0;
  for ( int p=0;p<3;p++ ) {

    TGraph* gmean  = new TGraph( maxchs );
    TGraph* gsigma = new TGraph( maxchs );

    for ( int ch=0; ch<maxchs; ch++ ) {
      int b1 = hists[p][ch]->GetXaxis()->FindBin( lowerbounds[p] );
      int b2 = hists[p][ch]->GetXaxis()->FindBin( midbounds[p] );
      int b3 = hists[p][ch]->GetXaxis()->FindBin( upperbounds[p] );
      float integral = hists[p][ch]->Integral(b1,b3); // total integral
      float mean = 0;
      float rms  = 0;
      if ( integral>100 ) {

	// set range between low and mid bounds to find dip before peak
	hists[p][ch]->GetXaxis()->SetRange( b1, b2 );
	int dmaxbin1 = hists[p][ch]->GetMaximumBin();
	int dminbin1 = hists[p][ch]->GetMinimumBin();

	// set range between dip and upper bound to find peak
	hists[p][ch]->GetXaxis()->SetRange( dminbin1, b3 );
	int dmaxbin2 = hists[p][ch]->GetMaximumBin();
	int dminbin2 = hists[p][ch]->GetMinimumBin();

	// set initial parameters to fit
	mean = hists[p][ch]->GetXaxis()->GetBinCenter( dmaxbin2 );
	rms  = hists[p][ch]->GetRMS();
	  
	// restore range
	hists[p][ch]->GetXaxis()->SetRange(1,nbins);

	char fitname[500];
	sprintf( fitname, "fit_p%d_ch%d", p, ch );
	TF1* fit    = new TF1(fitname,"gaus");
	fit->SetParameter(0, hists[p][ch]->GetBinContent(dmaxbin2) );
	fit->SetParameter(1, mean );
	fit->SetParameter(2, rms );
	//hists[p][ch]->Fit( fit, "RQ", "", lowerbounds[p], upperbounds[p] );
	float fit1 = mean-100;
	float fit2 = mean+100;
	if (fit1<hists[p][ch]->GetXaxis()->GetBinLowEdge(dminbin1) ) fit1 = hists[p][ch]->GetXaxis()->GetBinLowEdge(dminbin1);
	if (fit2>upperbounds[p]) fit2 = upperbounds[p];

	hists[p][ch]->Fit( fit, "RQ", "", fit1, fit2 );
	mean = fit->GetParameter(1);
	rms  = fit->GetParameter(2);
	if ( mean<0 ) {
	  std::cout << "Bad fit plane=" << p << " ch=" << ch << ": " << mean << std::endl;
	  mean = 0;
	  // hists[p][ch]->GetXaxis()->SetRange( lowerbounds[p], upperbounds[p] );
	  // mean = hists[p][ch]->GetMean();
	  // rms  = hists[p][ch]->GetRMS();
	  // fit->SetParameter(0, hists[p][ch]->GetMaximum() );
	  // fit->SetParameter(1, mean );
	  // fit->SetParameter(2, rms );
	  // hists[p][ch]->GetXaxis()->SetRange( 1, 500 );
	}
	else {
	  std::cout << "Fit plane=" << p << " ch=" << ch << " mean=" << mean << std::endl;
	  // really on arithmetic mean instead of poor fits
	  // if ( (p!=1 && mean<150) || (p==1 && mean<80) ) {
	  //   if ( p!=1 )
	  //     hists[p][ch]->GetXaxis()->SetRange( hists[p][ch]->GetXaxis()->FindBin(50), hists[p][ch]->GetXaxis()->FindBin(150) );
	  //   else
	  //     hists[p][ch]->GetXaxis()->SetRange( hists[p][ch]->GetXaxis()->FindBin(10), hists[p][ch]->GetXaxis()->FindBin(80) );
	  //   mean = hists[p][ch]->GetMean();
	  // }
	  // else {
	  //   if ( p!=1 ) {
	  //     hists[p][ch]->GetXaxis()->SetRange(hists[p][ch]->GetXaxis()->FindBin(100), hists[p][ch]->GetXaxis()->FindBin(300) );
	  //     mean = hists[p][ch]->GetMean();
	  //   }
	  //   else {
	  //     hists[p][ch]->GetXaxis()->SetRange(hists[p][ch]->GetXaxis()->FindBin(50), hists[p][ch]->GetXaxis()->FindBin(250) );
	  //     mean = hists[p][ch]->GetMean();
	  //   }
	  // }
	}
	//fit->Write(fitname);
	delete fit;
	hists[p][ch]->GetXaxis()->SetRange( 1, 500 );
      }
      gmean->SetPoint(ch,ch,mean);
      gsigma->SetPoint(ch,ch,rms);
    }
    char meanname[100];
    sprintf( meanname, "gmean_plane%d", p );
    char sigmaname[100];
    sprintf( sigmaname, "gsigma_plane%d", p );
    gmean->Write( meanname );
    gsigma->Write( sigmaname );
  }
  
  std::cout << "Fin." << std::endl;
}
