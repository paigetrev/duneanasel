#include "duneanasel/nd/ndlar/Selections.h"

#include "duneanaobj/StandardRecord/Proxy/SRProxy.h"
#include "duneanaobj/StandardRecord/StandardRecord.h"

#include "TChain.h"
#include "TFile.h"
#include "TH1D.h"

#include <memory>
#include <fstream>
#include <string>
#include <iostream>

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

  TFile hout("vtx_position.root", "RECREATE");
  TH1D vtxX("vtrxX", ";Vertex X [cm]; Count", 100, -350, 350);
  TH1D vtxY("vtrxY", ";Vertex Y [cm]; Count", 100, -220, 85);
  TH1D vtxZ("vtrxZ", ";Vertex Z [cm]; Count", 100, 410, 920);
  for (Long64_t i = 0; i < ents; ++i) {
    ch.GetEntry(i);

    std::cout << "Event: " << i << " with " << SR->common.ixn.dlp.size()
              << " dlp interactions." << std::endl;
    int j = 0;
    for (auto const &nd_int : SR->common.ixn.dlp) {
      std::cout << "  Interaction: " << j++ << " | InFV: "
                << (sel::beam::ndlar::InFV(nd_int) ? "true" : "false")
                << " | AllPrimaryParticlesContainedMuonsEscapeDownstream: "
                << (sel::beam::ndlar::ParticlesNDLArContainedMuonsEscapeDownstream(nd_int)
                        ? "true"
                        : "false");
      auto longp = ana::GetLongestParticle(nd_int);
      if (longp) {
        std::cout << " | LongestParticle: { pid: " << longp->pdg
                  << ", len: " << ana::ParticleLength(*longp) << " cm"
                  << std::endl;
      } else {
        std::cout << " | LongestParticle: null" << std::endl;
      }

      if (sel::beam::ndlar::numode::ApplySelectionV0p1(nd_int) !=
          sel::beam::ndlar::kNuMuCCLikeContained) {
        continue;
      } else {
        std::cout << "  => Selected!" << std::endl;
      }

      vtxX.Fill(nd_int.vtx.x);
      vtxY.Fill(nd_int.vtx.y);
      vtxZ.Fill(nd_int.vtx.z);

    }
  }

  hout.Write();
}
