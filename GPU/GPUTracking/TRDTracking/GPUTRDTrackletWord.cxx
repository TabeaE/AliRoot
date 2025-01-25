//**************************************************************************\
//* This file is property of and copyright by the ALICE Project            *\
//* ALICE Experiment at CERN, All rights reserved.                         *\
//*                                                                        *\
//* Primary Authors: Matthias Richter <Matthias.Richter@ift.uib.no>        *\
//*                  for The ALICE HLT Project.                            *\
//*                                                                        *\
//* Permission to use, copy, modify and distribute this software and its   *\
//* documentation strictly for non-commercial purposes is hereby granted   *\
//* without fee, provided that the above copyright notice appears in all   *\
//* copies and that both the copyright notice and this permission notice   *\
//* appear in the supporting documentation. The authors make no claims     *\
//* about the suitability of this software for any purpose. It is          *\
//* provided "as is" without express or implied warranty.                  *\
//**************************************************************************

/// \file GPUTRDTrackletWord.cxx
/// \author Ole Schmidt

#include "GPUTRDTrackletWord.h"
using namespace GPUCA_NAMESPACE::gpu;

#ifndef GPUCA_TPC_GEOMETRY_O2

GPUd() GPUTRDTrackletWord::GPUTRDTrackletWord(uint32_t trackletWord) : mHCId(-1), mTrackletWord(trackletWord)
{
}
GPUd() GPUTRDTrackletWord::GPUTRDTrackletWord(uint32_t trackletWord, int32_t hcid) : mHCId(hcid), mTrackletWord(trackletWord) {}

#ifdef GPUCA_ALIROOT_LIB
#include "AliTRDtrackletWord.h"
#include "AliTRDtrackletMCM.h"

GPUTRDTrackletWord::GPUTRDTrackletWord(const AliTRDtrackletWord& rhs) : mHCId(rhs.GetHCId()), mTrackletWord(rhs.GetTrackletWord())
{
}
GPUTRDTrackletWord::GPUTRDTrackletWord(const AliTRDtrackletMCM& rhs) : mHCId(rhs.GetHCId()), mTrackletWord(rhs.GetTrackletWord()) {}

GPUTRDTrackletWord& GPUTRDTrackletWord::operator=(const AliTRDtrackletMCM& rhs)
{
  this->~GPUTRDTrackletWord();
  new (this) GPUTRDTrackletWord(rhs);
  return *this;
}

#endif // GPUCA_ALIROOT_LIB

GPUd() int32_t GPUTRDTrackletWord::GetYbin() const
{
  // returns (signed) value of Y
  if (mTrackletWord & 0x1000) {
    return -((~(mTrackletWord - 1)) & 0x1fff);
  } else {
    return (mTrackletWord & 0x1fff);
  }
}

GPUd() int32_t GPUTRDTrackletWord::GetdYbin() const
{
  // returns (signed) value of the deflection length
  if (mTrackletWord & (1 << 19)) {
    return -((~((mTrackletWord >> 13) - 1)) & 0x7f);
  } else {
    return ((mTrackletWord >> 13) & 0x7f);
  }
}

#endif // !GPUCA_TPC_GEOMETRY_O2
