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
 * Content: tool to combine tracks from several mp4 files into one
 */

// System includes
#include <string>
#include <iostream>
#include <exception>
#include <memory>
#include <numeric>

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
#define LOG_COMPONENT "mp4Combine"
#include "ilo/logging.h"

using namespace ilo;
using namespace mmt::isobmff;

using TrackIndexList = std::vector<size_t>;

struct SInputParam {
  std::string inputFileUri;
  TrackIndexList trackIndices;
};

struct SCommandlineParams {
  std::string outFileUri;
  std::vector<SInputParam> inputParams;

  void verify() {
    ILO_ASSERT_WITH(!outFileUri.empty(), std::invalid_argument, "Output file uri must be set.");
    ILO_ASSERT_WITH(!inputParams.empty(), std::invalid_argument, "Input file uri must be set.");
  }
};

void printUsage() {
  std::cout << "Usage:" << std::endl;
  std::cout << std::endl;
  std::cout << "mp4Combine -if <Input1-URI> [-t <trackIndex0> ... -t <trackIndexN>] ... -if "
               "<InputM-URI> [-t <trackIndex0> ... -t <trackIndexN>] -of <OutputFile-URI>"
            << std::endl;
  std::cout << std::endl;
  std::cout << "Arguments:" << std::endl;
  std::cout << std::endl;
  std::cout
      << "-if   URI of an input mp4 file. User can specify an arbitrary number of input file URIs"
      << std::endl;
  std::cout << "-t    The index of the track(s) in the specified input file (0-based). If the "
               "index is not explicitely specified, all tracks of the input file are used"
            << std::endl;
  std::cout << "-of   URI of the mp4 out file." << std::endl;
  std::cout << std::endl;
  std::cout << "Example:" << std::endl;
  std::cout << std::endl;
  std::cout
      << "mp4Combine -if input1.mp4 -t 0 -t 2 -if input2.mp4 -if input3.mp4 -t 1 -of output.mp4"
      << std::endl;
  std::cout << std::endl;
}

SCommandlineParams parseCommanline(int argc, char** argv) {
  SCommandlineParams cmdLineParams;
  bool collecting = false;

  for (int i = 1; i < argc; i++) {
    if (std::string(argv[i]) == "-if" && i + 1 < argc) {
      collecting = true;
      SInputParam param;
      param.inputFileUri = std::string(argv[++i]);
      cmdLineParams.inputParams.push_back(param);
    } else if (std::string(argv[i]) == "-of" && i + 1 < argc) {
      collecting = false;
      cmdLineParams.outFileUri = std::string(argv[++i]);
    } else if (std::string(argv[i]) == "-t" && i + 1 < argc) {
      ILO_ASSERT_WITH(collecting && !cmdLineParams.inputParams.empty(), std::invalid_argument,
                      "-t found, but not after a valid -if statement.");
      cmdLineParams.inputParams.back().trackIndices.push_back(
          static_cast<size_t>(std::stoul(argv[++i])));
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

TrackIndexList createIndexList(const TrackIndexList& trackIndices,
                               const CTrackInfoVec& trackInfoVec) {
  if (trackInfoVec.empty()) {
    throw std::runtime_error("Input file does not contain any tracks");
  }

  // Verify if user specified indices are in range of the avaiable ones
  for (auto& tIndex : trackIndices) {
    ILO_ASSERT_WITH(
        tIndex < trackInfoVec.size(), std::invalid_argument,
        "Selected track index (zero based) of %d is not in range of available tracks (%d)", tIndex,
        trackInfoVec.size());
  }

  if (!trackIndices.empty()) {
    return trackIndices;
  }

  TrackIndexList tIndices(trackInfoVec.size());
  std::iota(tIndices.begin(), tIndices.end(), 0);

  return tIndices;
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
    movieConfig.compatibleBrands = {toFcc("mp42")};
    movieConfig.majorBrand = toFcc("mp42");

    CIsobmffFileWriter::SOutputConfig outConf;
    outConf.outputUri = cmdlArgs.outFileUri;

    CIsobmffFileWriter writer(outConf, movieConfig);

    std::cout << std::endl << "Writing to output file: " << cmdlArgs.outFileUri << std::endl;

    for (auto& input : cmdlArgs.inputParams) {
      std::cout << std::endl << "======================================= " << std::endl;
      std::cout << "Reading from input file: " << input.inputFileUri << std::endl;
      std::cout << std::endl;

      // Create a file input from the given uri and use it to create the reader
      CIsobmffReader reader(ilo::make_unique<CIsobmffFileInput>(input.inputFileUri));
      const auto movieInfo = reader.movieInfo();

      std::cout << "The selected track info(s):" << std::endl;

      // Create index list of tracks to be copied from this input file
      auto trackIndices = createIndexList(input.trackIndices, reader.trackInfos());

      // Copy tracks
      for (auto& index : trackIndices) {
        auto trackInfo = reader.trackInfos()[index];
        std::cout << tools::printTrackInfo(trackInfo);
        std::cout << "Copying Track: " << std::flush;
        tools::SCopyConfig cConfig;
        cConfig.keepFragNumber = false;
        cConfig.fragmentDuration = 0;
        cConfig.oldMovieTimescale = movieInfo.timeScale;
        cConfig.newMovieTimescale = movieConfig.movieTimeScale;
        cConfig.trackInfo = trackInfo;
        tools::copyTrack(reader, writer, cConfig);
        std::cout << "done" << std::endl << std::endl;
      }
    }
    std::cout << "Writing output file: " << std::flush;
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
  std::cout << "Combining finished!" << std::endl << std::endl;
  return 0;
}
