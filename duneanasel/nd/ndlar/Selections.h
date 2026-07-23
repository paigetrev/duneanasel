#pragma once

#include "duneanasel/common/RecoTools.h"
#include "duneanaobj/StandardRecord/SRTrueParticle.h"
//add SRTrueInteraction.h

// The selection in this file was originally developed by Eva Sabater, what
// follows is a transcription from the original CAFAna code.

namespace sel {
namespace beam {

namespace ndlar {

enum Sample { kRejected = 0, kNuMuCCLikeContained, kNuECCLikeContained };

constexpr double NDLArXLo = -346.9;
constexpr double NDLArXHi = 346.9;
constexpr double NDLArYLo = -215.5;
constexpr double NDLArYHi = 81.7;
constexpr double NDLArZLo = 418.2;
constexpr double NDLArZHi = 913.3;

template <typename T, typename C = Proxyable_t<caf::SRInteraction, T>>
inline bool IntHasParticles(T const &nd_int) {
  if (nd_int.part.dlp.empty()) {
    return false;
  }
  return true;
}

template <typename T, typename C = Proxyable_t<caf::SRInteraction, T>>
inline bool LongestPrimTrackLengthCut(T const &nd_int) {
  auto plong = ana::GetLongestParticle(nd_int);
  if (!plong) {
    return false;
  }
  return ana::ParticleLength(*plong) > 0.1;
}

template <typename T, typename C = Proxyable_t<caf::SRInteraction, T>>
inline bool InFV(T const &nd_int) {

  float x = nd_int.vtx.x;
  if ((x <= (NDLArXLo + 25)) || (x >= (NDLArXHi - 25))) {
    return false;
  }
  float y = nd_int.vtx.y;
  if ((y <= (NDLArYLo + 25)) || (y >= (NDLArYHi - 25))) {
    return false;
  }
  float z = nd_int.vtx.z;
  if ((z <= (NDLArZLo + 25)) || (z >= (NDLArZHi - 25))) {
    return false;
  }

  return true;
}

template <typename T, typename C = Proxyable_t<caf::SRInteraction, T>>
inline bool AllPrimaryParticlesContained(T const &nd_int) {
  for (auto const &p : nd_int.part.pandora) {
    if ((p.primary == 1) && (p.contained != 1)) {
      return false;
    }
  }
  return true;
}


template <typename T, typename C = Proxyable_t<caf::SRInteraction, T>>
inline bool ParticlesNDLArContainedMuonsEscapeDownstream(T const &nd_int) {
  for (auto const &p : nd_int.part.dlp){
    bool contained = (p.contained == 1);
    bool isPrimaryMuon = (p.pdg == 13 && p.primary == 1);
    bool escapesToTMS = false;

    if (isPrimaryMuon) {
      float x = p.end.x;
      float y = p.end.y;
      float z = p.end.z;

      escapesToTMS =
        (x < (NDLArXHi - 25)) && (x > (NDLArXLo + 25)) &&
        (y < (NDLArYHi - 25)) && (y > (NDLArYLo + 25)) &&
        (z > (NDLArZLo + 25));
    }
    if (!contained && !escapesToTMS) {
      return false;
    }
  }
  return true;
}

template <typename T, typename C = Proxyable_t<caf::SRInteraction, T>>
inline bool HasParticleWithReconstructedPID(T const &nd_int, int pid) {
  for (auto const &p : nd_int.part.pandora) {
    if ((p.primary == 1) && (p.pdg == pid)) {
      return false;
    }
  }
  return false;
}
/*
template <typename T, typename C = Proxyable_t<caf::SRInteraction, T>>
inline bool HasTruePrimaryParticle(T const &nd_int, int pdg) {
    for(auto const &p : nd_int.prim){
      if(p.pdg == pdg){
        return true;
      }
    }
    return false;
}
*/
namespace numode {

template <typename T, typename C = Proxyable_t<caf::SRInteraction, T>>
inline bool NuMuCCLikeContained(T const &nd_int) {
  if (!InFV(nd_int) || !AllPrimaryParticlesContained(nd_int)) {
    return false;
  }
  auto plong = ana::GetLongestParticle(nd_int);
  if (!plong || (plong->pdg != 13)) {
    return false;
  }
  return true;
}

template <typename T, typename C = Proxyable_t<caf::SRInteraction, T>>
inline bool NuMuCCLike(T const &nd_int) {
  for (auto const &p : nd_int.part.dlp) {
    if (p.primary == 1 && p.pdg == 13) {
      return true;
    }
  }
  return false;
}

template <typename T, typename C = Proxyable_t<caf::SRInteraction, T>>
inline bool NuECCLikeContained(T const &nd_int) {
  if (!InFV(nd_int) || !AllPrimaryParticlesContained(nd_int) ||
      !HasParticleWithReconstructedPID(nd_int, 11)) {
    return false;
  }
  return true;
}

template <typename T, typename C = Proxyable_t<caf::SRInteraction, T>>
inline sel::beam::ndlar::Sample ApplySelectionV0(T const &fd_int) {

  if (!sel::beam::ndlar::InFV(fd_int)) {
    return sel::beam::ndlar::kRejected;
  }

  if (sel::beam::ndlar::numode::NuMuCCLikeContained(fd_int)) {
    return sel::beam::ndlar::kNuMuCCLikeContained;
  } else if (sel::beam::ndlar::numode::NuECCLikeContained(fd_int)) {
    return sel::beam::ndlar::kNuECCLikeContained;
  }
  return sel::beam::ndlar::kRejected;
}

template <typename T, typename C = Proxyable_t<caf::SRInteraction, T>>
inline sel::beam::ndlar::Sample ApplySelectionV0p1(T const &nd_int) {

  if (!sel::beam::ndlar::IntHasParticles(nd_int) 
   || !sel::beam::ndlar::LongestPrimTrackLengthCut(nd_int)
   || !sel::beam::ndlar::InFV(nd_int)
   || !sel::beam::ndlar::ParticlesNDLArContainedMuonsEscapeDownstream(nd_int)) {
    return sel::beam::ndlar::kRejected;
  }

  if (sel::beam::ndlar::numode::NuMuCCLike(nd_int)) {
    return sel::beam::ndlar::kNuMuCCLikeContained;
  } else if (sel::beam::ndlar::numode::NuECCLikeContained(nd_int)) {
    return sel::beam::ndlar::kNuECCLikeContained;
  }
  return sel::beam::ndlar::kRejected;
}


template <typename T, typename C = Proxyable_t<caf::SRTrueInteraction, T>>
inline bool HasTruePrimary(T const &nd_int, int true_pdg) {
    for(auto const &p : nd_int.prim){
        if(p.pdg == true_pdg){
            return true;   
        }
    }  
return false;
}


} // namespace numode
} // namespace ndlar

} // namespace beam
} // namespace sel