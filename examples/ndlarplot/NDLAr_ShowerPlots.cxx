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

  //TH1D *nshowers = new TH1D("nshowers", ";Number of Showers; Count", 10, 0, 10);
  //TH1D *recoPDGs = new TH1D("recoPDGs", "; Reco PDG; Count", 2250, 0, 2250);
  TH1D *containsPi0 = new TH1D("containsPi0", "; Contains Pi0; Count", 13, 0, 13);
  TH1D *energyPi0 = new TH1D("energyPi0", "; Energy (GeV); Count", 50, 0, 10 );
  for(Long64_t i = 0; i < ents; ++i) {
    ch.GetEntry(i);
    std::cout << "****************************************************************** " << std ::endl;
    std::cout << "* Event: " << i << " with " << SR->common.ixn.pandora.size() << " reco interactions and " << SR->mc.nu.size()   << " true interactions. *" << std::endl;   
    std::cout << " ***************************************************************** " << std ::endl;
    /*  
    int showersCounter = 0; 
    int trueInteractionCounter = 0;
    for(auto const &nd_true_int : SR->mc.nu){
        

        if(sel::beam::ndlar::numode::HasTruePrimary(nd_true_int, 111)){

        std::cout << "########## TRUE INTERACTION NUMBER:  " << nd_true_int.id << " INDEX: " << trueInteractionCounter << nd_true_int.genieIdx << " ###########"  << std::endl;
        trueInteractionCounter +=1;
        std::cout << " Containins The Following Particles: " << std::endl;
        
        for(auto const &p : nd_true_int.prim){
            std::cout << "True PDG: " << p.pdg << " Interaction ID = " << p.interaction_id << " With True Energy: " << p.p.T() << std::endl;
        }

        if(sel::beam::ndlar::numode::HasTruePrimary(nd_true_int, 111)){
            for(auto const &truthPart : nd_true_int.prim){
                if(truthPart.pdg == 111){
                    energyPi0->Fill(truthPart.p.T());
                    containsPi0->Fill(i);
                }
            }
        }
        else{
            continue;
        }
        }
    
    }
    */

    // Loop over Reco interactions to begin truth matching

    for(auto const &nd_reco_int : SR->common.ixn.pandora){
     std::cout << "###### RECO INTERACTION NUMBER " << nd_reco_int.id << " ######" << std::endl;
        int showersCounter = 0;
        int trueInteractionIndex = nd_reco_int.truth[0];
        float truthOverlap = nd_reco_int.truthOverlap[0];

        if(trueInteractionIndex < SR->mc.nu.size()){
        std::cout << "True Neutrino Energy: " <<  SR->mc.nu[trueInteractionIndex].E << " GeV.     Reco Neutrino Energy: " << nd_reco_int.Enu.calo/1000 << " GeV. " << std::endl;
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

        std::cout << "#####Reco Interaction Contains the Following Particles: ##### " << std::endl;

        for(auto const &recPart : nd_reco_int.part.pandora){
            int trueIntIndex = recPart.truth[0].ixn;
            int trueIntIndexSize = recPart.truth.size();
            int truePartIndex = recPart.truth[0].part;
            int trueType = recPart.truth[0].type; 
            int recoPDG = recPart.pdg;

          //  std::cout << " There are: " << trueIntIndexSize << " possible true particles." << std::endl;

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
                
          
             int primaryTruthPDG = SR->mc.nu[trueIntIndex].prim[truePartIndex].pdg; 
             float primaryTruthEnergy = SR->mc.nu[trueIntIndex].prim[truePartIndex].p.T();
             int gID = SR->mc.nu[trueIntIndex].prim[truePartIndex].G4ID;
             
             std::cout << "Reco Particle Interaction ID: " << trueIntIndex << " Reco Particle ID: " << truePartIndex << std::endl;
             std::cout << " Reco Particle PDG: " << recoPDG << " Type: " << trueType << " With Reco Energy: " << recPart.E << std::endl;
             
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
            
                std::cout << "----------------------------------------" << std::endl;
                std::cout << " Particle is type: SECONDARY. Currently no truth information to find..." << std::endl; 
                std::cout << "Reco Particle Interaction ID: " << trueIntIndex << " Reco Particle ID: " << truePartIndex << std::endl;
                std::cout << " Reco Particle PDG: " << recoPDG << " Type: " << trueType << " With Reco Energy: " << recPart.E << std::endl;
            }
                   // recoPDGs->Fill(recoPDG);

            /*        if(recoPDG == 22 or recoPDG == 11){
                        showersCounter += 1;
                   
                    }
            
                    else{
                    continue;
                    }
                }
          
            if(showersCounter > 0){
                nshowers->Fill(showersCounter);
            }
          */     
        
        }
  }
}


  
  /*
    ///Trial Stuff just keeping for reference
    for(auto const &nd_int : SR->mc.nu){
  //      interactionCounter += 1;
        if(sel::beam::ndlar::numode::HasTruePrimary(nd_int, 111)){
            std::cout << "Pi0 Found!" << std::endl;
            for(auto const &p : nd_int.prim){
                std::cout << "Particle: " << p.pdg << " Truth:" << std::endl;
                
            }

        }      
    }
    for(auto const &nd_int : SR->nd.lar.pandora){
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
        //std::cout << "ID: " << interactionID << "Particles Conatained: " << std::endl;
        for (auto const &p : nd_int.part.pandora){
            std::cout << " Truth: " << p.truth[0].ixn << std::endl;
            //std::cout << SR->mc.nu::Particle(p.truth[0].ixn) << std::endl;
            
            //FOR LATER: Consider that there are more than 1 true matched check the trueOverlap to choose which is best match 
        
       
            //if(photonCounter > 1){
             //   std::cout << "Event: " << i << " Interaction: " << interactionID << " Contains: " << photonCounter << " Photons" << std::endl;
             //   std::cout << "Particles Contained: " << std::endl;
              //  for(auto const &p : nd_int.part.pandora){
            //        std::cout << p.pdg << std::endl;           
               //     std::cout << "Particle is Primary?: " << p.primary << std::endl;
             //   } 
            }
        }
  //  }
  //}
  */
  hout.Write();
  }

