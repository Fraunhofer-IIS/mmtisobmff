/*-----------------------------------------------------------------------------
Software License for The Fraunhofer FDK MPEG-H Software

Copyright (c) 2020 - 2023 Fraunhofer-Gesellschaft zur Förderung der angewandten
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
 * Content: seeking and timestamp api demo
 */

// System includes
#include <string>
#include <iostream>
#include <exception>
#include <stdexcept>
#include <algorithm>

// External includes
#include "ilo/memory.h"

// Internal includes
#include "mmtisobmff/types.h"
#include "mmtisobmff/logging.h"
#include "mmtisobmff/reader/input.h"
#include "mmtisobmff/reader/reader.h"
#include "mmtisobmff/helper/videohelpertools.h"
#include "mmtisobmff/helper/printhelpertools.h"
#include "mmtisobmff/reader/trackreader.h"

using namespace mmt::isobmff;

void printSampleData(const CSample& sample, const SSampleExtraInfo& extraInfo) {
  if (sample.empty()) {
    throw std::runtime_error("Failed to get sample.");
  }

  CIsoTimestamp timestamp = extraInfo.timestamp;
  if (!timestamp.isValid()) {
    throw std::runtime_error("Invalid PTS/DTS value found.");
  }

  std::cout << "Sample processed: "
            << "DTS:[" << timestamp.dtsValue() << "/" << timestamp.timescale() << "], PTS:["
            << timestamp.ptsValue() << "/" << timestamp.timescale() << "] " << std::endl;
}

void process(const std::string& fileUri) {
  std::cout << "Reading from input file: " << fileUri << std::endl;
  std::cout << std::endl;

  // Create a file input from the given uri and use it to create the reader
  CIsobmffReader reader(ilo::make_unique<CIsobmffFileInput>(fileUri));

  if (reader.trackCount() == 0U) {
    throw std::runtime_error("No tracks found in file.");
  }

  // Use track index 0 in this demo use case
  CTrackInfo trackInfo = reader.trackInfos()[0];

  std::cout << "Creating reader for track with ID " << trackInfo.trackId << std::endl;
  std::cout << std::endl;

  // Create a generic track reader for track number 0
  std::unique_ptr<CGenericTrackReader> trackReader =
      reader.trackByIndex<CGenericTrackReader>(trackInfo.trackIndex);
  if (!trackReader) {
    throw std::runtime_error("Track reader could not be created.");
  }

  std::cout << "Sample Info:" << std::endl;
  std::cout << "########################################" << std::endl;
  std::cout << "Max Sample Size        : " << trackInfo.maxSampleSize << " Bytes" << std::endl;
  std::cout << "Total number of samples: " << trackInfo.sampleCount << std::endl;
  std::cout << std::endl;

  if (trackInfo.sampleCount == 0U) {
    throw std::runtime_error("No samples found in this track.");
  }

  // Preallocate the sample with max sample size to avoid reallocation of memory.
  // Sample can be re-used for each nextSample call.
  CSample sample = CSample(trackInfo.maxSampleSize);

  SSampleExtraInfo extraInfo;

  //###############################################################
  //# Demo1 - Read first 10 samples and get timestamp information #
  //###############################################################

  size_t samplesToRead = std::min(trackInfo.sampleCount, size_t(10));

  std::cout << "Reading first " << samplesToRead << " samples " << std::endl;
  std::cout << "########################################" << std::endl;

  for (size_t i = 0; i < samplesToRead; ++i) {
    extraInfo = trackReader->nextSample(sample);
    printSampleData(sample, extraInfo);
  }
  std::cout << std::endl;

  //##############################################################
  //# Demo2 - Jump to a specific sample index and then read data #
  //##############################################################

  size_t readIndex = samplesToRead / 2;
  samplesToRead = samplesToRead - readIndex;

  std::cout << "Jumping to frame index " << readIndex << " and read " << samplesToRead << " samples"
            << std::endl;
  std::cout << "########################################" << std::endl;

  extraInfo = trackReader->sampleByIndex(readIndex, sample);
  printSampleData(sample, extraInfo);

  // Hint: Starting with 1, since we already read the first sample.
  for (size_t i = 1; i < samplesToRead; ++i) {
    extraInfo = trackReader->nextSample(sample);
    printSampleData(sample, extraInfo);
  }
  std::cout << std::endl;

  //###########################################################################
  //# Demo3 - Seek to the nearest SyncSample at given time and then read data #
  //###########################################################################

  // Simulate seeking to 50% of the track
  SSeekConfig seekConfig;
  seekConfig.seekPoint = CTimeDuration(trackInfo.timescale, trackInfo.duration / 2);
  seekConfig.seekMode = ESampleSeekMode::nearestSyncSample;

  std::cout << "Jumping to nearest sync sample around time point "
            << "[" << seekConfig.seekPoint.duration() << "/" << seekConfig.seekPoint.timescale()
            << "] ("
            << seekConfig.seekPoint.duration() /
                   static_cast<double>(seekConfig.seekPoint.timescale())
            << "s) and read 2 samples" << std::endl;
  std::cout << "########################################" << std::endl;

  extraInfo = trackReader->sampleByTimestamp(seekConfig, sample);
  printSampleData(sample, extraInfo);
  extraInfo = trackReader->nextSample(sample);
  printSampleData(sample, extraInfo);

  std::cout << std::endl;
}

int main(int argc, char** argv) {
  // Configure to your liking (logging to file, system, console or disable)
  disableLogging();

  if (argc != 2) {
    std::cout << "Usage: seekingAndTimestampApiDemo <File-URI>" << std::endl;
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
