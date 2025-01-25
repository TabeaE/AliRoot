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

/// \file GPUO2Interface.cxx
/// \author David Rohr

#include "GPUO2InterfaceUtils.h"
#include "GPUO2InterfaceConfiguration.h"
#include "GPUO2InterfaceRefit.h"
#include "TPCPadGainCalib.h"
#include "CalibdEdxContainer.h"
#include "TPCBase/Sector.h"
#include "DataFormatsTPC/Digit.h"
#include "GPUParam.h"
#include "GPUReconstructionConvert.h"
#include "DataFormatsTPC/Digit.h"
#include "DetectorsRaw/RDHUtils.h"
#include "TPCBase/CRU.h"
#include "TPCBase/RDHUtils.h"
#include "DataFormatsTPC/ZeroSuppression.h"
#include <gsl/span>

using namespace o2::gpu;

using DigitArray = std::array<gsl::span<const o2::tpc::Digit>, o2::tpc::Sector::MAXSECTOR>;

std::unique_ptr<TPCPadGainCalib> GPUO2InterfaceUtils::getPadGainCalibDefault()
{
  return std::make_unique<TPCPadGainCalib>();
}

std::unique_ptr<TPCPadGainCalib> GPUO2InterfaceUtils::getPadGainCalib(const o2::tpc::CalDet<float>& in)
{
  return std::make_unique<TPCPadGainCalib>(in);
}

std::unique_ptr<o2::tpc::CalibdEdxContainer> GPUO2InterfaceUtils::getCalibdEdxContainerDefault()
{
  return std::make_unique<o2::tpc::CalibdEdxContainer>();
}

template <>
void GPUO2InterfaceUtils::RunZSEncoder<DigitArray>(const DigitArray& in, std::unique_ptr<uint64_t[]>* outBuffer, uint32_t* outSizes, o2::raw::RawFileWriter* raw, const o2::InteractionRecord* ir, int32_t version, bool verify, float threshold, bool padding, std::function<void(std::vector<o2::tpc::Digit>&)> digitsFilter)
{
  GPUParam param;
  param.SetDefaults(5.00668);
  o2::gpu::GPUReconstructionConvert::RunZSEncoder(in, outBuffer, outSizes, raw, ir, param, version, verify, threshold, padding, digitsFilter);
}
template <>
void GPUO2InterfaceUtils::RunZSEncoder<DigitArray>(const DigitArray& in, std::unique_ptr<uint64_t[]>* outBuffer, uint32_t* outSizes, o2::raw::RawFileWriter* raw, const o2::InteractionRecord* ir, GPUO2InterfaceConfiguration& config, int32_t version, bool verify, bool padding, std::function<void(std::vector<o2::tpc::Digit>&)> digitsFilter)
{
  GPUParam param;
  param.SetDefaults(&config.configGRP, &config.configReconstruction, &config.configProcessing, nullptr);
  o2::gpu::GPUReconstructionConvert::RunZSEncoder(in, outBuffer, outSizes, raw, ir, param, version, verify, config.configReconstruction.tpc.zsThreshold, padding, digitsFilter);
}

void GPUO2InterfaceUtils::GPUReconstructionZSDecoder::DecodePage(std::vector<o2::tpc::Digit>& outputBuffer, const void* page, uint32_t tfFirstOrbit, const GPUParam* param, uint32_t triggerBC)
{
  const o2::header::RAWDataHeader* rdh = (const o2::header::RAWDataHeader*)page;
  if (o2::raw::RDHUtils::getMemorySize(*rdh) == sizeof(o2::header::RAWDataHeader)) {
    return;
  }
  o2::tpc::TPCZSHDR* const hdr = (o2::tpc::TPCZSHDR*)(o2::tpc::rdh_utils::getLink(o2::raw::RDHUtils::getFEEID(*rdh)) == o2::tpc::rdh_utils::DLBZSLinkID ? ((const char*)page + o2::raw::RDHUtils::getMemorySize(*rdh) - sizeof(o2::tpc::TPCZSHDRV2)) : ((const char*)page + sizeof(o2::header::RAWDataHeader)));

  if (mDecoders.size() < hdr->version + 1) {
    mDecoders.resize(hdr->version + 1);
  }
  if (mDecoders[hdr->version] == nullptr) {
    mDecoders[hdr->version] = GPUReconstructionConvert::GetDecoder(hdr->version, param);
  }
  mDecoders[hdr->version](outputBuffer, page, tfFirstOrbit, triggerBC);
}

std::unique_ptr<GPUParam> GPUO2InterfaceUtils::getFullParam(float solenoidBz, uint32_t nHbfPerTf, std::unique_ptr<GPUO2InterfaceConfiguration>* pConfiguration, std::unique_ptr<GPUSettingsO2>* pO2Settings, bool* autoMaxTimeBin)
{
  std::unique_ptr<GPUParam> retVal = std::make_unique<GPUParam>();
  std::unique_ptr<GPUO2InterfaceConfiguration> tmpConfig;
  std::unique_ptr<GPUSettingsO2> tmpSettingsO2;
  if (!pConfiguration) {
    tmpConfig = std::make_unique<GPUO2InterfaceConfiguration>();
    pConfiguration = &tmpConfig;
    (*pConfiguration)->configGRP.grpContinuousMaxTimeBin = -1;
  } else if (!*pConfiguration) {
    *pConfiguration = std::make_unique<GPUO2InterfaceConfiguration>();
    (*pConfiguration)->configGRP.grpContinuousMaxTimeBin = -1;
  }
  (*pConfiguration)->configGRP.solenoidBzNominalGPU = solenoidBz;
  if (pO2Settings && *pO2Settings) {
    **pO2Settings = (*pConfiguration)->ReadConfigurableParam();
  } else {
    if (!pO2Settings) {
      pO2Settings = &tmpSettingsO2;
    }
    *pO2Settings = std::make_unique<GPUSettingsO2>((*pConfiguration)->ReadConfigurableParam());
  }
  if (nHbfPerTf == 0) {
    nHbfPerTf = (*pO2Settings)->overrideNHbfPerTF ? (*pO2Settings)->overrideNHbfPerTF : 256;
  }
  if (autoMaxTimeBin) {
    *autoMaxTimeBin = (*pConfiguration)->configGRP.grpContinuousMaxTimeBin == -1;
  }
  if ((*pConfiguration)->configGRP.grpContinuousMaxTimeBin == -1) {
    (*pConfiguration)->configGRP.grpContinuousMaxTimeBin = GPUO2InterfaceUtils::getTpcMaxTimeBinFromNHbf(nHbfPerTf);
  }
  retVal->SetDefaults(&(*pConfiguration)->configGRP, &(*pConfiguration)->configReconstruction, &(*pConfiguration)->configProcessing, nullptr);
  return retVal;
}

std::shared_ptr<GPUParam> GPUO2InterfaceUtils::getFullParamShared(float solenoidBz, uint32_t nHbfPerTf, std::unique_ptr<GPUO2InterfaceConfiguration>* pConfiguration, std::unique_ptr<GPUSettingsO2>* pO2Settings, bool* autoMaxTimeBin)
{
  return std::move(getFullParam(solenoidBz, nHbfPerTf, pConfiguration, pO2Settings, autoMaxTimeBin));
}

void GPUO2InterfaceUtils::paramUseExternalOccupancyMap(GPUParam* param, uint32_t nHbfPerTf, const uint32_t* occupancymap, int32_t occupancyMapSize)
{
  size_t expectedOccMapSize = nHbfPerTf ? GPUO2InterfaceRefit::fillOccupancyMapGetSize(nHbfPerTf, param) : 0;
  if (occupancyMapSize != -1 && nHbfPerTf && (size_t)occupancyMapSize != expectedOccMapSize) {
    throw std::runtime_error("Received occupancy map of wrong size, most likely --configKeyValues or HBperTF of map creator and map consumer are different");
  }
  if (occupancymap && occupancyMapSize > sizeof(*occupancymap) && occupancymap[1] != (param->rec.tpc.occupancyMapTimeBins * 0x10000 + param->rec.tpc.occupancyMapTimeBinsAverage)) {
    throw std::runtime_error("Occupancy map has invalid paramters occupancyMapTimeBins and occupancyMapTimeBinsAverage");
  }
  if (occupancymap) {
    param->occupancyTotal = *occupancymap;
    if (param->rec.tpc.occupancyMapTimeBins) {
      param->occupancyMap = occupancymap + 2;
    }
  }
}

uint32_t GPUO2InterfaceUtils::getTpcMaxTimeBinFromNHbf(uint32_t nHbf)
{
  return (nHbf * o2::constants::lhc::LHCMaxBunches + 2 * o2::tpc::constants::LHCBCPERTIMEBIN - 2) / o2::tpc::constants::LHCBCPERTIMEBIN;
}
