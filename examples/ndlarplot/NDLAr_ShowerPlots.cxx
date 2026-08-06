#include "duneanasel/nd/ndlar/Selections.h"
#include "duneanaobj/StandardRecord/SREnums.h"
#include "duneanaobj/StandardRecord/Proxy/SRProxy.h"
#include "duneanaobj/StandardRecord/StandardRecord.h"
#include "duneanaobj/StandardRecord/SRTruthBranch.h"

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

  TH1D *containsPi0 = new TH1D("containsPi0", "; Contains Pi0; Count", 13, 0, 13);
  TH1D *energyPi0 = new TH1D("energyPi0", "; Energy (GeV); Count", 50, 0, 10 );
  for(Long64_t i = 0; i < ents; ++i) {
      ch.GetEntry(i);
      std::cout << "****************************************************************** " << std ::endl;
      std::cout << "* Event: " << i << " with " << SR->common.ixn.pandora.size() << " reco interactions and " << SR->mc.nu.size()   << " true interactions. *" << std::endl;   
      std::cout << " ***************************************************************** " << std ::endl;

    // Loop over Reco interactions to begin truth matching

      for(auto const &nd_reco_int : SR->common.ixn.pandora){
        
          if(!sel::beam::ndlar::InFV(nd_reco_int)){
              std::cout << "Vertex NOT in FV" << std::endl;
          }

          if(!sel::beam::ndlar::AllPrimaryParticlesContained(nd_reco_int)){
              std::cout << "All Primary Particles NOT Contained" << std::endl;
          }
        
          if(sel::beam::ndlar::InFV(nd_reco_int) and sel::beam::ndlar::AllPrimaryParticlesContained(nd_reco_int)){
              std::cout << "InFV and All Primaries are Contained!" << std::endl; 
          }
           
          int showersCounter = 0;
          int trueInteractionIndex = nd_reco_int.truth[0];
          float truthOverlap = nd_reco_int.truthOverlap[0];

          std::cout << "###### RECO INTERACTION NUMBER " << nd_reco_int.id << " ######" << std::endl;
        
          if(trueInteractionIndex < SR->mc.nu.size()){
              std::cout << "True Neutrino Energy: " <<  SR->mc.nu[trueInteractionIndex].E << " GeV.     Reco Neutrino Energy: " << nd_reco_int.Enu.calo << " GeV. " << std::endl;
          }
          
          std::cout << " True Int Index: " << trueInteractionIndex << " Truth Overlap: " << truthOverlap << std::endl;
          std::cout << "------- True Interaction Contains the Following Particles: ------- " << std::endl;
        
          if(trueInteractionIndex == -1){
              std::cout << "True Interaction Index was not matched. " << std::endl;
              continue;
          }

          for(auto const &truePart : SR->mc.nu[trueInteractionIndex].prim){
              std::cout << "True PDG: " << truePart.pdg << " With Energy: " << truePart.p.T() << " G4ID: " << truePart.G4ID << std::endl;
          }

          std::cout << "##### Reco Interaction Contains the Following Particles: ##### " << std::endl;

          int temporary_index = 0;
          
          std::cout << "Reco Pandora Particles has:  " << nd_reco_int.part.pandora.size() << " entries." << std::endl;
          continue;
          for(auto const &recPart : nd_reco_int.part.pandora){
                  
              int trueIntIndex = recPart.truth[0].ixn;
              int trueIntIndexSize = recPart.truth.size();
              int truePartIndex = recPart.truth[0].part;
              int trueType = recPart.truth[0].type; 
              int recoPDG = recPart.pdg;
              //IGNORE THIS SECTION
              /*    
              std::cout << " Truth Part Vector Size: " << recPart.truth.size() << std::endl;
              std::cout << " Truth Overlap Vector Size: " << recPart.truthOverlap.size() << std::endl;

              std::cout << " SRTrueInteraction Vector: " << SR->mc.nu.size() << std::endl;
              std::cout << " Matched SRTrueInteraction has : " << SR->mc.nu[trueIntIndex].prim.size() << " primaries. " << std::endl;
              std::cout << " Matched SRTrueParticle: " << SR->mc.nu[trueIntIndex].prim[truePartIndex].pdg << std::endl;
              
              if(temporary_index == 1){
                throw "";
              }
              temporary_index++;
              */
              std::cout << " Reco Object Type: " << recPart.origRecoObjType << std::endl;
              std::cout << " true Part Index: " << truePartIndex << std::endl;

              if(trueIntIndex != trueInteractionIndex){
                  std::cout << "CHECK THIS EVENT! Saved branch level interaction ID and particle level interaction ID DO NOT MATCH!" << std::endl;
                  std::cout << "Branch Level ID: " << trueInteractionIndex << " Particle Level ID: " << trueIntIndex << std::endl;
              }

            //Check the type first to determine which vector to access
            //we probably need to see why secondaries aren't being filled
            
              if(trueIntIndex == -1){
                  std::cout << "----------------------------------------" << std::endl;
                  std::cout << " True Int Index was -1 with Reco PDG " << recoPDG << ". Skipping this reco particle." << std::endl;
                  continue;
              } 
              if(trueIntIndex > SR->mc.nu.size() ){
                  std::cout << "----------------------------------------" << std::endl;
                  std::cout << " Associated interaction index is outside of true interaction size. It is: " << trueIntIndex << " With PDG: " << recoPDG <<                " Type: " << trueType << " and Energy: " << recPart.E << ". Skipping this reco particle." << std::endl;
                  continue;
              }
              if(trueType == 1){
                  std::cout << "----------------------------------------" << std::endl;
                  std::cout << " Particle is type: PRIMARY. Finding matched true particle..." << std::endl;
                    
                  if(truePartIndex > SR->mc.nu[trueIntIndex].prim.size()){
                      std::cout << "----------------------------------------" << std::endl;
                      std::cout << " Associated particle index is outside of true particle size. It is: " << truePartIndex << " With PDG: " << recoPDG <<                     " Type: " << trueType << " and Energy:  " << recPart.E << ". Skipping this reco particle." << std::endl;
                      continue;
                  }
                  if(sel::beam::ndlar::numode::HasTruePrimary(SR->mc.nu[trueIntIndex],111)){
                     std::cout << "FOUND!" << std::endl;
                     std::cout <<"Pi 0 is at event: " << i << " Int Index: " << trueIntIndex << " Reco Index: " << nd_reco_int.id << std::endl; 
                }
                  
                 int primaryTruthPDG = SR->mc.nu[trueIntIndex].prim[truePartIndex].pdg; 
                 float primaryTruthEnergy = SR->mc.nu[trueIntIndex].prim[truePartIndex].p.T();
                 int gID = SR->mc.nu[trueIntIndex].prim[truePartIndex].G4ID;
             
                 std::cout << "Reco Particle Interaction ID: " << trueIntIndex << " Reco Particle ID: " << truePartIndex << std::endl;
                 std::cout << " Reco Particle PDG: " << recoPDG << " Type: " << trueType << " With Reco Energy: " << recPart.E << " Score: " << recPart.score << std::endl;
             
                 std::cout << " Truth Matched Information " << std::endl;
                 std::cout << " True Particle PDG: " << primaryTruthPDG << " With True Energy: " << primaryTruthEnergy << std::endl;
              }
            else if(trueType ==2){
                std::cout << "----------------------------------------" << std::endl;
                std::cout << " Particle is type: PRIMARY PRE FSI. Finding matched true particle..." << std::endl;
                    
                if(truePartIndex > SR->mc.nu[trueIntIndex].prefsi.size()){
                    std::cout << "----------------------------------------" << std::endl;
                    std::cout << " Associated particle index is outside of true particle size. It is: " << truePartIndex << " With PDG: " << recoPDG <<                     " Type: " << trueType << " and Energy:  " << recPart.E << ". Skipping this reco particle." << std::endl;
                    continue;
                }
                int primaryTruthPDG = SR->mc.nu[trueIntIndex].prefsi[truePartIndex].pdg; 
                float primaryTruthEnergy = SR->mc.nu[trueIntIndex].prefsi[truePartIndex].p.T();
                int gID = SR->mc.nu[trueIntIndex].prefsi[truePartIndex].G4ID;
             
                std::cout << "Reco Particle Interaction ID: " << trueIntIndex << " Reco Particle ID: " << truePartIndex << std::endl;
                std::cout << " Reco Particle PDG: " << recoPDG << " Type: " << trueType << " With Reco Energy: " << recPart.E << std::endl; 
                std::cout << " Truth Matched Information " << std::endl;
                std::cout << " True Particle PDG: " << primaryTruthPDG << " With True Energy: " << primaryTruthEnergy << std::endl;
            }
            else if(trueType == 3){   
           //    std::cout << "----------------------------------------" << std::endl;
                std::cout << " Particle is type: SECONDARY. Currently no truth information to find..." << std::endl; 
           //     std::cout << "Reco Particle Interaction ID: " << trueIntIndex << " Reco Particle ID: " << truePartIndex << std::endl;
           //     std::cout << " Reco Particle PDG: " << recoPDG << " Type: " << trueType << " With Reco Energy: " << recPart.E << std::endl;
            }
     
        }
  
      }

    }
  hout.Write();  
}

