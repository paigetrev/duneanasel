#include <ROOT/RVec.hxx>
#include "duneanaobj/StandardRecord/SRTrueInteraction.h"
#include "duneanaobj/StandardRecord/SRTrueParticle.h"
#include "duneanaobj/StandardRecord/SRVector3D.h"
#include "duneanaobj/StandardRecord/SRLorentzVector.h"
ROOT::VecOps::RVec<Float_t> nu_E_by_pdg(const ROOT::VecOps::RVec<caf::SRTrueInteraction>& p, Int_t pdg_target) {

    ROOT::VecOps::RVec<Float_t> OUT;
    for (const auto & nu : p) {
        if (nu.pdg == pdg_target) {
            OUT.push_back(nu.E);
        }
    }

    return OUT;
}

ROOT::VecOps::RVec<Int_t> prim_pdg(const ROOT::VecOps::RVec<caf::SRTrueInteraction>& p, Int_t nu_pdg) {
    ROOT::VecOps::RVec<Int_t> OUT ;

    for (const auto & inter : p) {
        if (inter.pdg != nu_pdg) continue;
        for (const auto & prim : inter.prim) {
                OUT.push_back(prim.pdg);
        } 
    }

    return OUT;
}

ROOT::VecOps::RVec<Float_t> prim_p_by_pdg(const ROOT::VecOps::RVec<caf::SRTrueInteraction>& p, Int_t nu_pdg, Int_t prim_pdg) {
    ROOT::VecOps::RVec<Float_t> OUT ;

    for (const auto & inter : p) {
        if (inter.pdg != nu_pdg) continue;
        for (const auto & prim : inter.prim) {
            if(prim.pdg == prim_pdg) OUT.push_back(prim.p.Mag());
        } 
    }

    return OUT;
}