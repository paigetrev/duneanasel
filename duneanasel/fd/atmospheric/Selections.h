#pragma once

#include "duneanaobj/StandardRecord/SRInteraction.h"

#include "duneanasel/common/type_traits.h"

namespace sel {
namespace atm {

namespace fd1x2x6 {

enum Sample { kRejected = 0, kNuMuCCLike, kNuECCLike, kNCLike };

// private correspondance from Pierre Granger
template <typename T, typename C = Proxyable_t<caf::SRInteraction, T>>
inline bool InFV(T const &fd_int) {
  if (std::fabs(fd_int.vtx.x) > 343) {
    return false;
  }
  if (std::fabs(fd_int.vtx.y) > 588) {
    return false;
  }
  float const &z = fd_int.vtx.z;
  if ((z < 20) || (z > 1374)) {
    return false;
  }

  return true;
}

// https://github.com/DUNE/atmospherics-tools/blob/401037e4ced8ff1ab48fefacf0929bbc260b6489/weight_calculation/app/split_channels.cxx#L142-L153
template <typename T, typename C = Proxyable_t<caf::SRInteraction, T>>
inline bool NumuCCLike(T const &fd_int) {
  return fd_int.nuhyp.cvn.numu > 0.56;
}

template <typename T, typename C = Proxyable_t<caf::SRInteraction, T>>
inline bool NueCCLike(T const &fd_int) {

  return !NumuCCLike(fd_int) && (fd_int.nuhyp.cvn.nue > 0.56);
}

template <typename T, typename C = Proxyable_t<caf::SRInteraction, T>>
inline bool NCLike(T const &fd_int) {
  return !NumuCCLike(fd_int) && !NueCCLike(fd_int);
}

template <typename T, typename C = Proxyable_t<caf::SRInteraction, T>>
inline Sample ApplySelection(T const &fd_int) {
  if (!InFV(fd_int)) {
    return sel::atm::kRejected;
  }
  if (sel::atm::fd1x2x6::NuMuCCLike(fd_int)) {
    return sel::atm::fd1x2x6::kNuMuCCLike;
  } else if (sel::atm::fd1x2x6::NuMuCCLike(fd_int)) {
    return sel::atm::fd1x2x6::kNuECCLike;
  } else if (sel::atm::fd1x2x6::NCLike(fd_int)) {
    return sel::atm::fd1x2x6::kNCLike;
  }
  return sel::atm::fd1x2x6::kRejected;
}

} // namespace fd1x2x6

using namespace FDHD = fd1x2x6;
using namespace FD2 = FDHD;

} // namespace atm
} // namespace sel
