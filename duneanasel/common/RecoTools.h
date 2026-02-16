#pragma once

#include "duneanaobj/StandardRecord/SRInteraction.h"

#include "duneanasel/common/type_traits.h"

#include <limits>

namespace ana {

enum RecoBranch { kPandora, kSPINE, kNRecoBranchs };

template <typename T, typename C = Proxyable_t<caf::SRRecoParticle, T>>
inline double ParticleLength(T const &p) {
  return std::hypot(p.end.x - p.start.x, p.end.y - p.start.y,
                    p.end.z - p.start.z);
}

template <typename T, typename C = Proxyable_t<caf::SRInteraction, T>>
inline auto const *GetRecoParticles(T const &nd_int, RecoBranch reco = kPandora) {
  auto const *parts =
      &nd_int.part.pandora; // set it to pandora first so that we can get the
                            // right template-dependent type, then set it to the
                            // one we really want.
  switch (reco) {
  case kPandora: {
    parts = &nd_int.part.pandora;
    break;
  }
  case kSPINE: {
    parts = &nd_int.part.dlp;
    break;
  }
  default: {
    throw std::runtime_error("Invalid RecoBranch specified.");
  }
  }

  return parts;
}

// the auto return type here lets the compiler figure out whether to return a
//  caf::SRRecoParticle * or a caf::SRRecoParticleProxy * for a given
//  instantiation
template <typename T, typename C = Proxyable_t<caf::SRInteraction, T>>
inline auto const *GetLongestParticle(T const &nd_int,
                                      RecoBranch reco = kPandora) {

  auto parts = GetRecoParticles(nd_int, reco);

  double longest = -std::numeric_limits<double>::max();
  size_t longest_idx = std::numeric_limits<size_t>::max();
  for (size_t i = 0; i < parts->size(); ++i) {
    const auto &p = parts->at(i);
    if (p.primary != 1) {
      continue;
    }
    double const &plen = ParticleLength(p);
    if (plen > longest) {
      longest = plen;
      longest_idx = i;
    }
  }
  return (longest_idx == std::numeric_limits<size_t>::max())
             ? nullptr
             : &parts->at(longest_idx);
}

} // namespace ana
