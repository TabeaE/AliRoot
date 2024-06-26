void TOFMerger (Int_t evNumber1=0, Int_t evNumber2=0, Int_t ibg=0, Int_t bgr=10) 
{
// Dynamically link some shared libs

   if (gClassTable->GetID("AliRun") < 0) {
      gROOT->LoadMacro("loadlibs.C");
      loadlibs();
   }

// Connect the Root Galice file containing Geometry, Kine and Hits

   TFile *file = (TFile*)gROOT->GetListOfFiles()->FindObject("galice.root");
   if (file) file->Close(); 
   file = new TFile("galice.root","UPDATE");

// Get AliRun object from file or create it if not on file

   if (!gAlice) {
       gAlice = (AliRun*)file->Get("gAlice");
       if (gAlice) printf("AliRun object found on file\n");
       if (!gAlice) gAlice = new AliRun("gAlice","Alice test program");
   }
   AliTOF *TOF  = (AliTOF*) gAlice->GetModule("TOF");
  cout<<" TOFMerger:: TOF "<<TOF<<" gAlice "<<gAlice<<endl;

   if (TOF) {
// ctor invocation
       AliTOFMerger* merger = new AliTOFMerger();
// configuration
       if (ibg) {
	 merger->SetMode(ibg);
	 merger->SetBackgroundFileName("bg.root");
      }
       // pass signal filename
       //  merger->SetSignalFileName("galice.root");
       TOF->SetMerger(merger);
   }

//   Loop over events              
//
    for (int nev=evNumber1; nev<= evNumber2; nev++) {
	Int_t nparticles = gAlice->GetEvent(nev);
	cout << "nev         " << nev <<endl;
	cout << "nparticles  " << nparticles <<endl;
	if (nev < evNumber1) continue;
	if (nparticles <= 0) return;
	Int_t nbgr_ev = Int_t(nev*bgr/(evNumber2+1));
	cout<<" nbgr_ev "<<nbgr_ev<<endl;
	
	if (ibg) {
	    merger->SetBackgroundEventNumber(nbgr_ev);
	}

	gAlice->SDigits2Digits("TOF");

	char hname[30];
	snprintf(hname,30,"TreeD%d",nev);
	//	gAlice->TreeD()->Write(hname);
	//	cout<<hname<<" was written in file"<<file<<endl;
	//	gAlice->TreeD()->Print();
	//reset tree
	gAlice->TreeD()->Reset();

    }   // event loop 
}














