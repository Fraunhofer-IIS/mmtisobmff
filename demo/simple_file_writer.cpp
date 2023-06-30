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
 * Content: Simple File Writer Demo
 */

// System includes
#include <string>
#include <memory>
#include <vector>
#include <iostream>

// External includes
#include "ilo/memory.h"

// Project includes
#include "mmtisobmff/types.h"
#include "mmtisobmff/logging.h"
#include "mmtisobmff/reader/input.h"
#include "mmtisobmff/reader/reader.h"
#include "mmtisobmff/reader/trackreader.h"
#include "mmtisobmff/writer/writer.h"
#include "mmtisobmff/writer/trackwriter.h"

using namespace mmt::isobmff;

void process(const std::string& inputFileUri, std::string& outputPath) {
  // Create a file input from the given uri and use it to create the reader
  CIsobmffReader reader(ilo::make_unique<CIsobmffFileInput>(inputFileUri));

  std::unique_ptr<CMpeghTrackReader> mpeghTrackReader = nullptr;
  std::unique_ptr<CAvcTrackReader> avcTrackReader = nullptr;

  CTrackInfo mpeghTrackInfo;
  CTrackInfo avcTrackInfo;

  // Get the trackInfo for MPEG-H and AVC
  // Initialize the trackReaders with the specific trackIndex
  for (const CTrackInfo& trackInfo : reader.trackInfos()) {
    if (trackInfo.codec == Codec::mpegh_mhm) {
      mpeghTrackInfo = trackInfo;
      mpeghTrackReader = reader.trackByIndex<CMpeghTrackReader>(trackInfo.trackIndex);
    } else if (trackInfo.codec == Codec::avc) {
      avcTrackInfo = trackInfo;
      avcTrackReader = reader.trackByIndex<CAvcTrackReader>(trackInfo.trackIndex);
    }
  }

  if (!mpeghTrackReader) {
    throw std::runtime_error("MPEG-H track reader could not be created!");
  }

  if (!avcTrackReader) {
    throw std::runtime_error("AVC track reader could not be created!");
  }

  // Adjust MPEG-H configuration
  SMpeghMhm1TrackConfig mpeghConfig;
  mpeghConfig.mediaTimescale = mpeghTrackInfo.timescale;
  mpeghConfig.sampleRate = mpeghTrackReader->sampleRate();

  // Configure the output
  CIsobmffFileWriter::SOutputConfig outputConfig;
  outputConfig.outputUri = outputPath;
  // Optional: Path to tmp file. If not set, a unique tmp file
  //           will be generated in system specific tmp dir.
  outputConfig.tmpUri = "";

  SMovieConfig movieConfig;
  // Configure the movieConfig as needed
  movieConfig.majorBrand = ilo::toFcc("mp42");
  movieConfig.compatibleBrands = std::vector<ilo::Fourcc>{ilo::toFcc("mp42")};

  // Create a non-fragmented (plain) MP4 file writer
  std::unique_ptr<CIsobmffFileWriter> fileWriter =
      ilo::make_unique<CIsobmffFileWriter>(outputConfig, movieConfig);

  // Create MPEG-H track writer
  std::unique_ptr<CMpeghTrackWriter> mpeghTrackWriter =
      fileWriter->trackWriter<CMpeghTrackWriter>(mpeghConfig);

  std::unique_ptr<config::CAvcDecoderConfigRecord> trackAvcConfigRecord =
      avcTrackReader->avcDecoderConfigRecord();
  if (!trackAvcConfigRecord) {
    throw std::runtime_error("AVC Config Record could not be received!");
  }

  // Fill the AVC config record (in this example with data read from the input file)
  SAvcTrackConfig avcConfig;
  avcConfig.mediaTimescale = avcTrackInfo.timescale;
  avcConfig.height = avcTrackReader->height();
  avcConfig.width = avcTrackReader->width();
  avcConfig.configRecord = ilo::make_unique<config::CAvcDecoderConfigRecord>();
  avcConfig.configRecord->setConfigurationVersion(trackAvcConfigRecord->configurationVersion());
  avcConfig.configRecord->setAvcProfileIndication(trackAvcConfigRecord->avcProfileIndication());
  avcConfig.configRecord->setProfileCompatibility(trackAvcConfigRecord->profileCompatibility());
  avcConfig.configRecord->setAvcLevelIndication(trackAvcConfigRecord->avcLevelIndication());
  avcConfig.configRecord->setLengthSizeMinusOne(trackAvcConfigRecord->lengthSizeMinusOne());
  avcConfig.configRecord->setChromaFormat(trackAvcConfigRecord->chromaFormat());
  avcConfig.configRecord->setBitDepthLumaMinus8(trackAvcConfigRecord->bitDepthLumaMinus8());
  avcConfig.configRecord->setBitDepthChromaMinus8(trackAvcConfigRecord->bitDepthChromaMinus8());
  avcConfig.configRecord->setSequenceParameterSets(trackAvcConfigRecord->sequenceParameterSets());
  avcConfig.configRecord->setPictureParameterSets(trackAvcConfigRecord->pictureParameterSets());
  avcConfig.configRecord->setSequenceParameterExtSets(
      trackAvcConfigRecord->sequenceParameterExtSets());

  // Create AVC track writer
  std::unique_ptr<CAvcTrackWriter> avcTrackWriter =
      fileWriter->trackWriter<CAvcTrackWriter>(avcConfig);

  // Pre-Allocate the sample with max sample size to avoid re-allocation of memory.
  // Sample can be re-used for each nextSample call.
  CSample sample = CSample(mpeghTrackInfo.maxSampleSize);

  // Get all MPEG-H samples in order. Each call fetches the next sample and writes it immediately to
  // file.
  mpeghTrackReader->nextSample(sample);
  while (!sample.empty()) {
    // In this file to file example the read sample can be directly added again
    // Otherwise the created sample needs to be filled with data manually before calling addSample()
    // sample.clear() needs to be called before new data can be filled into the sample
    mpeghTrackWriter->addSample(sample);
    sample.clear();
    mpeghTrackReader->nextSample(sample);
  }

  // Pre-Allocate the sample with max sample size to avoid re-allocation of memory.
  // Sample can be re-used for each nextSample call.
  SAvcSample avcSample = SAvcSample(avcTrackInfo.maxSampleSize);

  // Get all AVC samples in order. Each call fetches the next sample and writes it immediately to
  // file.
  avcTrackReader->nextSample(avcSample);
  while (!avcSample.empty()) {
    // The below mentioned interface is most useful for mp4 file re-packing.
    // Video samples are stored differently in mp4 files than audio.
    // The SAvcSample gives access to separate video nalus.
    // When writing mp4 files from a different source (e.g. a video encoder)
    // the user should make use of the function "void addSample(const SAvcNalus& nalus)"
    // The SAvcNalu supports AnnexB and non AnnexB video nalus and the addSample function
    // takes care of converting them into isobmff format

    // In this file to file example the read sample can be directly added again
    // Otherwise the created sample needs to be filled with data manually before calling addSample()
    // sample.clear() needs to be called before new data can be filled into the sample
    avcTrackWriter->addSample(avcSample);
    avcSample.clear();
    avcTrackReader->nextSample(avcSample);
  }

  // Finish the file, delete temp files, close the file library
  fileWriter->close();
}

int main(int argc, char** argv) {
  // Configure to your liking (logging to file, system, console or disable)
  disableLogging();

  if (argc != 3) {
    std::cout << "Usage: simpleFileWriter <FileInputUri> <FileOutputPath>" << std::endl;
    return 1;
  }

  std::string inputFileUri = std::string(argv[1]);
  std::string outputPath = std::string(argv[2]);

  try {
    process(inputFileUri, outputPath);
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
