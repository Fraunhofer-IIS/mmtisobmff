/*-----------------------------------------------------------------------------
Software License for The Fraunhofer FDK MPEG-H Software

Copyright (c) 2016 - 2023 Fraunhofer-Gesellschaft zur Förderung der angewandten
Forschung e.V. and Contributors
All rights reserved.

1. INTRODUCTION

The "Fraunhofer FDK MPEG-H Software" is software that implements the ISO/MPEG
MPEG-H 3D Audio standard for digital audio or related system features. Patent
licenses for necessary patent claims for the Fraunhofer FDK MPEG-H Software
(including those of Fraunhofer), for the use in commercial products and
services, may be obtained from the respective patent owners individually and/or
from Via LA (www.via-la.com).

Fraunhofer supports the development of MPEG-H products and services by offering
additional software, documentation, and technical advice. In addition, it
operates the MPEG-H Trademark Program to ease interoperability testing of end-
products. Please visit www.mpegh.com for more information.

2. COPYRIGHT LICENSE

Redistribution and use in source and binary forms, with or without modification,
are permitted without payment of copyright license fees provided that you
satisfy the following conditions:

* You must retain the complete text of this software license in redistributions
of the Fraunhofer FDK MPEG-H Software or your modifications thereto in source
code form.

* You must retain the complete text of this software license in the
documentation and/or other materials provided with redistributions of
the Fraunhofer FDK MPEG-H Software or your modifications thereto in binary form.
You must make available free of charge copies of the complete source code of
the Fraunhofer FDK MPEG-H Software and your modifications thereto to recipients
of copies in binary form.

* The name of Fraunhofer may not be used to endorse or promote products derived
from the Fraunhofer FDK MPEG-H Software without prior written permission.

* You may not charge copyright license fees for anyone to use, copy or
distribute the Fraunhofer FDK MPEG-H Software or your modifications thereto.

* Your modified versions of the Fraunhofer FDK MPEG-H Software must carry
prominent notices stating that you changed the software and the date of any
change. For modified versions of the Fraunhofer FDK MPEG-H Software, the term
"Fraunhofer FDK MPEG-H Software" must be replaced by the term "Third-Party
Modified Version of the Fraunhofer FDK MPEG-H Software".

3. No PATENT LICENSE

NO EXPRESS OR IMPLIED LICENSES TO ANY PATENT CLAIMS, including without
limitation the patents of Fraunhofer, ARE GRANTED BY THIS SOFTWARE LICENSE.
Fraunhofer provides no warranty of patent non-infringement with respect to this
software. You may use this Fraunhofer FDK MPEG-H Software or modifications
thereto only for purposes that are authorized by appropriate patent licenses.

4. DISCLAIMER

This Fraunhofer FDK MPEG-H Software is provided by Fraunhofer on behalf of the
copyright holders and contributors "AS IS" and WITHOUT ANY EXPRESS OR IMPLIED
WARRANTIES, including but not limited to the implied warranties of
merchantability and fitness for a particular purpose. IN NO EVENT SHALL THE
COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE for any direct, indirect,
incidental, special, exemplary, or consequential damages, including but not
limited to procurement of substitute goods or services; loss of use, data, or
profits, or business interruption, however caused and on any theory of
liability, whether in contract, strict liability, or tort (including
negligence), arising in any way out of the use of this software, even if
advised of the possibility of such damage.

5. CONTACT INFORMATION

Fraunhofer Institute for Integrated Circuits IIS
Attention: Division Audio and Media Technologies - MPEG-H FDK
Am Wolfsmantel 33
91058 Erlangen, Germany
www.iis.fraunhofer.de/amm
amm-info@iis.fraunhofer.de
-----------------------------------------------------------------------------*/

/*
 * Project: MPEG-4 ISO Base Media File Format (ISO BMFF) library
 * Content: advanced MP4 file reader demo
 */

// System includes
#include <string>
#include <iostream>
#include <iomanip>

// External includes
#include "ilo/memory.h"
#include "ilo/string_utils.h"

// Internal includes
#include "mmtisobmff/types.h"
#include "mmtisobmff/logging.h"
#include "mmtisobmff/reader/input.h"
#include "mmtisobmff/reader/reader.h"
#include "mmtisobmff/helper/videohelpertools.h"
#include "mmtisobmff/helper/printhelpertools.h"
#include "mmtisobmff/reader/trackreader.h"
#include "mmtisobmff/configdescriptor/mha_decoderconfigrecord.h"
#include "mmtisobmff/configdescriptor/avc_decoderconfigrecord.h"
#include "mmtisobmff/configdescriptor/hevc_decoderconfigrecord.h"
#include "mmtisobmff/configdescriptor/jxs_decoderconfigrecord.h"
#include "mmtisobmff/configdescriptor/vvc_decoderconfigrecord.h"

using namespace mmt::isobmff;

void processMpeghTrack(CIsobmffReader& reader, size_t trackNumber, size_t maxSampleSize) {
  // Create a specific track reader for track number i
  std::unique_ptr<CMpeghTrackReader> mpeghTrackReader =
      reader.trackByIndex<CMpeghTrackReader>(trackNumber);

  if (mpeghTrackReader == nullptr) {
    throw std::runtime_error("MPEGH track reader could not be created.");
  } else {
    std::cout << "Done!\n";
  }
  std::cout << std::endl;

  // Get some extra info
  std::cout << "Extra Audio Info:\n";
  std::cout << "########################################\n";
  std::cout << "MPEGH-Type: " << ilo::toString(mpeghTrackReader->codingName()) << '\n';
  std::cout << "SampleRate: " << mpeghTrackReader->sampleRate() << '\n';
  std::cout << '\n';

  // Get decoder specific config.
  std::unique_ptr<config::CMhaDecoderConfigRecord> mhaDcr =
      mpeghTrackReader->mhaDecoderConfigRecord();

  if (mhaDcr != nullptr) {
    // Get some infos about mha decoder config record
    std::cout << "Found MHA Decoder Config record:\n";
    std::cout << "########################################\n";
    std::cout << "-- ConfigurationVersion  : "
              << static_cast<uint16_t>(mhaDcr->configurationVersion()) << '\n';
    std::cout << "-- ProfileLevelIndication: "
              << static_cast<uint16_t>(mhaDcr->mpegh3daProfileLevelIndication()) << '\n';
    std::cout << "-- ReferenceChannelLayout: "
              << static_cast<uint16_t>(mhaDcr->referenceChannelLayout()) << '\n';
    std::cout << '\n';

    std::cout << "Found 1 Decoder Specific Config Entry:\n";
    std::cout << "########################################\n";
    std::cout << "-Decoder Specific Config Info:\n";
    std::cout << "-- Type: MPEGH-3daConfig\n";
    std::cout << "-- Size: " << mhaDcr->mpegh3daConfig().size() << '\n';
    std::cout << '\n';
  }

  std::cout << "Sample Info:\n";
  std::cout << "########################################\n";
  std::cout << "Max Sample Size: " << maxSampleSize << " Bytes" << '\n';
  std::cout << '\n';

  std::cout << "Reading all samples of this track\n";
  std::cout << "########################################" << std::endl;

  // Preallocate the sample with max sample size to avoid reallocation of memory.
  // Sample can be re-used for each nextSample call.
  CSample sample = CSample(maxSampleSize);

  uint64_t sampleCouter = 0;

  // Get all samples in order. Each call fetches the next sample.
  mpeghTrackReader->nextSample(sample);
  while (!sample.empty()) {
    sampleCouter++;

    std::cout << "MPEGH Samples processed: " << sampleCouter << "\r" << std::flush;

    mpeghTrackReader->nextSample(sample);
  }

  std::cout << std::endl;
}

void processAvcTrack(CIsobmffReader& reader, size_t trackNumber, size_t maxSampleSize) {
  // Create a specific track reader for track number i
  auto avcTrackReader = reader.trackByIndex<CAvcTrackReader>(trackNumber);

  if (avcTrackReader == nullptr) {
    throw std::runtime_error("H264/AVC track reader could not be created");
  } else {
    std::cout << "Done!" << '\n';
  }

  std::cout << std::endl;

  // Get some extra info
  std::cout << "Extra Video Info:" << '\n';
  std::cout << "########################################" << '\n';
  std::cout << "AVC-Type      : " << ilo::toString(avcTrackReader->codingName()) << '\n';
  std::cout << "CompressorName: " << avcTrackReader->compressorName() << '\n';
  std::cout << "Depth         : " << avcTrackReader->depth() << '\n';
  std::cout << "Width         : " << avcTrackReader->width() << '\n';
  std::cout << "Height        : " << avcTrackReader->height() << '\n';
  std::cout << '\n';

  // Get decoder specific config.
  std::unique_ptr<config::CAvcDecoderConfigRecord> avcDcr =
      avcTrackReader->avcDecoderConfigRecord();

  if (avcDcr == nullptr) {
    throw std::runtime_error("Failed to get AVC decoder config record.");
  }

  // Get some infos about avc decoder config record
  std::cout << "Found AVC Decoder Config record:" << '\n';
  std::cout << "########################################" << '\n';
  std::cout << "-- ConfigurationVersion: " << static_cast<uint16_t>(avcDcr->configurationVersion())
            << '\n';
  std::cout << "-- AvcProfileIndication: " << static_cast<uint16_t>(avcDcr->avcProfileIndication())
            << '\n';
  std::cout << "-- ProfileCompatibility: " << static_cast<uint16_t>(avcDcr->profileCompatibility())
            << '\n';
  std::cout << "-- AvcLevelIndication  : " << static_cast<uint16_t>(avcDcr->avcLevelIndication())
            << '\n';
  std::cout << "-- LengthSizeMinusOne  : " << static_cast<uint16_t>(avcDcr->lengthSizeMinusOne())
            << '\n';
  std::cout << "-- ChromaFormat        : " << static_cast<uint16_t>(avcDcr->chromaFormat()) << '\n';
  std::cout << "-- BitDepthLumaMinus8  : " << static_cast<uint16_t>(avcDcr->bitDepthLumaMinus8())
            << '\n';
  std::cout << "-- BitDepthChromaMinus8: " << static_cast<uint16_t>(avcDcr->bitDepthChromaMinus8())
            << '\n';
  std::cout << '\n';

  size_t nrOfHeaderNalus = avcDcr->sequenceParameterSets().size() +
                           avcDcr->pictureParameterSets().size() +
                           avcDcr->sequenceParameterExtSets().size();

  std::cout << "Found " << nrOfHeaderNalus << " AVC non-VCL Nalus in DCR:" << '\n';
  std::cout << "########################################" << '\n';

  // If needed, the non-Vcl nalus can be converted into AnnexB
  //(is stored in same order as in bytestream SPS, PPS, SPS_EXT)
  SAvcSample avcAnnexbSample;
  tools::convertNonVclNalusToAnnexBNalus(*avcDcr, avcAnnexbSample);

  size_t annexbVectorIndex = 0;

  for (const ilo::ByteBuffer& sps : avcDcr->sequenceParameterSets()) {
    std::cout << "-Decoder Specific Config Info: " << '\n';
    std::cout << "-- Type: H264/AVC-SPS" << '\n';
    std::cout << "-- Size: " << sps.size() << '\n';
    std::cout << "-- Size: " << avcAnnexbSample.nalus.at(annexbVectorIndex).size() << " (AnnexB)"
              << '\n';
    annexbVectorIndex++;
  }

  for (const ilo::ByteBuffer& pps : avcDcr->pictureParameterSets()) {
    std::cout << "-Decoder Specific Config Info: " << '\n';
    std::cout << "-- Type: H264/AVC-PPS" << '\n';
    std::cout << "-- Size: " << pps.size() << '\n';
    std::cout << "-- Size: " << avcAnnexbSample.nalus.at(annexbVectorIndex).size() << " (AnnexB)"
              << '\n';
    annexbVectorIndex++;
  }

  for (const ilo::ByteBuffer& spsExt : avcDcr->sequenceParameterExtSets()) {
    std::cout << "-Decoder Specific Config Info: " << '\n';
    std::cout << "-- Type: H264/AVC-SPS_Ext" << '\n';
    std::cout << "-- Size: " << spsExt.size() << '\n';
    std::cout << "-- Size: " << avcAnnexbSample.nalus.at(annexbVectorIndex).size() << " (AnnexB)"
              << '\n';
    annexbVectorIndex++;
  }
  std::cout << std::endl;

  std::cout << "Sample Info:" << '\n';
  std::cout << "########################################" << '\n';
  std::cout << "Max Sample Size: " << maxSampleSize << " Bytes" << '\n';
  std::cout << std::endl;

  std::cout << "Reading all samples of this track" << '\n';
  std::cout << "########################################" << std::endl;

  // Preallocate the sample with max sample size to avoid reallocation of memory.
  // Sample can be re-used for each nextSample call.
  SAvcSample avcSample = SAvcSample(maxSampleSize);

  uint64_t sampleCouter = 0;
  size_t videoNaluCounter = 0;

  // Get all samples in order. Each call fetches the next sample.
  avcTrackReader->nextSample(avcSample);
  while (!avcSample.empty()) {
    sampleCouter++;

    std::cout << "Samples processed: " << sampleCouter << std::flush;

    // If needed, the avc video nalus can be extracted and converted into AnnexB.
    tools::convertVideoSampleToAnnexBNalus(avcSample, avcAnnexbSample);
    videoNaluCounter += avcAnnexbSample.nalus.size();
    std::cout << " Video Nalus processed: " << videoNaluCounter << std::flush;

    std::cout << "\r" << std::flush;
    avcTrackReader->nextSample(avcSample);
  }

  std::cout << std::endl;
}

void processHevcTrack(CIsobmffReader& reader, size_t trackNumber, size_t maxSampleSize) {
  // Create a specific track reader for track number
  std::unique_ptr<CHevcTrackReader> hevcTrackReader =
      reader.trackByIndex<CHevcTrackReader>(trackNumber);

  if (hevcTrackReader == nullptr) {
    throw std::runtime_error("H265/HEVC track reader could not be created.");
  } else {
    std::cout << "Done!" << '\n';
  }

  std::cout << std::endl;

  // Get some extra info
  std::cout << "Extra Video Info:" << '\n';
  std::cout << "########################################" << '\n';
  std::cout << "HEVC-Type     : " << ilo::toString(hevcTrackReader->codingName()) << '\n';
  std::cout << "CompressorName: " << hevcTrackReader->compressorName() << '\n';
  std::cout << "Depth         : " << hevcTrackReader->depth() << '\n';
  std::cout << "Width         : " << hevcTrackReader->width() << '\n';
  std::cout << "Height        : " << hevcTrackReader->height() << '\n';
  std::cout << '\n';

  // Get decoder specific config.
  std::unique_ptr<config::CHevcDecoderConfigRecord> hevcDcr =
      hevcTrackReader->hevcDecoderConfigRecord();

  if (hevcDcr == nullptr) {
    throw std::runtime_error("Failed to get HEVC decoder config record.");
  }

  // Get some infos about hevc decoder config record
  std::cout << "Found HEVC Decoder Config record:" << '\n';
  std::cout << "########################################" << '\n';
  std::cout << "-- ConfigurationVersion     : "
            << static_cast<uint16_t>(hevcDcr->configurationVersion()) << '\n';
  std::cout << "-- GeneralProfileSpace      : "
            << static_cast<uint16_t>(hevcDcr->generalProfileSpace()) << '\n';
  std::cout << "-- GeneralTierFlag          : " << static_cast<uint16_t>(hevcDcr->generalTierFlag())
            << '\n';
  std::cout << "-- GeneralProfileIdc        : "
            << static_cast<uint16_t>(hevcDcr->generalProfileIdc()) << '\n';
  std::cout << "-- GeneralProfileCompFlags  : " << hevcDcr->generalProfileCompatabilityFlags()
            << '\n';
  std::cout << "-- GeneralConstIndicFlags   : " << hevcDcr->generalConstraintIndicatorFlags()
            << '\n';
  std::cout << "-- GeneralLevelIdc          : " << static_cast<uint16_t>(hevcDcr->generalLevelIdc())
            << '\n';
  std::cout << "-- MinSpatialSegmentationIdc: " << hevcDcr->minSpatialSegmentationIdc() << '\n';
  std::cout << "-- ParalelismType           : " << static_cast<uint16_t>(hevcDcr->paralelismType())
            << '\n';
  std::cout << "-- ChromaFormatIdc          : " << static_cast<uint16_t>(hevcDcr->chromaFormatIdc())
            << '\n';
  std::cout << "-- BitDepthLumaMinus8       : "
            << static_cast<uint16_t>(hevcDcr->bitDepthLumaMinus8()) << '\n';
  std::cout << "-- BitDepthChromaMinus8     : "
            << static_cast<uint16_t>(hevcDcr->bitDepthChromaMinus8()) << '\n';
  std::cout << "-- AvgFrameRate             : " << hevcDcr->avgFrameRate() << '\n';
  std::cout << "-- ConstFrameRate           : " << static_cast<uint16_t>(hevcDcr->constFrameRate())
            << '\n';
  std::cout << "-- NumTemporatlLayers       : "
            << static_cast<uint16_t>(hevcDcr->numTemporatlLayers()) << '\n';
  std::cout << "-- TemporalIdNested         : "
            << static_cast<uint16_t>(hevcDcr->temporalIdNested()) << '\n';
  std::cout << "-- LengthSizeMinusOne       : "
            << static_cast<uint16_t>(hevcDcr->lengthSizeMinusOne()) << '\n';
  std::cout << '\n';

  std::cout << "Found " << hevcDcr->nonVclArrays().size() << " HEVC non-VCL Nalus in DCR:" << '\n';
  std::cout << "########################################" << '\n';

  // If needed, the non-Vcl nalus can be converted into AnnexB
  //(is stored in same order as in bytestream VPS, SPS, PPS)
  SHevcSample hevcAnnexbSample;
  tools::convertNonVclNalusToAnnexBNalus(*hevcDcr, hevcAnnexbSample);

  size_t annexbVectorIndex = 0;

  for (const config::CHevcDecoderConfigRecord::SHevcArray& hevcArray : hevcDcr->nonVclArrays()) {
    std::cout << "-Decoder Specific Config Info: " << '\n';
    std::cout << "-- Nalu Type         : " << static_cast<uint16_t>(hevcArray.naluType) << '\n';
    std::cout << "-- Array Completeness: " << static_cast<uint16_t>(hevcArray.arrayCompleteness)
              << '\n';
    std::cout << "-- Number of Nalus   : " << hevcArray.nalus.size() << '\n';

    for (size_t i = 0; i < hevcArray.nalus.size(); ++i) {
      std::cout << "--- Nalu[" << i << "], Size    : " << hevcArray.nalus[i].size() << '\n';
      std::cout << "--- Nalu[" << i
                << "], Size    : " << hevcAnnexbSample.nalus.at(annexbVectorIndex).size()
                << " (AnnexB)" << '\n';
      annexbVectorIndex++;
    }
  }

  std::cout << std::endl;
  std::cout << "Reading all samples of this track" << '\n';
  std::cout << "########################################" << std::endl;

  // Preallocate the sample with max sample size to avoid reallocation of memory.
  // Sample can be re-used for each nextSample call.
  SHevcSample hevcSample = SHevcSample(maxSampleSize);

  uint64_t sampleCouter = 0;
  size_t videoNaluCounter = 0;

  // Get all samples in order. Each call fetches the next sample.
  hevcTrackReader->nextSample(hevcSample);
  while (!hevcSample.empty()) {
    sampleCouter++;

    std::cout << "Samples processed: " << sampleCouter << std::flush;

    // If needed, the hevc video nalus can be extracted and converted into AnnexB.
    tools::convertVideoSampleToAnnexBNalus(hevcSample, hevcAnnexbSample);
    videoNaluCounter += hevcAnnexbSample.nalus.size();
    std::cout << " Video Nalus processed: " << videoNaluCounter << std::flush;

    std::cout << "\r" << std::flush;
    hevcTrackReader->nextSample(hevcSample);
  }

  std::cout << std::endl;
}

void processJxsTrack(CIsobmffReader& reader, size_t trackNumber, size_t maxSampleSize) {
  std::unique_ptr<CJxsTrackReader> jxsTrackReader =
      reader.trackByIndex<CJxsTrackReader>(trackNumber);

  if (jxsTrackReader == nullptr) {
    throw std::runtime_error("JXS track reader could not be created.");
  } else {
    std::cout << "Done!" << '\n';
  }

  std::cout << std::endl;

  // Get some extra info
  std::cout << "Extra JPEG XS Info:" << '\n';
  std::cout << "########################################" << '\n';
  std::cout << "JXS-Type      : " << ilo::toString(jxsTrackReader->codingName()) << '\n';
  std::cout << "CompressorName: " << jxsTrackReader->compressorName() << '\n';
  std::cout << "Depth         : " << jxsTrackReader->depth() << '\n';
  std::cout << "Width         : " << jxsTrackReader->width() << '\n';
  std::cout << "Height        : " << jxsTrackReader->height() << '\n';
  std::cout << '\n';

  SJpegxsExtraData extraData = jxsTrackReader->jpegxsExtraData();

  std::cout << "Video Info:" << '\n';
  std::cout << "########################################" << '\n';
  std::cout << "Maximum bit rate   : " << extraData.brat << '\n';
  std::cout << "frat               : " << std::hex << std::uppercase << "0x" << std::setfill('0')
            << std::setw(8) << extraData.frat << std::dec << '\n';
  std::cout << "schar              : " << std::hex << std::uppercase << "0x" << std::setfill('0')
            << std::setw(4) << extraData.schar << std::dec << '\n';

  uint8_t hour = static_cast<uint8_t>((extraData.tcod & 0xFF000000) > 24);
  uint8_t minutes = static_cast<uint8_t>((extraData.tcod & 0x00FF0000) > 16);
  uint8_t seconds = static_cast<uint8_t>((extraData.tcod & 0x0000FF00) > 8);
  uint8_t frames = static_cast<uint8_t>(extraData.tcod & 0x000000FF);
  std::cout << "Time Code          : " << std::dec << std::uppercase << std::setfill('0')
            << std::setw(2) << static_cast<uint16_t>(hour) << " " << std::setw(2)
            << static_cast<uint16_t>(minutes) << " " << std::setw(2)
            << static_cast<uint16_t>(seconds) << " " << std::setw(2)
            << static_cast<uint16_t>(frames) << " (HH MM SS FF)" << '\n';
  std::cout << '\n';

  std::cout << "Profile Info:" << '\n';
  std::cout << "########################################" << '\n';
  std::cout << "Profile               : " << std::hex << std::uppercase << "0x" << std::setfill('0')
            << std::setw(4) << extraData.ppih << std::dec << '\n';
  std::cout << "Level                 : " << std::hex << std::uppercase << "0x" << std::setfill('0')
            << std::setw(4) << extraData.plev << std::dec << '\n';
  std::cout << '\n';

  if (extraData.colourInformations.size() >= 1) {
    for (uint16_t i = 0; i < extraData.colourInformations.size(); i++) {
      std::cout << "Colour Info #" << (i + 1) << ":" << '\n';
      std::cout << "########################################" << '\n';
      std::cout << "Colour Type          : "
                << ilo::toString(extraData.colourInformations[i].colourType) << '\n';

      if (extraData.colourInformations[i].colourType == ilo::toFcc("nclx")) {
        std::cout << "Colour Primaries          : "
                  << extraData.colourInformations[i].colourPrimaries << '\n';
        std::cout << "Transfer Characteristics  : "
                  << extraData.colourInformations[i].transferCharacteristics << '\n';
        std::cout << "Matrix Coefficients       : "
                  << extraData.colourInformations[i].matrixCoefficients << '\n';
        std::cout << "Full Range Flat           : " << extraData.colourInformations[i].fullRangeFlag
                  << '\n';
      } else if (extraData.colourInformations[i].colourType == ilo::toFcc("rICC") ||
                 extraData.colourInformations[i].colourType == ilo::toFcc("prof")) {
        std::cout << "ICC Profile          : ";
        ilo::ByteBuffer iccProfile = extraData.colourInformations[i].iccProfile;

        for (uint8_t j = 0; j < iccProfile.size(); j++) {
          if (j > 10u) {
            std::cout << " ... ";
            break;
          }
          std::cout << std::hex << std::uppercase << "0x" << std::setfill('0') << std::setw(2)
                    << static_cast<uint16_t>(iccProfile.at(j)) << std::dec << " ";
        }

        std::cout << " { length: " << iccProfile.size() << " }" << '\n';
      }
      std::cout << std::endl;
    }
  }

  CSample sample = CSample(maxSampleSize);
  uint64_t sampleCouter = 0;

  jxsTrackReader->nextSample(sample);
  while (!sample.empty()) {
    sampleCouter++;

    std::cout << "Samples processed: " << sampleCouter << "\r" << std::flush;
    jxsTrackReader->nextSample(sample);
  }

  std::cout << std::endl;
}

void processVvcTrack(CIsobmffReader& reader, size_t trackNumber, size_t maxSampleSize) {
  // Create a specific track reader for track number
  std::unique_ptr<CVvcTrackReader> vvcTrackReader =
      reader.trackByIndex<CVvcTrackReader>(trackNumber);

  if (vvcTrackReader == nullptr) {
    throw std::runtime_error("H266/VVC track reader could not be created.");
  } else {
    std::cout << "Done!" << '\n';
  }

  std::cout << std::endl;

  // Get some extra info
  std::cout << "Extra Video Info:" << '\n';
  std::cout << "########################################" << '\n';
  std::cout << "VVC-Type      : " << ilo::toString(vvcTrackReader->codingName()) << '\n';
  std::cout << "CompressorName: " << vvcTrackReader->compressorName() << '\n';
  std::cout << "Depth         : " << vvcTrackReader->depth() << '\n';
  std::cout << "Width         : " << vvcTrackReader->width() << '\n';
  std::cout << "Height        : " << vvcTrackReader->height() << '\n';
  std::cout << '\n';

  // Get decoder specific config.
  std::unique_ptr<config::CVvcDecoderConfigRecord> vvcDcr =
      vvcTrackReader->vvcDecoderConfigRecord();

  if (vvcDcr == nullptr) {
    throw std::runtime_error("Failed to get VVC decoder config record.");
  }

  // Get some infos about vvc decoder config record
  std::cout << "Found VVC Decoder Config record:" << '\n';
  std::cout << "########################################" << '\n';
  std::cout << "-- LengthSizeMinusOne        : " << std::to_string(vvcDcr->lengthSizeMinusOne())
            << '\n';
  std::cout << "-- PtlPresentFlag            : " << std::to_string(vvcDcr->vvcPtlPresent()) << '\n';

  if (vvcDcr->vvcPtlPresent()) {
    const auto& pptl = vvcDcr->vvcPtl();
    std::cout << "-- OlsIdx                    : " << std::to_string(pptl.olsIdx) << '\n';
    std::cout << "-- NumSublayers              : " << std::to_string(pptl.numSublayers) << '\n';
    std::cout << "-- ConstantFramerate         : " << std::to_string(pptl.constantFrameRate)
              << '\n';
    std::cout << "-- ChomaFromatIdc            : " << std::to_string(pptl.chromaFormatIdc) << '\n';
    std::cout << "-- BitDepthMinus8            : " << std::to_string(pptl.bitDepthMinus8) << '\n';
    std::cout << "-- ###--Start--VvcPtlRecord--###" << std::endl;
    std::cout << "-- GeneralProfileIdc         : "
              << std::to_string(pptl.nativePtl.generalProfileIdc) << '\n';
    std::cout << "-- GeneralTierFlag           : " << std::to_string(pptl.nativePtl.generalTierFlag)
              << '\n';
    std::cout << "-- GeneralLevelIdc           : " << std::to_string(pptl.nativePtl.generalLevelIdc)
              << '\n';
    std::cout << "-- PtlFrameOnlyConstaintFlag : "
              << std::to_string(pptl.nativePtl.ptlFrameOnlyConstraintFlag) << '\n';
    std::cout << "-- PtlMultilayerEnabledFlag  : "
              << std::to_string(pptl.nativePtl.ptlMultiLayerEnabledFlag) << '\n';
    std::cout << "-- GeneralConstraintInfo     : " << std::hex << std::uppercase;
    for (uint8_t value : pptl.nativePtl.generalConstraintInfo) {
      std::cout << "0x" << std::setfill('0') << std::setw(2) << std::to_string(value) << ' ';
    }
    std::cout << std::nouppercase << std::dec << '\n';

    if (pptl.nativePtl.sublayerLevelIdcs.empty()) {
      std::cout << "-- SublayerLevelIdcs         : { None }" << '\n';
    } else {
      for (const auto& ptlSubglayerFlag : pptl.nativePtl.sublayerLevelIdcs) {
        std::cout << "-- SublayerLevelIdc        : { Layer: "
                  << std::to_string(ptlSubglayerFlag.first)
                  << ", Value: " << std::to_string(ptlSubglayerFlag.second) << "}" << '\n';
      }
    }

    std::cout << "-- GeneralSubProfileIdcs     : { ";
    if (pptl.nativePtl.generalSubProfileIdcs.empty()) {
      std::cout << "None";
    } else {
      for (auto subProfileIdcs : pptl.nativePtl.generalSubProfileIdcs) {
        std::cout << subProfileIdcs << ", ";
      }
    }
    std::cout << " }" << '\n';
    std::cout << "-- ###--End---VvcPtlRecord---###" << '\n';
    std::cout << "-- MaxPictureWidth           : " << pptl.maxPictureWidth << '\n';
    std::cout << "-- MaxPictureHeight          : " << pptl.maxPictureHeight << '\n';
    std::cout << "-- AvgFrameRate              : " << pptl.avgFrameRate << '\n';
  }

  std::cout << '\n';

  std::cout << "Found " << vvcDcr->nonVclArrays().size() << " VVC non-VCL Nalus in DCR:" << '\n';
  std::cout << "########################################" << '\n';

  // If needed, the non-Vcl nalus can be converted into AnnexB
  //(is stored in same order as in bytestream VPS, SPS, PPS)
  SVvcSample vvcAnnexbSample;
  tools::convertNonVclNalusToAnnexBNalus(*vvcDcr, vvcAnnexbSample);

  size_t annexbVectorIndex = 0;

  for (const config::CVvcDecoderConfigRecord::SVvcArray& vvcArray : vvcDcr->nonVclArrays()) {
    std::cout << "-Decoder Specific Config Info: " << '\n';
    std::cout << "-- Nalu Type         : " << std::to_string(vvcArray.naluType) << '\n';
    std::cout << "-- Array Completeness: " << std::to_string(vvcArray.arrayCompleteness) << '\n';
    std::cout << "-- Number of Nalus   : " << vvcArray.nalus.size() << '\n';

    for (size_t i = 0; i < vvcArray.nalus.size(); ++i) {
      std::cout << "--- Nalu[" << i << "], Size    : " << vvcArray.nalus[i].size() << '\n';
      std::cout << "--- Nalu[" << i
                << "], Size    : " << vvcAnnexbSample.nalus.at(annexbVectorIndex).size()
                << " (AnnexB)" << '\n';
      annexbVectorIndex++;
    }
  }

  std::cout << '\n';
  std::cout << "Reading all samples of this track" << '\n';
  std::cout << "########################################" << std::endl;

  // Preallocate the sample with max sample size to avoid reallocation of memory.
  // Sample can be re-used for each nextSample call.
  SVvcSample vvcSample = SVvcSample(maxSampleSize);

  uint64_t sampleCouter = 0;
  size_t videoNaluCounter = 0;

  // Get all samples in order. Each call fetches the next sample.
  vvcTrackReader->nextSample(vvcSample);
  while (!vvcSample.empty()) {
    sampleCouter++;

    std::cout << "Samples processed: " << sampleCouter << std::flush;

    // If needed, the vvc video nalus can be extracted and converted into AnnexB.
    tools::convertVideoSampleToAnnexBNalus(vvcSample, vvcAnnexbSample);
    videoNaluCounter += vvcAnnexbSample.nalus.size();
    std::cout << " Video Nalus processed: " << videoNaluCounter << std::flush;

    std::cout << "\r" << std::flush;
    vvcTrackReader->nextSample(vvcSample);
  }

  std::cout << std::endl;
}

void processGenericTrack(CIsobmffReader& reader, size_t trackNumber, size_t maxSampleSize) {
  // Create a generic track reader for track number i
  std::unique_ptr<CGenericTrackReader> trackReader =
      reader.trackByIndex<CGenericTrackReader>(trackNumber);

  if (trackReader == nullptr) {
    throw std::runtime_error("Generic Track reader could not be created.");
  } else {
    std::cout << "Done!" << '\n';
  }

  // Get decoder config record blob
  auto dcr = trackReader->decoderConfigRecord();

  std::cout << "Decoder Config Record available?: " << (dcr.size() ? "Yes" : "No") << '\n';
  std::cout << '\n';

  std::cout << "Sample Info:" << std::endl;
  std::cout << "########################################" << std::endl;
  std::cout << "Max Sample Size: " << maxSampleSize << " Bytes" << '\n';
  std::cout << '\n';

  std::cout << "Reading all samples of this track" << std::endl;
  std::cout << "########################################" << std::endl;

  // Preallocate the sample with max sample size to avoid reallocation of memory.
  // Sample can be re-used for each nextSample call.
  CSample sample = CSample(maxSampleSize);

  uint64_t sampleCouter = 0;

  // Get all samples in order. Each call fetches the next sample.
  trackReader->nextSample(sample);
  while (!sample.empty()) {
    sampleCouter++;

    std::cout << "Samples processed: " << sampleCouter << "\r" << std::flush;
    trackReader->nextSample(sample);
  }

  std::cout << std::endl;
}

void process(const std::string& fileUri) {
  std::cout << "Reading from input file: " << fileUri << '\n';
  std::cout << std::endl;

  // Create a file input from the given uri and use it to create the reader
  CIsobmffReader reader(ilo::make_unique<CIsobmffFileInput>(fileUri));

  // Getting some information about the available tracks
  std::cout << "Found " << reader.trackCount() << " tracks in input file." << std::endl;

  for (const CTrackInfo& trackInfo : reader.trackInfos()) {
    std::cout << "########################################" << std::endl;
    std::cout << "-TrackInfo: " << '\n';
    std::cout << "-- ID             : " << trackInfo.trackId << '\n';
    std::cout << "-- Handler        : " << ilo::toString(trackInfo.handler) << '\n';
    std::cout << "-- Type           : " << tools::trackTypeToString(trackInfo.type) << '\n';
    std::cout << "-- Codec          : " << ilo::toString(trackInfo.codingName) << '\n';
    std::cout << "-- Duration       : " << trackInfo.duration << '\n';
    std::cout << "-- Timescale      : " << trackInfo.timescale << '\n';
    std::cout << "-- Max Sample Size: " << trackInfo.maxSampleSize << '\n';

    std::cout << '\n';
    std::cout << "Creating reader for track with ID " << trackInfo.trackId << " ... ";

    switch (trackInfo.codec) {
      case Codec::mpegh_mha:
      case Codec::mpegh_mhm:
        processMpeghTrack(reader, trackInfo.trackIndex, trackInfo.maxSampleSize);
        break;
      case Codec::avc:
        processAvcTrack(reader, trackInfo.trackIndex, trackInfo.maxSampleSize);
        break;
      case Codec::hevc:
        processHevcTrack(reader, trackInfo.trackIndex, trackInfo.maxSampleSize);
        break;
      case Codec::jxs:
        processJxsTrack(reader, trackInfo.trackIndex, trackInfo.maxSampleSize);
        break;
      case Codec::vvc:
        processVvcTrack(reader, trackInfo.trackIndex, trackInfo.maxSampleSize);
        break;
      default:
        processGenericTrack(reader, trackInfo.trackIndex, trackInfo.maxSampleSize);
        break;
    }
  }
}

int main(int argc, char** argv) {
  // Configure to your liking (logging to file, system, console or disable)
  disableLogging();

  if (argc != 2) {
    std::cout << "Usage: advancedFileReader <File-URI>" << std::endl;
    return 1;
  }

  std::string fileUri = std::string(argv[1]);

  try {
    process(fileUri);
  } catch (std::exception& e) {
    std::cout << std::endl << "Error: " << e.what() << std::endl << std::endl;
    return 1;
  } catch (...) {
    std::cout << std::endl
              << "Error: An unknown error happened. The program will exit now." << std::endl
              << std::endl;
    return 1;
  }

  return 0;
}
