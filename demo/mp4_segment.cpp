/*-----------------------------------------------------------------------------
Software License for The Fraunhofer FDK MPEG-H Software

Copyright (c) 2018 - 2023 Fraunhofer-Gesellschaft zur Förderung der angewandten
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
 * Content: tool to fragment/segment a plain mp4 file
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
#define LOG_COMPONENT "mp4Segment"
#include "ilo/logging.h"

using namespace ilo;
using namespace mmt::isobmff;

struct SCommandlineParams {
  std::string inFileUri;
  std::string outFileUri;

  void verify() {
    ILO_ASSERT_WITH(!inFileUri.empty(), std::invalid_argument, "Input file uri must be set.");
    ILO_ASSERT_WITH(!outFileUri.empty(), std::invalid_argument, "Output file uri must be set.");
  }
};

void printUsage() {
  std::cout << "Usage:" << std::endl;
  std::cout << std::endl;
  std::cout << "mp4Segment -if <InputFile-URI> -of <OutputFile-URI>" << std::endl;
  std::cout << std::endl;
  std::cout << "Arguments:" << std::endl;
  std::cout << std::endl;
  std::cout << "-if    URI of the fragmented input mp4 file." << std::endl;
  std::cout << "-of    URI of the segmented mp4 out file." << std::endl;
  std::cout << std::endl;
  std::cout << "Example:" << std::endl;
  std::cout << std::endl;
  std::cout << "mp4Segment -if inputFragment.mp4 -of outputSegment.mp4" << std::endl;
  std::cout << std::endl;
}

SCommandlineParams parseCommanline(int argc, char** argv) {
  SCommandlineParams cmdLineParams;

  for (int i = 1; i < argc; i++) {
    if (std::string(argv[i]) == "-if" && i + 1 < argc) {
      cmdLineParams.inFileUri = std::string(argv[++i]);
    } else if (std::string(argv[i]) == "-of" && i + 1 < argc) {
      cmdLineParams.outFileUri = std::string(argv[++i]);
    } else  // invalid argument or missing value for argument
    {
      std::string errorMsg;
      if (i + 1 < argc) {
        errorMsg = "Unknown argument found: " + std::string(argv[i]);
      } else {
        errorMsg = "Error: Malformed argument found. Missing at least one value for an argument.";
      }
      throw std::invalid_argument(errorMsg);
    }
  }
  cmdLineParams.verify();

  return cmdLineParams;
}

int main(int argc, char** argv) {
  if (argc <= 2) {
    printUsage();
    return 1;
  }

  try {
    redirectLoggingToSystemLogger();

    std::cout << std::endl << "Parsing command line: " << std::flush;
    auto cmdlArgs = parseCommanline(argc, argv);
    std::cout << "Done" << std::endl;

    // Writer Configuration
    SMovieConfig movieConfig;
    // Default values for the compatible brands and the major brand
    movieConfig.compatibleBrands = {toFcc("mp42"), toFcc("dash")};
    movieConfig.majorBrand = toFcc("mp42");

    SSidxConfig sidxConfig;
    sidxConfig.sapType = ESapType::SapType1;
    movieConfig.sidxConfig = ilo::make_unique<SSidxConfig>(sidxConfig);

    CIsobmffFragFileWriter::SOutputConfig outConf;
    outConf.outputUri = cmdlArgs.outFileUri;

    CIsobmffFragFileWriter writer(outConf, movieConfig);
    std::cout << std::endl << "======================================= " << std::endl;
    std::cout << "Reading from input file: " << cmdlArgs.inFileUri << std::endl;
    std::cout << std::endl;

    // Create a file input from the given uri and use it to create the reader
    CIsobmffReader reader(ilo::make_unique<CIsobmffFileInput>(cmdlArgs.inFileUri));
    const auto movieInfo = reader.movieInfo();

    std::cout << "The selected track info(s):" << std::endl;

    auto tInfos = reader.trackInfos();

    // Copy tracks
    for (auto& tInfo : tInfos) {
      std::cout << tools::printTrackInfo(tInfo);
      std::cout << "Copying Track: " << std::flush;
      tools::SCopyConfig cConfig;
      cConfig.keepFragNumber = true;
      cConfig.trackInfo = tInfo;
      cConfig.oldMovieTimescale = movieInfo.timeScale;
      cConfig.newMovieTimescale = movieConfig.movieTimeScale;
      tools::copyTrack(reader, writer, cConfig);
      std::cout << "done" << std::endl << std::endl;
    }
    std::cout << "Writing output file: " << std::flush;
    writer.createMediaFragments();
  } catch (std::exception& e) {
    std::cout << std::endl << "Error: " << e.what() << std::endl << std::endl;
    return 1;
  } catch (...) {
    std::cout << std::endl
              << "Error: An unknown error happened. The program will exit now." << std::endl
              << std::endl;
    return 1;
  }

  std::cout << "Done" << std::endl << std::endl;
  std::cout << "Segmenting finished!" << std::endl << std::endl;
  return 0;
}
