# duneanasel

A development package for testing the design and implementation of a common
selections/observables package for DUNE ND and FD high-level analysis.

## Requirements / Design Philosophy

*Requirements:*
* Provide header-only, free functions defining event selections and event observables that satisfy high-level analysis needs for the near detector and far detector event samples.
* Functions should operate on the hierarchial analysis objects defined in [`duneanaobj`](https://github.com/DUNE/duneanaobj).
* Function should be able to operate on `caf::Proxy<T>` wrappers of instances of the `duneanaobj`ects to enable the I/O speedup on offer.

*Design Choices:*
* Make use of nested namespaces to scope generically named selection and projection functions: __e.g.__ `sel::beam::fd1x8x6::NuMuCCLike`.
* Use SFINAE template-based type constraints to provide single functions that can take either `duneanaobj`ect instances or `caf::Proxy<T>` wrappers of instances.
* Use unscoped enum types to identify events passing cuts for specific samples.
* Provide a CMake build system capable of building the `duneanaobj` dependency without user intervention, under the expectation that `duneanaobj` changes much more slowly than development on the common tools package and so non-expert co-development of both package is infrequent.

## Selections

Example selection functions for a beam neutrino selection in FD2 are implemented with the below call signatures:

```c++
//in duneanasel/fd/beam/Selections.h

namespace sel::beam::fd1x8x6 {
  enum Sample { kRejected = 0, kNuMuCCLike, kNuECCLike, kNCLike };

  template <typename T, typename SRTypeConstraint = Proxyable_t<caf::SRInteraction, T>>
  bool InFV(T const &fd_int);

  template <typename T, typename SRTypeConstraint = Proxyable_t<caf::SRInteraction, T>>
  Sample numode::ApplySelection(T const &fd_int);

}
```

These operate on either `caf::SRInteraction` instances or `caf::Proxy<caf::SRInteraction>` instances.

The current selection functions for the atmospheric neutrino selection in FD1 are implemented with similar call signatures, but scoped to a different namespace:

```c++
//in duneanasel/fd/atm/Selections.h

namespace sel::atm::fd1x2x6 {
  enum Sample { kRejected = 0, kNuMuCCLike, kNuECCLike, kNCLike };

  template <typename T, typename SRTypeConstraint = Proxyable_t<caf::SRInteraction, T>>
  bool InFV(T const &fd_int);

  template <typename T, typename SRTypeConstraint = Proxyable_t<caf::SRInteraction, T>>
  Sample ApplySelection(T const &fd_int);
}
```

### A Note On Template Usage

The `Proxyable_t` type trait is defined in [duneanasel/common/type_traits.h](duneanasel/common/type_traits.h) and its usage above causes an error if any type other than `caf::SRInteraction` instances or `caf::Proxy<caf::SRInteraction>` is passed to the templated functions. It is essentially acting as a pre C++20 type constraint (concept). The worry with using templates is that the compiler error messages become opaque for even quite seasoned users. The `Proxyable_t` type contains a `static_assert` that gives a semi-sensible error message upon failure, however, it is at the top of a potentially long list of template-based error messages from the compiler. The age-old-adage of *always fix error messages from the top down* is important here. A example of incorrect usage and an example error message is given below:

```c++
double test = 5;
sel::atm::fd1x2x6::InFV(test);
```

might result in an error message like:

```
[ 81%] Building CXX object CMakeFiles/myana.dir/myana.cxx.o
In file included from /root/software/DUNE/duneanasel/examples/user_project/build/_deps/duneanasel-src/duneanasel/fd/beam/Selections.h:5,
                 from /root/software/DUNE/duneanasel/examples/user_project/myana.cxx:1:
/root/software/DUNE/duneanasel/examples/user_project/build/_deps/duneanasel-src/duneanasel/common/type_traits.hxx: In instantiation of ‘struct Proxyable<caf::SRInteraction, double>’:
/root/software/DUNE/duneanasel/examples/user_project/build/_deps/duneanasel-src/duneanasel/common/type_traits.hxx:18:7:   required by substitution of ‘template<class SRType, class T> using Proxyable_t = typename Proxyable::type [with SRType = caf::SRInteraction; T = double]’
/root/software/DUNE/duneanasel/examples/user_project/build/_deps/duneanasel-src/duneanasel/fd/beam/Selections.h:77:23:   required by substitution of ‘template<class T, class C> bool sel::beam::fd1x2x6::InFV(const T&) [with T = double; C = <missing>]’
/root/software/DUNE/duneanasel/examples/user_project/myana.cxx:24:29:   required from here
/root/software/DUNE/duneanasel/examples/user_project/build/_deps/duneanasel-src/duneanasel/common/type_traits.hxx:11:25: error: static assertion failed: Invalid type used for template function
   11 |   static_assert(is_type || is_proxy_to_type,
      |                 ~~~~~~~~^~~~~~~~~~~~~~~~~~~
/root/software/DUNE/duneanasel/examples/user_project/build/_deps/duneanasel-src/duneanasel/common/type_traits.hxx:11:25: note: ‘(((bool)Proxyable<caf::SRInteraction, double>::is_type) || ((bool)Proxyable<caf::SRInteraction, double>::is_proxy_to_type))’ evaluates to false
In file included from /usr/include/c++/11/bits/move.h:57,
                 from /usr/include/c++/11/bits/stl_pair.h:59,
                 from /usr/include/c++/11/bits/stl_algobase.h:64,
                 from /usr/include/c++/11/bits/specfun.h:45,
                 from /usr/include/c++/11/cmath:1935,
                 from /root/software/DUNE/duneanasel/examples/user_project/build/_deps/duneanaobj-src/duneanaobj/StandardRecord/Proxy/../../../duneanaobj/StandardRecord/SRVector3D.h:14,
                 from /root/software/DUNE/duneanasel/examples/user_project/build/_deps/duneanaobj-src/duneanaobj/StandardRecord/Proxy/../../../duneanaobj/StandardRecord/SRDirectionBranch.h:10,
                 from /root/software/DUNE/duneanasel/examples/user_project/build/_deps/duneanaobj-src/duneanaobj/StandardRecord/Proxy/../../../duneanaobj/StandardRecord/SRInteraction.h:9,
                 from /root/software/DUNE/duneanasel/examples/user_project/build/_deps/duneanasel-src/duneanasel/fd/beam/Selections.h:3,
                 from /root/software/DUNE/duneanasel/examples/user_project/myana.cxx:1:
/usr/include/c++/11/type_traits: In substitution of ‘template<bool _Cond, class _Tp> using enable_if_t = typename std::enable_if::type [with bool _Cond = false; _Tp = double]’:
/root/software/DUNE/duneanasel/examples/user_project/build/_deps/duneanasel-src/duneanasel/common/type_traits.hxx:14:9:   required from ‘struct Proxyable<caf::SRInteraction, double>’
/root/software/DUNE/duneanasel/examples/user_project/build/_deps/duneanasel-src/duneanasel/common/type_traits.hxx:18:7:   required by substitution of ‘template<class SRType, class T> using Proxyable_t = typename Proxyable::type [with SRType = caf::SRInteraction; T = double]’
/root/software/DUNE/duneanasel/examples/user_project/build/_deps/duneanasel-src/duneanasel/fd/beam/Selections.h:77:23:   required by substitution of ‘template<class T, class C> bool sel::beam::fd1x2x6::InFV(const T&) [with T = double; C = <missing>]’
/root/software/DUNE/duneanasel/examples/user_project/myana.cxx:24:29:   required from here
/usr/include/c++/11/type_traits:2579:11: error: no type named ‘type’ in ‘struct std::enable_if<false, double>’
 2579 |     using enable_if_t = typename enable_if<_Cond, _Tp>::type;
      |           ^~~~~~~~~~~
/root/software/DUNE/duneanasel/examples/user_project/myana.cxx: In function ‘int main(int, const char**)’:
/root/software/DUNE/duneanasel/examples/user_project/myana.cxx:24:29: error: no matching function for call to ‘InFV(double&)’
   24 |     sel::beam::fd1x2x6::InFV(test);
      |     ~~~~~~~~~~~~~~~~~~~~~~~~^~~~~~
In file included from /root/software/DUNE/duneanasel/examples/user_project/myana.cxx:1:
/root/software/DUNE/duneanasel/examples/user_project/build/_deps/duneanasel-src/duneanasel/fd/beam/Selections.h:78:13: note: candidate: ‘template<class T, class C> bool sel::beam::fd1x2x6::InFV(const T&)’
   78 | inline bool InFV(T const &fd_int) {
      |             ^~~~
/root/software/DUNE/duneanasel/examples/user_project/build/_deps/duneanasel-src/duneanasel/fd/beam/Selections.h:78:13: note:   substitution of deduced template arguments resulted in errors seen above
make[2]: *** [CMakeFiles/myana.dir/build.make:76: CMakeFiles/myana.dir/myana.cxx.o] Error 1
make[1]: *** [CMakeFiles/Makefile2:323: CMakeFiles/myana.dir/all] Error 2
make: *** [Makefile:136: all] Error 2

```

Where the important line near the top is:

```
error: static assertion failed: Invalid type used for template function
   11 |   static_assert(is_type || is_proxy_to_type,
      |                 ~~~~~~~~^~~~~~~~~~~~~~~~~~~
/root/software/DUNE/duneanasel/examples/user_project/build/_deps/duneanasel-src/duneanasel/common/type_traits.hxx:11:25: note: ‘(((bool)Proxyable<caf::SRInteraction, double>::is_type) || ((bool)Proxyable<caf::SRInteraction, double>::is_proxy_to_type))’ evaluates to false
```

## Observable Projections

The structure of these functions follow the same logic as the Selections functions. The example below is scoped to usage for beam neutrino events from FD2 and uses the namespace-scoped FD2 beam samples(unscoped) enums, however, it is possible that the internal logic may be equivalent for any FD detector or neutrino source.

```c++
//in duneanasel/fd/beam/Observables.h

template <typename T, typename SRTypeConstraint = Proxyable_t<caf::SRInteraction, T>>
float proj::beam::fd1x8x6::ENuReco(T const &fd_int, sel::beam::fd1x8x6::Sample smpl)
```

## Buildme

As the tools provided by this package are header-only, there isn't a whole lot to build directly. However,
the buildsystem exists to check if dependencies are built and optionally build them if not, specifically
[`duneanaobj`](https://github.com/DUNE/duneanaobj). A few configuration options exist depending on how
you want to find/build `duneanaobj`:

* `cmake .. -Dduneanasel_USE_UPS_duneanaobj=ON`:
  Will try and use duneanaobj/SRProxy from a set up UPS distribution.
* `cmake .. [-Dduneanasel_USE_SRProxy=ON] [-Dduneanasel_SRProxy_PYTHON_ENABLED=OFF]`:
  Enabled by default, this will set up or build the clingified version of
  duneanaobj. Optionally `StandardRecord` python binding generation can be
  enabled. You will have to manually add `<prefix>/python/<pyversion maj>.<min>`
  to `PYTHONPATH` and `<prefix>lib` to `LD_LIBRARY_PATH=` to enable
  `python3 -c "import pySRProxy"`.
* `cmake .. -Dduneanasel_USE_SRProxy=OFF [-DDUNE_ANAOBJ_BRANCH=v03_06_01]`:
  This will build just the `StandardRecord` class from a specified version of
  duneanaobj. It will not build SRProxy for you and so no `caf::Proxy` instances
  can be used.

C++ Examples can be built by configuring like `cmake .. -Dduneanasel_build_examples=ON`.

## Example Usage

The following sections give brief descriptions of the example usage contained in this repository.

### Cut Motivation in python

An example of using the [`SRProxy`]()-generated python bindings to perform a simple cut optimization in `pandas` can be found [here](examples/cut_optimization/fd/VDFDToySelections.ipynb).

### An application built by this project

An example of using selections defined in this package to write an application that splits an incoming CAF TTree into separate TTree's based on the selection cuts that each entry passes can be found [here](examples/sample_separation/separate_FD_sample.cxx). This executable is built by this package as an example for how to add compiled code/applications to this repository.

* An NDLAr example that applys a simple selection and outputs a histogram can be found [here](examples/ndlarplot/NDLAr_NumuCCContained_LongestTrack.cxx).

### An application built by a dependent project

An example of an empty analysis loop exectuable with a minimal CMake build system can be found [here](examples/user_project). This directory should not be added to the main buildsystem used by this project, but can be used as a skeleton for new CMake projects that should depend on the selections and observables defined in this repository.

### Fiducial volume cuts and summary files for analysis software

Another example of using the `SRProxy` python bindings (`pySRProxy`). This example applies some cuts with `pandas` and then outputs a root file using `uproot` that could be used as input to the [MaCh3](https://github.com/mach3-software/MaCh3) analysis software.
