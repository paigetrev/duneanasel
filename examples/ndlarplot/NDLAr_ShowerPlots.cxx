#include "duneanasel/nd/ndlar/Selections.h"
#include "duneanaobj/StandardRecord/SREnums.h"
#include "duneanaobj/StandardRecord/Proxy/SRProxy.h"
#include "duneanaobj/StandardRecord/StandardRecord.h"
#include "duneanaobj/StandardRecord/SRTruthBranch.h"

#include "TChain.h"
#include "TFile.h"
#include "TH1D.h"

#include <memory>
#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>

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

  TH1D *containsPi0 = new TH1D("containsPi0", "; Contains Pi0; Count", 13, 0, 13);
  TH1D *energyPi0 = new TH1D("energyPi0", "; Energy (GeV); Count", 50, 0, 10 );
  for(Long64_t i = 0; i < ents; ++i) {
      ch.GetEntry(i);
      std::cout << "****************************************************************** " << std ::endl;
      std::cout << "* Event: " << i << " with " << SR->common.ixn.pandora.size() << " reco interactions and " << SR->mc.nu.size()   << " true interactions. *" << std::endl;   
      std::cout << " ***************************************************************** " << std ::endl;

    // Loop over Reco interactions to begin truth matching

      for(auto const &nd_reco_int : SR->common.ixn.pandora){
          //std::cout << "Next Interaction..." << std::endl;
          //std::cout << "###### RECO INTERACTION NUMBER " << nd_reco_int.id << " ######" << std::endl;
          std::cout << "------- Interaction Info -------" << std::endl;

          if(!sel::beam::ndlar::InFV(nd_reco_int)){
              std::cout << "InFV: False" << std::endl;
          }
          else{
              std::cout << "InFV: True" << std::endl;
          }

          if(!sel::beam::ndlar::AllPrimaryParticlesContained(nd_reco_int)){
              std::cout << "All Primary Particles Contained: False" << std::endl;
          }
          else{
              std::cout << "All Primary Particles Contained: True" << std::endl;
          }
           
          int showersCounter = 0;
          int truthIndexSize = nd_reco_int.truthOverlap.size();
          int max_int_idx = 0;

          if(truthIndexSize > 1){
              auto max_int_it = std::max_element(nd_reco_int.truthOverlap.begin(), nd_reco_int.truthOverlap.end());
              int max_int_idx = std::distance(nd_reco_int.truthOverlap.begin(), max_int_it);
          }

          
          int trueInteractionIndex = nd_reco_int.truth[max_int_idx];
          float truthOverlap = nd_reco_int.truthOverlap[max_int_idx];

        
          if(trueInteractionIndex < SR->mc.nu.size()){
              std::cout << "True Neutrino Energy: " <<  SR->mc.nu[trueInteractionIndex].E << " GeV." << " True Interaction Number: " << trueInteractionIndex << std::endl;  
              std::cout << "Reco Neutrino Energy: " << nd_reco_int.Enu.calo << " GeV." << " Reco Interaction Number: " << nd_reco_int.id << std::endl;
          }
          
          std::cout << " Truth Overlap: " << truthOverlap << std::endl;
          std::cout << "------- True Interaction Contains the Following Particles: ------- " << std::endl;
        
          if(trueInteractionIndex == -1){
              std::cout << "True Interaction Index was not matched. " << std::endl;
              continue;
          }

          for(auto const &truePart : SR->mc.nu[trueInteractionIndex].prim){
              std::cout << "True PDG: " << truePart.pdg << " With Energy: " << truePart.p.T() << " G4ID: " << truePart.G4ID << std::endl;
          }

          
          std::cout << "------- Reco Pandora Particles has: " << nd_reco_int.part.pandora.size() << " entries. -------" << std::endl;
          std::cout << "------- Reco Interaction Contains the Following Particles: ------- " << std::endl;   
        
          int recoParticleCounter = 0;
          for(auto const &recPart : nd_reco_int.part.pandora){                        
              int trueIntIndexSize = recPart.truth.size();
              int max_part_idx = 0;
              if(trueIntIndexSize > 1){
                  auto max_part_it = std::max_element(recPart.truthOverlap.begin(), recPart.truthOverlap.end());
                  int max_part_idx = std::distance(recPart.truthOverlap.begin(), max_part_it);
              }
  
              int trueIntIndex = recPart.truth[max_part_idx].ixn; 
              int truePartIndex = recPart.truth[max_part_idx].part;
              int trueType = recPart.truth[max_part_idx].type; 
             
              std::cout << "Reco Particle: " << recoParticleCounter << std::endl;
              recoParticleCounter ++;
              if(trueIntIndex > SR->mc.nu.size()){
                  std::cout << "Truth Interaction Index is out of range:  " << trueIntIndex << std::endl;
                  continue;
              }
              if(truePartIndex > SR->mc.nu[trueIntIndex].prim.size()){
                  std::cout << "Truth Particle Index is out of range: " << truePartIndex << std::endl;
                  continue;
              }
              
              std::cout << "SRRecoParticle PDG: " << recPart.pdg << " With Energy: " << recPart.E << " GeV" << " Type: " << trueType << std::endl;
              std::cout << "Matched SRTrueParticle PDG: " << SR->mc.nu[trueIntIndex].prim[truePartIndex].pdg << " With Energy: " << SR->mc.nu[trueIntIndex].prim[truePartIndex].p.T() << std::endl;
             
            
        }
  
      }

    }
  hout.Write();  
}

