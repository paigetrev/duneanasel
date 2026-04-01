#include "duneanasel/nd/ndlar/Selections.h"
//#include "duneanasel/common/TruthTools.h"

#include "duneanaobj/StandardRecord/Proxy/SRProxy.h"
#include "duneanaobj/StandardRecord/StandardRecord.h"

#include "TChain.h"
#include "TFile.h"
#include "TH1D.h"

#include <memory>

int main(int argc, char const *argv[]) {

  TChain ch("cafTree");

  for(int i = 1; i < argc; ++i){
    ch.Add(argv[i]);
  }

  caf::StandardRecord *SR = nullptr;
  ch.SetBranchAddress("rec", &SR);

  Long64_t ents = ch.GetEntries();
  std::cout << "Input tree has " << ents << " entries." << std::endl;

  ch.GetEntry(0);

  TFile hout("New_Output_NDLAR_ShowerPlots.root", "RECREATE");

  TH1D *nshowers = new TH1D("nshowers", ";Number of Showers; Count", 20, 0, 20);
  TH1D *showerEnergies = new TH1D("showerEnergies", "; Photon Shower Energy [GeV]; Count", 100, 0, 5);
  
  for (Long64_t i = 0; i < ents; ++i) {
    ch.GetEntry(i);


    std::cout << "Event: " << i << " with " << SR->nd.lar.pandora.size()   << " pandora interactions." << std::endl;  
    
    int j = 0;

    for(auto const &nd_int : SR->nd.lar.pandora){
        
        /*Get the Outerface Values for Updated files
        for(auto const &p :nd_int.showers){
           std::cout << p.Evis << std::endl;
        }
    */
        int numberShowers = nd_int.showers.size();
        if(numberShowers > 0){
            nshowers->Fill(numberShowers);
        }
        else{
        continue;
        }
    }
    
    for (auto const &nd_int : SR->common.ixn.pandora) {
        int photonCounter = 0;
        int interactionID = nd_int.id;
        for(auto const &p : nd_int.part.pandora){
            if(p.pdg == 22){
                photonCounter += 1; 
            }
           
        }
        if(photonCounter > 1){
        std::cout << "Event: " << i << " Interaction: " << interactionID << " Contains: " << photonCounter << " Photons" << std::endl;
            for(auto const &p : nd_int.part.pandora){
            showerEnergies->Fill(p.E);
            }
        }
    }

    }
  
  hout.Write();
}
