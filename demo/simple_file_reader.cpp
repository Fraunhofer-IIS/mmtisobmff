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
 * Content: simple MP4 file reader demo
 */

// System includes
#include <string>
#include <iostream>

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
#include "mmtisobmff/specificboxinfo.h"

using namespace mmt::isobmff;

void process(const std::string& fileUri) {
  std::cout << "Reading from input file: " << fileUri << std::endl;
  std::cout << std::endl;

  // Create a file input from the given uri and use it to create the reader
  CIsobmffReader reader(ilo::make_unique<CIsobmffFileInput>(fileUri));

  std::unique_ptr<SIodsInfo> iodsInfo = reader.specificBoxInfo<SIodsInfo>();

  if (iodsInfo->iodsInfoAvailable()) {
    std::cout << "Iods - Audio Profile Level Indication: "
              << static_cast<uint16_t>(iodsInfo->audioProfileLevelIndication()) << std::endl;
    std::cout << std::endl;
  }

  // Getting some information about the available tracks
  std::cout << "Found " << reader.trackCount() << " tracks in input file." << std::endl;

  for (const auto& trackInfo : reader.trackInfos()) {
    std::cout << "########################################" << std::endl;
    std::cout << "-TrackInfo: " << std::endl;
    std::cout << "-- ID       : " << trackInfo.trackId << std::endl;
    std::cout << "-- Handler  : " << ilo::toString(trackInfo.handler) << std::endl;
    std::cout << "-- Type     : " << tools::trackTypeToString(trackInfo.type) << std::endl;
    std::cout << "-- Codec    : " << ilo::toString(trackInfo.codingName) << std::endl;
    std::cout << "-- Duration : " << trackInfo.duration << std::endl;
    std::cout << "-- Timescale: " << trackInfo.timescale << std::endl;

    std::cout << std::endl;
    std::cout << "Creating reader for track with ID " << trackInfo.trackId << " ... ";

    // Create a generic track reader for track number i
    std::unique_ptr<CGenericTrackReader> trackReader =
        reader.trackByIndex<CGenericTrackReader>(trackInfo.trackIndex);

    if (trackReader == nullptr) {
      std::cout << "Error: Track reader could not be created!" << std::endl;
      continue;
    } else {
      std::cout << "Done!" << std::endl;
    }

    // Get decoder config record blob
    ilo::ByteBuffer dcr = trackReader->decoderConfigRecord();

    std::cout << "Decoder Config Record available?: " << (dcr.size() ? "Yes" : "No") << std::endl;
    std::cout << std::endl;

    std::cout << "Sample Info:" << std::endl;
    std::cout << "########################################" << std::endl;
    std::cout << "Max Sample Size        : " << trackInfo.maxSampleSize << " Bytes" << std::endl;
    std::cout << "Total number of samples: " << trackInfo.sampleCount << std::endl;
    std::cout << std::endl;

    std::cout << "Reading all samples of this track" << std::endl;
    std::cout << "########################################" << std::endl;

    // Preallocate the sample with max sample size to avoid reallocation of memory.
    // Sample can be re-used for each nextSample call.
    CSample sample = CSample(trackInfo.maxSampleSize);

    uint64_t sampleCounter = 0;

    // Get all samples in order. Each call fetches the next sample.
    trackReader->nextSample(sample);
    while (!sample.empty()) {
      sampleCounter++;

      std::cout << "Samples processed: " << sampleCounter << "\r" << std::flush;
      trackReader->nextSample(sample);
    }

    std::cout << std::endl;
  }
}

int main(int argc, char** argv) {
  // Configure to your liking (logging to file, system, console or disable)
  disableLogging();

  if (argc != 2) {
    std::cout << "Usage: simpleFileReader <File-URI>" << std::endl;
    return 1;
  }

  std::string fileUri = std::string(argv[1]);

  try {
    process(fileUri);
  } catch (const std::exception& e) {
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
