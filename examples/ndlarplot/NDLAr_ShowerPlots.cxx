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


  for(Long64_t i = 0; i < ents; ++i) {
      ch.GetEntry(i);
      std::cout << "****************************************************************** " << std ::endl;
      std::cout << "* Event: " << i << " with " << SR->common.ixn.pandora.size() << " reco interactions and " << SR->mc.nu.size()   << " true interactions. *" << std::endl;   
      std::cout << " ***************************************************************** " << std ::endl;

    // Loop over Reco interactions to begin truth matching
      std::cout << "ALL TRUE INTRACTIONS:" << std::endl;

      for(auto const &nd_true_int : SR->mc.nu){
            std::cout << "----New True Interaction ----" << std::endl;
            std::cout << "Primaries: " << std::endl;
            int n = 0;
            for(auto const &primary : nd_true_int.prim){
                      std::cout << "n (primary index) = " << n << " G4ID= " << primary.G4ID << " PDG = " << primary.pdg << " Energy = " << primary.p.T() << std::endl;
                      n++;   
                  }


           
      
      
      }
      int counter = 0;
      for(auto const &nd_reco_int : SR->common.ixn.pandora){
          counter++;
          std::cout << "------- Reco Interaction Number " << counter << "---------" << std::endl;

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

          auto max_int_it = std::max_element(nd_reco_int.truthOverlap.begin(), nd_reco_int.truthOverlap.end());
          int max_int_idx = std::distance(nd_reco_int.truthOverlap.begin(), max_int_it);

          
          int trueInteractionIndex = nd_reco_int.truth[max_int_idx];
          float truthOverlap = nd_reco_int.truthOverlap[max_int_idx];

        
          
        
          if(trueInteractionIndex == -1){
              continue;
          }
          
          
          
          for(auto const &recPart : nd_reco_int.part.pandora){
              const bool isPrimary = recPart.primary;
              const float particleE = recPart.E;
              
          
          }
         
          int recoParticleCounter = 0;
          for(auto const &recPart : nd_reco_int.part.pandora){                        
              int trueIntIndexSize = recPart.truth.size();

              auto max_part_it = std::max_element(recPart.truthOverlap.begin(), recPart.truthOverlap.end());
              int max_part_idx = std::distance(recPart.truthOverlap.begin(), max_part_it);
  
              int trueIntIndex = recPart.truth[max_part_idx].ixn; 
              int truePartIndex = recPart.truth[max_part_idx].part;
              int trueType = recPart.truth[max_part_idx].type; 
              std:: cout << "truePartIndex:" << truePartIndex << " trueIntIndex: " << trueIntIndex <<" truePartType: " << trueType << std::endl;

              recoParticleCounter ++;
              if(trueType != 1){
                  std::cout << "Particle is not primary. It is type: " << trueType << "Skipping..." << std::endl; 
                  continue;
             }
              if(trueIntIndex >= SR->mc.nu.size()){
                  continue;
              }
              
              if(truePartIndex >= SR->mc.nu[trueIntIndex].prim.size()){
                  std::cout << "Truth Particle Index is out of range: " << truePartIndex << std::endl;
                  std::cout <<"trueIntIndex: [" << trueIntIndex << "]" << " truePartIndex: [" << truePartIndex << "] " << "interaction size: " << SR->mc.nu[trueIntIndex].prim.size() << std::endl;
                  int n = 0;
                  for(auto const &truePart : SR-> mc.nu[trueIntIndex].prim){
                      std::cout << "n (primary index) = " << n << " G4ID= " << truePart.G4ID << " PDG = " << truePart.pdg << "Energy = " << truePart.p.T() << std::endl;
                      n++;   
                  }

         
                  continue;
              }
              
             
            
        }
  
      }

    }
}

