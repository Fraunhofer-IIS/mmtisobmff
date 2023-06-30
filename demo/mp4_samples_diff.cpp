/*-----------------------------------------------------------------------------
Software License for The Fraunhofer FDK MPEG-H Software

Copyright (c) 2017 - 2023 Fraunhofer-Gesellschaft zur Förderung der angewandten
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
 * Content: tool to compare the samples of two MP4 files
 */

// System includes
#include <string>
#include <iostream>

// External includes
#include "ilo/memory.h"

// Internal includes
#include "mmtisobmff/types.h"
#include "mmtisobmff/logging.h"
#include "common/logging.h"
#include "mmtisobmff/reader/input.h"
#include "mmtisobmff/reader/reader.h"
#include "mmtisobmff/helper/videohelpertools.h"
#include "mmtisobmff/helper/printhelpertools.h"
#include "mmtisobmff/reader/trackreader.h"

int main(int argc, char** argv) {
  using namespace ilo;
  using namespace mmt::isobmff;

  if (argc != 3) {
    std::cout << "Usage: mp4SamplesDiff <File-URI> <File2-URI>" << std::endl;
    return 1;
  }

  redirectLoggingToFile("mp4SampleDiff.log", RedirectMode::overwrite);

  std::string fileUri = std::string(argv[1]);
  std::string fileUri2 = std::string(argv[2]);

  std::cout << "Reading from input file: " << fileUri << " and " << fileUri2 << std::endl;
  std::cout << std::endl;

  // Create a file input from the given uri and use it to create the reader
  CIsobmffReader reader(ilo::make_unique<CIsobmffFileInput>(fileUri));
  CIsobmffReader reader2(ilo::make_unique<CIsobmffFileInput>(fileUri2));

  std::cout << "Found " << reader.trackCount() << " tracks in the first input file"
            << "and " << reader2.trackCount() << " tracks in the second input file" << std::endl;

  ILO_ASSERT(reader.trackCount() == reader2.trackCount(),
             "the two files have a different number of tracks");

  for (uint8_t tracks = 0; tracks < reader.trackCount(); tracks++) {
    std::cout << "########################################" << std::endl;
    std::cout << "########################################" << std::endl;
    std::cout << "-TrackInfo for the two files: " << std::endl;
    std::cout << "-- IDs       : " << reader.trackInfos()[tracks].trackId << ", "
              << reader2.trackInfos()[tracks].trackId << std::endl;
    std::cout << "-- Handlers  : " << ilo::toString(reader.trackInfos()[tracks].handler) << ", "
              << ilo::toString(reader2.trackInfos()[tracks].handler) << std::endl;
    std::cout << "-- Types     : " << tools::trackTypeToString(reader.trackInfos()[tracks].type)
              << ", " << tools::trackTypeToString(reader2.trackInfos()[tracks].type) << std::endl;
    std::cout << "-- Codecs    : " << ilo::toString(reader.trackInfos()[tracks].codingName) << ", "
              << ilo::toString(reader2.trackInfos()[tracks].codingName) << std::endl;
    std::cout << "-- Durations : " << reader.trackInfos()[tracks].duration << ", "
              << reader2.trackInfos()[tracks].duration << std::endl;
    std::cout << "-- Timescales: " << reader.trackInfos()[tracks].timescale << ", "
              << reader2.trackInfos()[tracks].timescale << std::endl;

    std::cout << std::endl;
    std::cout << "Creating reader for track with ID" << reader.trackInfos()[tracks].trackId
              << " for the first file"
              << " ... ";

    // Create a generic track reader for track number i
    auto trackReader =
        reader.trackByIndex<CGenericTrackReader>(reader.trackInfos()[tracks].trackIndex);
    if (trackReader == nullptr) {
      std::cout << "Error: Track reader could not be created for the first file!" << std::endl;
      continue;
    } else {
      std::cout << "Done!" << std::endl;
    }

    std::cout << "Creating reader for track with ID" << reader.trackInfos()[tracks].trackId
              << " for the second file"
              << " ... ";
    auto trackReader2 =
        reader2.trackByIndex<CGenericTrackReader>(reader2.trackInfos()[tracks].trackIndex);
    if (trackReader2 == nullptr) {
      std::cout << "Error: Track reader could not be created for the first file!" << std::endl;
      continue;
    } else {
      std::cout << "Done!" << std::endl;
    }

    // Get decoder config record blob
    auto dcr = trackReader->decoderConfigRecord();
    auto dcr2 = trackReader2->decoderConfigRecord();

    std::cout << "Decoder Config Record available for the first file?: "
              << (dcr.size() ? "Yes" : "No") << std::endl;
    std::cout << "Decoder Config Record available for the second file?: "
              << (dcr2.size() ? "Yes" : "No") << std::endl;
    std::cout << std::endl;

    std::cout << "Sample Info:" << std::endl;
    std::cout << "########################################" << std::endl;
    std::cout << "Max Sample Size for the first file: " << reader.trackInfos()[tracks].maxSampleSize
              << " Bytes" << std::endl;
    std::cout << "Max Sample Size for the second file: "
              << reader2.trackInfos()[tracks].maxSampleSize << " Bytes" << std::endl;
    std::cout << std::endl;

    std::cout << "Comparing all samples of this track of the two files" << std::endl;
    std::cout << "########################################" << std::endl;

    // Pre-Allocate the sample with max sample size to avoid re-allocation of memory.
    // Sample can be re-used for each nextSample call.
    CSample sample = CSample(reader.trackInfos()[tracks].maxSampleSize);
    CSample sample2 = CSample(reader2.trackInfos()[tracks].maxSampleSize);

    uint64_t sampleCounter = 0;

    // Get all samples in order. Each call fetches the next sample.
    trackReader->nextSample(sample);
    trackReader2->nextSample(sample2);
    bool samplesMatch = true;
    while (!sample.empty() && !sample2.empty()) {
      sampleCounter++;

      if (sample.duration != sample2.duration || sample.rawData != sample2.rawData ||
          sample.ctsOffset != sample2.ctsOffset) {
        std::cout << "samples with order " << sampleCounter << " DO NOT match!" << std::endl;
        // samples don't match
        samplesMatch = false;
      }

      std::cout << "Samples processed: " << sampleCounter << "\r" << std::flush;
      trackReader->nextSample(sample);
      trackReader2->nextSample(sample2);
    }
    std::cout << std::endl << std::endl;

    if (!sample.empty()) {
      std::cout << "The number of samples in the two files is mismatched, the first file has more "
                   "samples !"
                << std::endl
                << std::endl;
    } else if (!sample2.empty()) {
      std::cout << "The number of samples in the two files is mismatched, the second file has more "
                   "samples !"
                << std::endl
                << std::endl;
    }

    if (samplesMatch) {
      if (sampleCounter > 0) {
        std::cout << "The " << sampleCounter << " samples, which have been compared, match !"
                  << std::endl
                  << std::endl
                  << std::endl;
      } else {
        std::cout << "0 samples have been compared !" << std::endl << std::endl << std::endl;
      }
    }
  }
  return 0;
}
