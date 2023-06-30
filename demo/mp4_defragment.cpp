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
 * Content: tool to convert a fragmented mp4 file into a plain one
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
#include "mmtisobmff/reader/trackreader.h"
#include "mmtisobmff/writer/trackwriter.h"
#include "mmtisobmff/writer/writer.h"
#include "mmtisobmff/helper/printhelpertools.h"
#include "mmtisobmff/helper/commonhelpertools.h"

// Logging
#define LOG_COMPONENT "mp4Defragment"
#include "ilo/logging.h"

using namespace ilo;
using namespace mmt::isobmff;

int main(int argc, char** argv) {
  if (argc != 3) {
    std::cout << "Usage: mp4Defragment <InputFile-URI> <OutputFile-URI>" << std::endl;
    return 1;
  }

  redirectLoggingToSystemLogger();

  std::string inputFileUri = std::string(argv[1]);
  std::string outputFileUri = std::string(argv[2]);
  std::cout << "Reading from input file: " << inputFileUri << std::endl;
  std::cout << std::endl;

  try {
    // Create a file input from the given uri and use it to create the reader
    CIsobmffReader reader(ilo::make_unique<CIsobmffFileInput>(inputFileUri));
    const auto movieInfo = reader.movieInfo();

    // Getting some information about the available tracks
    std::cout << "Found " << reader.trackCount() << " track(s) in input file." << std::endl;
    ILO_ASSERT(reader.trackCount() <= 1,
               "Files with more than one track are currently not supported");

    // Writer Configuration
    SMovieConfig movieConfig;
    // Default values for the compatible brands and the major brand
    movieConfig.compatibleBrands = {toFcc("mp42"), toFcc("isom")};
    movieConfig.majorBrand = toFcc("mp42");

    CIsobmffFileWriter::SOutputConfig outConf;
    outConf.outputUri = outputFileUri;

    CIsobmffFileWriter writer(outConf, movieConfig);

    // Copy tracks
    for (auto& trackInfo : reader.trackInfos()) {
      std::cout << tools::printTrackInfo(trackInfo);
      std::cout << "Copying Track: " << std::flush;
      tools::SCopyConfig cConfig;
      cConfig.keepFragNumber = false;
      cConfig.fragmentDuration = 0;
      cConfig.trackInfo = trackInfo;
      cConfig.oldMovieTimescale = movieInfo.timeScale;
      cConfig.newMovieTimescale = movieConfig.movieTimeScale;
      tools::copyTrack(reader, writer, cConfig);
      std::cout << "done" << std::endl << std::endl;
    }
    std::cout << "Writing output file: " << std::flush;
  } catch (std::exception& e) {
    std::cout << std::endl << "ERROR: " << e.what() << std::endl;
    return 1;
  } catch (...) {
    std::cout << std::endl
              << "ERROR: An unknown error happened. The program will exit now." << std::endl;
    return 1;
  }

  std::cout << "Done" << std::endl << std::endl;
  std::cout << "Defragmentation finished!" << std::endl << std::endl;

  return 0;
}
