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
 * Content: tool to print mp4 box structure in a tree view
 */

// System includes
#include <string>
#include <iostream>
#include <iomanip>
#include <algorithm>

// External includes
#include "ilo/memory.h"

// Internal includes
#include "mmtisobmff/types.h"
#include "mmtisobmff/logging.h"
#include "mmtisobmff/configdescriptor/mha_decoderconfigrecord.h"
#include "mmtisobmff/configdescriptor/avc_decoderconfigrecord.h"
#include "mmtisobmff/configdescriptor/hevc_decoderconfigrecord.h"
#include "mmtisobmff/configdescriptor/mp4a_decoderconfigrecord.h"
#include "mmtisobmff/configdescriptor/jxs_decoderconfigrecord.h"
#include "mmtisobmff/configdescriptor/vvc_decoderconfigrecord.h"
#include "mmtisobmff/helper/commonhelpertools.h"
#include "mmtisobmff/helper/printhelpertools.h"
#include "tree/tree_parser.h"
#include "service/servicesingleton.h"
#include "box/containerbox.h"
#include "box/mvhdbox.h"
#include "box/tkhdbox.h"
#include "box/mdhdbox.h"
#include "box/hdlrbox.h"
#include "box/smhdbox.h"
#include "box/vmhdbox.h"
#include "box/drefbox.h"
#include "box/urlbox.h"
#include "box/stsdbox.h"
#include "box/sttsbox.h"
#include "box/stscbox.h"
#include "box/stszbox.h"
#include "box/stcobox.h"
#include "box/stssbox.h"
#include "box/cttsbox.h"
#include "box/trunbox.h"
#include "box/tfdtbox.h"
#include "box/tfhdbox.h"
#include "box/mfhdbox.h"
#include "box/ftypbox.h"
#include "box/stypbox.h"
#include "box/mmpubox.h"
#include "box/trexbox.h"
#include "box/jpvibox.h"
#include "box/jxplbox.h"
#include "box/colrbox.h"
#include "box/decoderconfigurationbox.h"
#include "box/decoderconfigurationfullbox.h"
#include "box/mp4asampleentry.h"
#include "box/mhasampleentry.h"
#include "box/mhmsampleentry.h"
#include "box/avcsampleentry.h"
#include "box/hevcsampleentry.h"
#include "box/jxssampleentry.h"
#include "box/vvcsampleentry.h"
#include "box/invalidbox.h"
#include "box/unknownbox.h"
#include "box/loudnessbox.h"
#include "box/sbgpbox.h"
#include "box/sgpdbox.h"
#include "box/iodsbox.h"
#include "box/elstbox.h"
#include "box/mhapbox.h"
#include "box/btrtbox.h"
#include "box/sidxbox.h"
#include "mmtisobmff/reader/input.h"

using namespace ilo;
using namespace mmt::isobmff;
using namespace mmt::isobmff::box;
using namespace mmt::isobmff::config;

// Disable C4709 (comma in array) warning for VS2013 triggered with W4, because of a false positive.
#if (_WIN32 && _MSC_VER == 1800)
#pragma warning(push)
#pragma warning(disable : 4709)
#endif

void extraInfo(int /*argc*/, char** argv) {
  CIsobmffReader reader(ilo::make_unique<CIsobmffFileInput>(std::string(argv[1])));

  for (const auto& trackInfo : reader.trackInfos()) {
    std::cout << "\nShortTrack Info: " << trackInfo.trackIndex << std::endl;
    std::cout << "########################################" << std::endl;
    std::cout << "-TrackIndex      : " << trackInfo.trackIndex << std::endl;
    std::cout << "-- ID            : " << trackInfo.trackId << std::endl;
    std::cout << "-- Handler       : " << ilo::toString(trackInfo.handler) << std::endl;
    std::cout << "-- Type          : " << tools::trackTypeToString(trackInfo.type) << std::endl;
    std::cout << "-- Codec         : " << ilo::toString(trackInfo.codingName) << std::endl;
    std::cout << "-- Duration      : " << trackInfo.duration << std::endl;
    std::cout << "-- Timescale     : " << trackInfo.timescale << std::endl;
    std::cout << "-- Language      : " << ilo::toString(trackInfo.language) << std::endl;
    std::cout << "-- MaxSampleSize : " << trackInfo.maxSampleSize << std::endl;
    std::cout << "-- SampleCount   : " << trackInfo.sampleCount << std::endl;
  }
}

struct BoxPrinter {
  BoxPrinter() {
    printMap[(toFcc("ftyp"))] = [&](const BoxItem& item, const std::string& treeSpaces) {
      auto box = std::dynamic_pointer_cast<CFileTypeBox>(item);
      std::cout << treeSpaces << "-- Minor Version     : " << box->minorVersion() << std::endl;
      std::cout << treeSpaces << "-- Major Brand       : " << toString(box->majorBrand())
                << std::endl;
      std::cout << treeSpaces << "-- Compatible Brands : { ";
      for (size_t i = 0; i < box->compatibleBrands().size(); ++i) {
        std::cout << toString(box->compatibleBrands()[i]);
        if (i + 1 < box->compatibleBrands().size()) {
          std::cout << ", ";
        }
      }
      std::cout << " }" << std::endl;
      return;
    };

    printMap[(toFcc("styp"))] = [&](const BoxItem& item, const std::string& treeSpaces) {
      auto box = std::dynamic_pointer_cast<CSegmentTypeBox>(item);
      std::cout << treeSpaces << "-- Minor Version     : " << box->minorVersion() << std::endl;
      std::cout << treeSpaces << "-- Major Brand       : " << toString(box->majorBrand())
                << std::endl;
      std::cout << treeSpaces << "-- Compatible Brands : { ";
      for (size_t i = 0; i < box->compatibleBrands().size(); ++i) {
        std::cout << toString(box->compatibleBrands()[i]);
        if (i + 1 < box->compatibleBrands().size()) {
          std::cout << ", ";
        }
      }
      std::cout << " }" << std::endl;
      return;
    };

    printMap[(toFcc("mmpu"))] = [&](const BoxItem& item, const std::string& treeSpaces) {
      auto box = std::dynamic_pointer_cast<CMediaProcessingUnitBox>(item);

      std::string assetIdValue = std::string(box->assetIdentifierBox().assetIdValue.begin(),
                                             box->assetIdentifierBox().assetIdValue.end());

      std::cout << treeSpaces << "-- Is Complete?        : " << box->isComplete() << std::endl;
      std::cout << treeSpaces << "-- Is Adc Present      : " << box->isAdcPresent() << std::endl;
      std::cout << treeSpaces << "-- MPU Sequence Number : " << box->mpuSequenceNumber()
                << std::endl;
      std::cout << treeSpaces
                << "-- Asset ID Scheme     : " << toString(box->assetIdentifierBox().assetIdScheme)
                << std::endl;
      std::cout << treeSpaces
                << "-- Asset ID Length     : " << box->assetIdentifierBox().assetIdLength
                << std::endl;
      std::cout << treeSpaces << "-- Asset ID Value      : " << assetIdValue << std::endl;
      return;
    };

    printMap[(toFcc("mvhd"))] = [&](const BoxItem& item, const std::string& treeSpaces) {
      auto box = std::dynamic_pointer_cast<CMovieHeaderBox>(item);

      std::cout << treeSpaces << "-- Box Version       : " << static_cast<uint32_t>(box->version())
                << std::endl;
      std::cout << treeSpaces << "-- Box Flags         : " << box->flags() << std::endl;
      std::cout << treeSpaces
                << "-- Creation Time     : " << tools::UTCTimeToString(box->creationTime())
                << std::endl;
      std::cout << treeSpaces
                << "-- Modification Time : " << tools::UTCTimeToString(box->modificationTime())
                << std::endl;
      std::cout << treeSpaces << "-- Timescale         : " << box->timescale() << std::endl;
      std::cout << treeSpaces << "-- Duration          : " << box->duration() << " ("
                << box->durationSeconds() << " s)" << std::endl;
      std::cout << treeSpaces << "-- Rate              : " << box->rate() << " (" << box->rateHR()
                << ")" << std::endl;
      std::cout << treeSpaces << "-- Volume            : " << box->volume() << " ("
                << box->volumeHR() << ")" << std::endl;
      std::cout << treeSpaces << "-- Matrix            : {";
      for (auto value : box->matrix()) {
        std::cout << " " << value;
      }
      std::cout << " }" << std::endl;
      std::cout << treeSpaces << "-- Next Track ID     : " << box->nextTrackID() << std::endl;

      timescale = box->timescale();
      return;
    };

    printMap[(toFcc("tkhd"))] = [&](const BoxItem& item, const std::string& treeSpaces) {
      auto box = std::dynamic_pointer_cast<CTrackHeaderBox>(item);

      std::cout << treeSpaces << "-- Box Version       : " << static_cast<uint32_t>(box->version())
                << std::endl;
      std::cout << treeSpaces << "-- Box Flags         : " << box->flags() << std::endl;
      std::cout << treeSpaces
                << "-- Creation Time     : " << tools::UTCTimeToString(box->creationTime())
                << std::endl;
      std::cout << treeSpaces
                << "-- Modification Time : " << tools::UTCTimeToString(box->modificationTime())
                << std::endl;
      std::cout << treeSpaces << "-- Track ID          : " << box->trackID() << std::endl;
      if (timescale) {
        std::cout << treeSpaces << "-- Duration          : " << box->duration() << " ("
                  << box->duration() / timescale << " s)" << std::endl;
      } else {
        std::cout << treeSpaces << "-- Duration          : 0 s)" << std::endl;
      }
      std::cout << treeSpaces << "-- Layer             : " << box->layer() << std::endl;
      std::cout << treeSpaces << "-- Alternate Group   : " << box->alternateGroup() << std::endl;
      std::cout << treeSpaces << "-- Volume            : " << box->volume() << " ("
                << box->volumeHR() << ")" << std::endl;
      std::cout << treeSpaces << "-- Matrix            : {";
      for (auto value : box->matrix()) {
        std::cout << " " << value;
      }
      std::cout << " }" << std::endl;
      std::cout << treeSpaces << "-- Width             : " << box->width() << std::endl;
      std::cout << treeSpaces << "-- Height            : " << box->height() << std::endl;
      return;
    };

    printMap[(toFcc("stts"))] = [&](const BoxItem& item, const std::string& treeSpaces) {
      auto box = std::dynamic_pointer_cast<CDecodingTimeToSampleBox>(item);

      std::cout << treeSpaces << "-- Box Version : " << static_cast<uint32_t>(box->version())
                << std::endl;
      std::cout << treeSpaces << "-- Box Flags   : " << box->flags() << std::endl;
      std::cout << treeSpaces << "-- Entry Count : " << box->entries().size() << std::endl;

      for (size_t i = 0; i < box->entries().size() && i < maxEntryPrintNr; ++i) {
        std::cout << treeSpaces << "-- Stts Entry  : {";
        std::cout << " Sample Count: " << box->entries()[i].sampleCount << ",";
        std::cout << " Sample Delta: " << box->entries()[i].sampleDelta;
        std::cout << " }" << std::endl;
      }

      if (maxEntryPrintNr < box->entries().size()) {
        std::cout << treeSpaces << "-- Stts Entry : ... " << box->entries().size() - maxEntryPrintNr
                  << " more" << std::endl;
      }
      return;
    };

    printMap[(toFcc("stsc"))] = [&](const BoxItem& item, const std::string& treeSpaces) {
      auto box = std::dynamic_pointer_cast<CSampleToChunkBox>(item);

      std::cout << treeSpaces << "-- Box Version : " << static_cast<uint32_t>(box->version())
                << std::endl;
      std::cout << treeSpaces << "-- Box Flags   : " << box->flags() << std::endl;
      std::cout << treeSpaces << "-- Entry Count : " << box->entryCount() << std::endl;

      for (size_t i = 0; i < box->entries().size() && i < maxEntryPrintNr; ++i) {
        std::cout << treeSpaces << "-- Stsc Entry  : {";
        std::cout << " First Chunk: " << box->entries()[i].first_chunk << ",";
        std::cout << " Sample per Chunk: " << box->entries()[i].samples_per_chunk << ",";
        std::cout << " Sample Description Index: " << box->entries()[i].sample_description_index;
        std::cout << " }" << std::endl;
      }

      if (maxEntryPrintNr < box->entries().size()) {
        std::cout << treeSpaces << "-- Stsc Entry  : ... "
                  << box->entries().size() - maxEntryPrintNr << " more" << std::endl;
      }
      return;
    };

    printMap[(toFcc("stsz"))] = [&](const BoxItem item, const std::string& treeSpaces) {
      auto box = std::dynamic_pointer_cast<CSampleSizeBox>(item);

      std::cout << treeSpaces << "-- Box Version  : " << static_cast<uint32_t>(box->version())
                << std::endl;
      std::cout << treeSpaces << "-- Box Flags    : " << box->flags() << std::endl;
      std::cout << treeSpaces << "-- Sample Size  : " << box->sampleSize() << std::endl;
      std::cout << treeSpaces << "-- Sample Count : " << box->sampleCount() << std::endl;

      for (size_t i = 0; i < box->entrySize().size() && i < maxEntryPrintNr; ++i) {
        std::cout << treeSpaces << "-- Entry Size   : " << box->entrySize()[i] << std::endl;
      }

      if (maxEntryPrintNr < box->entrySize().size()) {
        std::cout << treeSpaces << "-- Entry Size   : ... "
                  << box->entrySize().size() - maxEntryPrintNr << " more" << std::endl;
      }
      return;
    };

    printMap[(toFcc("stco"))] = [&](const BoxItem& item, const std::string& treeSpaces) {
      auto box = std::dynamic_pointer_cast<CChunkOffsetBox>(item);

      std::cout << treeSpaces << "-- Box Version        : " << static_cast<uint32_t>(box->version())
                << std::endl;
      std::cout << treeSpaces << "-- Box Flags          : " << box->flags() << std::endl;
      std::cout << treeSpaces << "-- Chunk Offset Count : " << box->chunkOffsets().size()
                << std::endl;

      for (size_t i = 0; i < box->chunkOffsets().size() && i < maxEntryPrintNr; ++i) {
        std::cout << treeSpaces << "-- Chunk Offset       : " << box->chunkOffsets()[i]
                  << std::endl;
      }

      if (maxEntryPrintNr < box->chunkOffsets().size()) {
        std::cout << treeSpaces << "-- Chunk Offset       : ... "
                  << box->chunkOffsets().size() - maxEntryPrintNr << " more" << std::endl;
      }
      return;
    };

    printMap[(toFcc("stss"))] = [&](const BoxItem& item, const std::string& treeSpaces) {
      auto box = std::dynamic_pointer_cast<CSyncSampleTableBox>(item);

      std::cout << treeSpaces << "-- Box Version        : " << static_cast<uint32_t>(box->version())
                << std::endl;
      std::cout << treeSpaces << "-- Box Flags          : " << box->flags() << std::endl;
      std::cout << treeSpaces << "-- Sync Sample Count  : " << box->entries().size() << std::endl;

      for (size_t i = 0; i < box->entries().size() && i < maxEntryPrintNr; ++i) {
        std::cout << treeSpaces << "-- Sync Sample        : " << box->entries()[i].sampleNumber
                  << std::endl;
      }

      if (maxEntryPrintNr < box->entries().size()) {
        std::cout << treeSpaces << "-- Sync Sample        : ... "
                  << box->entries().size() - maxEntryPrintNr << " more" << std::endl;
      }
      return;
    };

    printMap[(toFcc("ctts"))] = [&](const BoxItem& item, const std::string& treeSpaces) {
      auto box = std::dynamic_pointer_cast<CCompositionTimeToSampleBox>(item);

      std::cout << treeSpaces << "-- Box Version : " << static_cast<uint32_t>(box->version())
                << std::endl;
      std::cout << treeSpaces << "-- Box Flags   : " << box->flags() << std::endl;
      std::cout << treeSpaces << "-- Entries     : " << box->entries().size() << std::endl;

      for (size_t i = 0; i < box->entries().size() && i < maxEntryPrintNr; ++i) {
        std::cout << treeSpaces << "-- Ctts Entry  : { ";
        std::cout << "Sample Count: " << box->entries()[i].sampleCount;
        std::cout << ", Sample Offset: " << box->entries()[i].sampleOffset;
        std::cout << " }" << std::endl;
      }

      if (maxEntryPrintNr < box->entries().size()) {
        std::cout << treeSpaces << "-- Ctts Entry  : ... "
                  << box->entries().size() - maxEntryPrintNr << " more" << std::endl;
      }
      return;
    };

    printMap[(toFcc("trun"))] = [&](const BoxItem& item, const std::string& treeSpaces) {
      auto box = std::dynamic_pointer_cast<CTrackRunBox>(item);

      std::cout << treeSpaces
                << "-- Box Version             : " << static_cast<uint32_t>(box->version())
                << std::endl;
      std::cout << treeSpaces << "-- Box Flags               : " << box->flags() << std::endl;
      std::cout << treeSpaces << "-- Sample Count            : " << box->sampleCount() << std::endl;
      std::cout << treeSpaces << "-- DataOffsetPresent?      : " << box->dataOffsetPresent()
                << std::endl;
      std::cout << treeSpaces << "-- FirstSampleFlagsPresent?: " << box->firstSampleFlagsPresent()
                << std::endl;
      std::cout << treeSpaces << "-- SampleCtsOffsetPresent? : " << box->sampleCtsOffsetPresent()
                << std::endl;
      std::cout << treeSpaces << "-- SampleDurationPresent?  : " << box->sampleDurationPresent()
                << std::endl;
      std::cout << treeSpaces << "-- SampleFlagsPresent?     : " << box->sampleFlagsPresent()
                << std::endl;
      std::cout << treeSpaces << "-- SampleSizePresent?      : " << box->sampleSizePresent()
                << std::endl;

      if (box->dataOffsetPresent()) {
        std::cout << treeSpaces << "-- Data Offset             : " << box->dataOffset()
                  << std::endl;
      }

      if (box->firstSampleFlagsPresent()) {
        auto flags = tools::valueToSampleFlags(box->firstSampleFlags());
        std::cout << treeSpaces << "-- First Sample Flags      : " << box->firstSampleFlags()
                  << " ("
                  << "is_leading : " << static_cast<uint16_t>(flags.isLeading) << ", "
                  << "depends_on : " << static_cast<uint16_t>(flags.dependsON) << ", "
                  << "is_depended_on : " << static_cast<uint16_t>(flags.isDependedOn) << ", "
                  << "has_redundancy : " << static_cast<uint16_t>(flags.hasRedundancy) << ", "
                  << "padding_value : " << static_cast<uint16_t>(flags.paddingValue) << ", "
                  << "is_non_sync_sample : " << static_cast<uint16_t>(flags.isNonSyncSample) << ", "
                  << "degradation_priority : " << static_cast<uint16_t>(flags.degradationPriority)
                  << ")" << std::endl;
      }

      std::cout << treeSpaces << "-- Trun Entry Count        : " << box->trunEntries().size()
                << std::endl;

      for (size_t i = 0; i < box->trunEntries().size() && i < maxEntryPrintNr; ++i) {
        std::cout << treeSpaces << "-- Trun Entry              : {";
        if (box->sampleDurationPresent()) {
          std::cout << " Sample Duration: " << box->trunEntries()[i].sampleDuration();
        }

        if (box->sampleSizePresent()) {
          std::cout << ", Sample Size: " << box->trunEntries()[i].sampleSize();
        }

        if (box->sampleFlagsPresent()) {
          std::cout << ", Sample Flags: " << box->trunEntries()[i].sampleFlags();
        }

        if (box->sampleCtsOffsetPresent()) {
          std::cout << ", Sample CTS Offset: " << box->trunEntries()[i].sampleCtsOffset();
        }
        std::cout << " }" << std::endl;
      }

      if (maxEntryPrintNr < box->trunEntries().size()) {
        std::cout << treeSpaces << "-- Trun Entry              : ... "
                  << box->trunEntries().size() - maxEntryPrintNr << " more" << std::endl;
      }
      return;
    };

    printMap[(toFcc("mdhd"))] = [&](const BoxItem& item, const std::string& treeSpaces) {
      auto box = std::dynamic_pointer_cast<CMediaHeaderBox>(item);

      std::cout << treeSpaces << "-- Box Version       : " << static_cast<uint32_t>(box->version())
                << std::endl;
      std::cout << treeSpaces << "-- Box Flags         : " << box->flags() << std::endl;
      std::cout << treeSpaces
                << "-- Creation Time     : " << tools::UTCTimeToString(box->creationTime())
                << std::endl;
      std::cout << treeSpaces
                << "-- Modification Time : " << tools::UTCTimeToString(box->modificationTime())
                << std::endl;
      std::cout << treeSpaces << "-- Timescale         : " << box->timescale() << std::endl;
      std::cout << treeSpaces << "-- Duration          : " << box->duration() << " ("
                << box->durationSeconds() << " s)" << std::endl;
      std::cout << treeSpaces << "-- Language          : " << toString(box->language())
                << std::endl;
      return;
    };

    printMap[(toFcc("hdlr"))] = [&](const BoxItem& item, const std::string& treeSpaces) {
      auto box = std::dynamic_pointer_cast<CHandlerReferenceBox>(item);

      std::cout << treeSpaces << "-- Box Version  : " << static_cast<uint32_t>(box->version())
                << std::endl;
      std::cout << treeSpaces << "-- Box Flags    : " << box->flags() << std::endl;
      std::cout << treeSpaces << "-- Handler Type : " << toString(box->handlerType()) << std::endl;
      std::cout << treeSpaces << "-- Handler Name : " << box->name() << std::endl;
      return;
    };

    printMap[(toFcc("smhd"))] = [&](const BoxItem& item, const std::string& treeSpaces) {
      auto box = std::dynamic_pointer_cast<CSoundMediaHeaderBox>(item);

      std::cout << treeSpaces << "-- Box Version : " << static_cast<uint32_t>(box->version())
                << std::endl;
      std::cout << treeSpaces << "-- Box Flags   : " << box->flags() << std::endl;
      std::cout << treeSpaces << "-- Balance     : " << box->balance() << " (" << box->balanceHR()
                << ")" << std::endl;
      return;
    };

    printMap[(toFcc("vmhd"))] = [&](const BoxItem& item, const std::string& treeSpaces) {
      auto box = std::dynamic_pointer_cast<CVideoMediaHeaderBox>(item);

      std::cout << treeSpaces << "-- Box Version : " << static_cast<uint32_t>(box->version())
                << std::endl;
      std::cout << treeSpaces << "-- Box Flags   : " << box->flags() << std::endl;
      std::cout << treeSpaces << "-- GraphicsMode: " << box->graphicsMode() << std::endl;
      std::cout << treeSpaces << "-- Opcolor     : [ ";

      auto opcolor = box->opcolor();
      for (size_t i = 0; i < opcolor.size(); ++i) {
        std::cout << opcolor[i] << " ";
      }

      std::cout << "]" << std::endl;
      return;
    };

    printMap[(toFcc("dref"))] = [&](const BoxItem& item, const std::string& treeSpaces) {
      auto box = std::dynamic_pointer_cast<CDataReferenceBox>(item);

      std::cout << treeSpaces << "-- Box Version : " << static_cast<uint32_t>(box->version())
                << std::endl;
      std::cout << treeSpaces << "-- Box Flags   : " << box->flags() << std::endl;
      std::cout << treeSpaces << "-- Entry Count : " << box->entryCount() << std::endl;
      return;
    };

    printMap[(toFcc("url "))] = [&](const BoxItem& item, const std::string& treeSpaces) {
      auto box = std::dynamic_pointer_cast<CDataEntryUrlBox>(item);

      std::cout << treeSpaces << "-- Box Version : " << static_cast<uint32_t>(box->version())
                << std::endl;
      std::cout << treeSpaces << "-- Box Flags   : " << box->flags() << std::endl;
      std::cout << treeSpaces << "-- Location    : " << box->location() << std::endl;

      return;
    };

    printMap[(toFcc("stsd"))] = [&](const BoxItem& item, const std::string& treeSpaces) {
      auto box = std::dynamic_pointer_cast<CSampleDescriptionBox>(item);

      std::cout << treeSpaces << "-- Box Version : " << static_cast<uint32_t>(box->version())
                << std::endl;
      std::cout << treeSpaces << "-- Box Flags   : " << box->flags() << std::endl;
      std::cout << treeSpaces << "-- Entry Count : " << box->entryCount() << std::endl;
      return;
    };

    printMap[(toFcc("avcC"))] = [&](const BoxItem& item, const std::string& treeSpaces) {
      auto decoderConfigBox = std::dynamic_pointer_cast<CDecoderConfigurationBox>(item);

      if (decoderConfigBox == nullptr) {
        return;
      }

      std::cout << treeSpaces << "-- ###ConfigRecordData###" << std::endl;

      ByteBuffer decoderConfigBlob = decoderConfigBox->decoderConfiguration();
      ByteBuffer::const_iterator iter = decoderConfigBlob.begin();

      auto configRecord = CAvcDecoderConfigRecord(iter, decoderConfigBlob.end());
      std::cout << treeSpaces << "-- ConfigurationVersion : "
                << static_cast<uint32_t>(configRecord.configurationVersion()) << std::endl;
      std::cout << treeSpaces << "-- AvcLevelIndication   : "
                << static_cast<uint32_t>(configRecord.avcLevelIndication()) << std::endl;
      std::cout << treeSpaces << "-- AvcProfileIndication : "
                << static_cast<uint32_t>(configRecord.avcProfileIndication()) << std::endl;
      std::cout << treeSpaces << "-- BitDepthChromaMinus8 : "
                << static_cast<uint32_t>(configRecord.bitDepthChromaMinus8()) << std::endl;
      std::cout << treeSpaces << "-- BitDepthLumaMinus8   : "
                << static_cast<uint32_t>(configRecord.bitDepthLumaMinus8()) << std::endl;
      std::cout << treeSpaces << "-- ChromaFormat         : "
                << static_cast<uint32_t>(configRecord.chromaFormat()) << std::endl;
      std::cout << treeSpaces << "-- LengthSizeMinusOne   : "
                << static_cast<uint32_t>(configRecord.lengthSizeMinusOne()) << std::endl;
      std::cout << treeSpaces << "-- ProfileCompatibility : "
                << static_cast<uint32_t>(configRecord.profileCompatibility()) << std::endl;
      std::cout << treeSpaces << "-- NumOfSps             : "
                << static_cast<uint32_t>(configRecord.sequenceParameterSets().size()) << std::endl;
      std::cout << treeSpaces << "-- NumOfPps             : "
                << static_cast<uint32_t>(configRecord.pictureParameterSets().size()) << std::endl;
      std::cout << treeSpaces << "-- NumOfSpsExt          : "
                << static_cast<uint32_t>(configRecord.sequenceParameterExtSets().size())
                << std::endl;
    };

    printMap[(toFcc("hvcC"))] = [&](const BoxItem& item, const std::string& treeSpaces) {
      auto decoderConfigBox = std::dynamic_pointer_cast<CDecoderConfigurationBox>(item);

      if (decoderConfigBox == nullptr) {
        return;
      }

      std::cout << treeSpaces << "-- ###ConfigRecordData###" << std::endl;

      ByteBuffer decoderConfigBlob = decoderConfigBox->decoderConfiguration();
      ByteBuffer::const_iterator iter = decoderConfigBlob.begin();

      auto configRecord = CHevcDecoderConfigRecord(iter, decoderConfigBlob.end());

      std::cout << treeSpaces << "-- ConfigurationVersion             : "
                << static_cast<uint32_t>(configRecord.configurationVersion()) << std::endl;
      std::cout << treeSpaces << "-- GeneralProfileSpace              : "
                << static_cast<uint32_t>(configRecord.generalProfileSpace()) << std::endl;
      std::cout << treeSpaces
                << "-- GeneralTierFlag                  : " << configRecord.generalTierFlag()
                << std::endl;
      std::cout << treeSpaces << "-- GeneralProfileIdc                : "
                << static_cast<uint32_t>(configRecord.generalProfileIdc()) << std::endl;
      std::cout << treeSpaces << "-- GeneralProfileCompatabilityFlags : "
                << configRecord.generalProfileCompatabilityFlags() << std::endl;
      std::cout << treeSpaces << "-- GeneralConstraintIndicatorFlags  : "
                << configRecord.generalConstraintIndicatorFlags() << std::endl;
      std::cout << treeSpaces << "-- GeneralLevelIdc                  : "
                << static_cast<uint32_t>(configRecord.generalLevelIdc()) << std::endl;
      std::cout << treeSpaces << "-- MinSpatialSegmentationIdc        : "
                << configRecord.minSpatialSegmentationIdc() << std::endl;
      std::cout << treeSpaces << "-- ParalelismType                   : "
                << static_cast<uint32_t>(configRecord.paralelismType()) << std::endl;
      std::cout << treeSpaces << "-- ChromaFormatIdc                  : "
                << static_cast<uint32_t>(configRecord.chromaFormatIdc()) << std::endl;
      std::cout << treeSpaces << "-- BitDepthLumaMinus8               : "
                << static_cast<uint32_t>(configRecord.bitDepthLumaMinus8()) << std::endl;
      std::cout << treeSpaces << "-- BitDepthChromaMinus8             : "
                << static_cast<uint32_t>(configRecord.bitDepthChromaMinus8()) << std::endl;
      std::cout << treeSpaces
                << "-- AvgFrameRate                     : " << configRecord.avgFrameRate()
                << std::endl;
      std::cout << treeSpaces << "-- ConstFrameRate                   : "
                << static_cast<uint32_t>(configRecord.constFrameRate()) << std::endl;
      std::cout << treeSpaces << "-- NumTemporatlLayers               : "
                << static_cast<uint32_t>(configRecord.numTemporatlLayers()) << std::endl;
      std::cout << treeSpaces
                << "-- TemporalIdNested                 : " << configRecord.temporalIdNested()
                << std::endl;
      std::cout << treeSpaces << "-- LengthSizeMinusOne               : "
                << static_cast<uint32_t>(configRecord.lengthSizeMinusOne()) << std::endl;
      std::cout << treeSpaces << "-- NumOfArrays                      : "
                << static_cast<uint32_t>(configRecord.nonVclArrays().size()) << std::endl;

      for (const auto& nonVCLArray : configRecord.nonVclArrays()) {
        std::cout << treeSpaces
                  << "-- NonVCL-Nalu: { NaluType: " << static_cast<uint32_t>(nonVCLArray.naluType);
        std::cout << ",  NaluCount: " << nonVCLArray.nalus.size();
        std::cout << ",  ArrayCompleteness: " << nonVCLArray.arrayCompleteness;
        std::cout << ",  NaluSizes: { ";

        for (const auto& nalu : nonVCLArray.nalus) {
          std::cout << nalu.size() << ", ";
        }
        std::cout << " } }" << std::endl;
      }
      return;
    };

    printMap[(toFcc("mhaC"))] = [&](const BoxItem& item, const std::string& treeSpaces) {
      auto decoderConfigBox = std::dynamic_pointer_cast<CDecoderConfigurationBox>(item);

      if (decoderConfigBox == nullptr) {
        return;
      }

      ByteBuffer decoderConfigBlob = decoderConfigBox->decoderConfiguration();
      ByteBuffer::const_iterator iter = decoderConfigBlob.begin();

      auto configRecord = CMhaDecoderConfigRecord(iter, decoderConfigBlob.end());

      std::cout << treeSpaces << "-- ###ConfigRecordData###" << std::endl;
      std::cout << treeSpaces << "-- ConfigurationVersion           : "
                << static_cast<uint32_t>(configRecord.configurationVersion()) << std::endl;
      std::cout << treeSpaces << "-- Mpegh3daProfileLevelIndication : "
                << static_cast<uint32_t>(configRecord.mpegh3daProfileLevelIndication())
                << std::endl;
      std::cout << treeSpaces << "-- ReferenceChannelLayout         : "
                << static_cast<uint32_t>(configRecord.referenceChannelLayout()) << std::endl;
      std::cout << treeSpaces << "-- Mpegh3daConfigLength           : "
                << static_cast<uint32_t>(configRecord.mpegh3daConfig().size()) << std::endl;
      return;
    };

    printMap[(toFcc("mhaP"))] = [&](const BoxItem item, const std::string& treeSpaces) {
      auto box = std::dynamic_pointer_cast<CMhaProfileLevelCompatibilitySetBox>(item);

      std::cout << treeSpaces
                << "-- NumCompatibleSets          : " << box->profileAndLevelCompatibleSets().size()
                << std::endl;

      for (size_t i = 0; i < box->profileAndLevelCompatibleSets().size() && i < maxEntryPrintNr;
           ++i) {
        std::cout << treeSpaces << "-- CompatibleSetIndication    : "
                  << static_cast<uint32_t>(box->profileAndLevelCompatibleSets()[i]) << std::endl;
      }

      if (maxEntryPrintNr < box->profileAndLevelCompatibleSets().size()) {
        std::cout << treeSpaces << "-- CompatibleSetIndication    : ... "
                  << box->profileAndLevelCompatibleSets().size() - maxEntryPrintNr << " more"
                  << std::endl;
      }
      return;
    };

    printMap[(toFcc("esds"))] = [&](const BoxItem& item, const std::string& treeSpaces) {
      auto decoderConfigBox = std::dynamic_pointer_cast<CDecoderConfigurationFullBox>(item);

      if (decoderConfigBox == nullptr) {
        return;
      }

      ByteBuffer decoderConfigBlob = decoderConfigBox->decoderConfiguration();
      ByteBuffer::const_iterator iter = decoderConfigBlob.begin();

      auto configRecord = CMp4aDecoderConfigRecord(iter, decoderConfigBlob.end());
      auto asc = configRecord.asc();

      std::cout << treeSpaces << "-- ###ConfigRecordData###" << std::endl;
      std::cout << treeSpaces << "-- ObjectTypeIndication : "
                << static_cast<uint32_t>(configRecord.objectTypeIndication()) << std::endl;
      std::cout << treeSpaces
                << "-- StreamType           : " << static_cast<uint32_t>(configRecord.streamType())
                << std::endl;
      std::cout << treeSpaces
                << "-- UpStream             : " << static_cast<uint32_t>(configRecord.upStream())
                << std::endl;
      std::cout << treeSpaces << "-- BufferSizeDB         : "
                << static_cast<uint32_t>(configRecord.bufferSizeDB()) << std::endl;
      std::cout << treeSpaces
                << "-- MaxBitrate           : " << static_cast<uint32_t>(configRecord.maxBitrate())
                << std::endl;
      std::cout << treeSpaces
                << "-- AvgBitrate           : " << static_cast<uint32_t>(configRecord.avgBitrate())
                << std::endl;
      std::cout << treeSpaces
                << "-- AscConfigLength      : " << static_cast<uint32_t>(configRecord.asc().size())
                << std::endl;
      std::cout << treeSpaces << "-- AscData              : " << std::hex << std::uppercase;
      for (const uint8_t& value : asc) {
        std::cout << "0x" << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(value)
                  << ' ';
      }
      std::cout << std::nouppercase << std::dec << std::endl;
      return;
    };

    printMap[(toFcc("jxsH"))] = [&](const BoxItem& item, const std::string& treeSpaces) {
      auto decoderConfigBox = std::dynamic_pointer_cast<CDecoderConfigurationBox>(item);

      if (decoderConfigBox == nullptr) {
        return;
      }

      ByteBuffer decoderConfigBlob = decoderConfigBox->decoderConfiguration();
      ByteBuffer::const_iterator iter = decoderConfigBlob.begin();

      auto configRecord = CJxsDecoderConfigRecord(iter, decoderConfigBlob.end());

      std::cout << treeSpaces << "-- ### Config Record Data ###" << std::endl;
      std::cout << treeSpaces << "-- Length : " << configRecord.size() << std::endl;
      std::cout << treeSpaces << "-- Data   : ";

      ilo::ByteBuffer codestreamHeader = configRecord.codestreamHeader();
      for (uint8_t i = 0; i < configRecord.size(); i++) {
        if (i > 10u) {
          std::cout << " ... ";
          break;
        }

        std::cout << std::hex << std::uppercase << "0x" << std::setfill('0') << std::setw(2)
                  << static_cast<uint16_t>(codestreamHeader.at(i)) << std::dec << " ";
      }

      std::cout << std::endl;
    };

    printMap[(toFcc("trex"))] = [&](const BoxItem& item, const std::string& treeSpaces) {
      auto box = std::dynamic_pointer_cast<CTrackExtendsBox>(item);

      std::cout << treeSpaces << "-- Box Version        : " << static_cast<uint32_t>(box->version())
                << std::endl;
      std::cout << treeSpaces << "-- Box Flags          : " << box->flags() << std::endl;
      std::cout << treeSpaces << "-- DefSampleDescIndex : " << box->defaultSampleDescriptionIndex()
                << std::endl;
      std::cout << treeSpaces << "-- DefSampleDuration  : " << box->defaultSampleDuration()
                << std::endl;
      std::cout << treeSpaces << "-- DefSampleSize      : " << box->defaultSampleSize()
                << std::endl;

      auto flags = tools::valueToSampleFlags(box->defaultSampleFlags());
      std::cout << treeSpaces << "-- DefSampleFlags     : " << box->defaultSampleFlags() << " ("
                << "is_leading : " << static_cast<uint16_t>(flags.isLeading) << ", "
                << "depends_on : " << static_cast<uint16_t>(flags.dependsON) << ", "
                << "is_depended_on : " << static_cast<uint16_t>(flags.isDependedOn) << ", "
                << "has_redundancy : " << static_cast<uint16_t>(flags.hasRedundancy) << ", "
                << "padding_value : " << static_cast<uint16_t>(flags.paddingValue) << ", "
                << "is_non_sync_sample : " << static_cast<uint16_t>(flags.isNonSyncSample) << ", "
                << "degradation_priority : " << static_cast<uint16_t>(flags.degradationPriority)
                << ")" << std::endl;
      return;
    };

    printMap[(toFcc("mfhd"))] = [&](const BoxItem& item, const std::string& treeSpaces) {
      auto box = std::dynamic_pointer_cast<CMovieFragmentHeaderBox>(item);

      std::cout << treeSpaces << "-- Box Version     : " << static_cast<uint32_t>(box->version())
                << std::endl;
      std::cout << treeSpaces << "-- Box Flags       : " << box->flags() << std::endl;
      std::cout << treeSpaces << "-- Sequence Number : " << box->sequenceNumber() << std::endl;
      return;
    };

    printMap[(toFcc("tfhd"))] = [&](const BoxItem& item, const std::string& treeSpaces) {
      auto box = std::dynamic_pointer_cast<CTrackFragmentHeaderBox>(item);

      std::cout << treeSpaces
                << "-- Box Version            : " << static_cast<uint32_t>(box->version())
                << std::endl;
      std::cout << treeSpaces << "-- Box Flags              : " << box->flags() << std::endl;
      std::cout << treeSpaces << "-- Track ID               : " << box->trackId() << std::endl;
      std::cout << treeSpaces << "-- DefaultBaseIsMoof?     : " << box->defaultBaseIsMoof()
                << std::endl;
      std::cout << treeSpaces << "-- BaseDataOffsetPresent? : " << box->baseDataOffsetPresent()
                << std::endl;
      std::cout << treeSpaces
                << "-- SampleDescrIndPresent? : " << box->sampleDescriptionIndexPresent()
                << std::endl;
      std::cout << treeSpaces
                << "-- DefSampleDurPresent?   : " << box->defaultSampleDurationPresent()
                << std::endl;
      std::cout << treeSpaces << "-- DefSampleSizePresent?  : " << box->defaultSampleSizePresent()
                << std::endl;
      std::cout << treeSpaces << "-- DefSampleFlagsPresent? : " << box->defaultSampleFlagsPresent()
                << std::endl;

      if (box->baseDataOffsetPresent()) {
        std::cout << treeSpaces << "-- Base Data Offset       : " << box->baseDataOffset()
                  << std::endl;
      }

      if (box->sampleDescriptionIndexPresent()) {
        std::cout << treeSpaces << "-- Sample Description Index:" << box->sampleDescriptionIndex()
                  << std::endl;
      }

      if (box->defaultSampleDurationPresent()) {
        std::cout << treeSpaces << "-- Default Sample Duration: " << box->defaultSampleDuration()
                  << std::endl;
      }

      if (box->defaultSampleSizePresent()) {
        std::cout << treeSpaces << "-- Default Sample Size    : " << box->defaultSampleSize()
                  << std::endl;
      }

      if (box->defaultSampleFlagsPresent()) {
        auto flags = tools::valueToSampleFlags(box->defaultSampleFlags());
        std::cout << treeSpaces << "-- Default Sample Flags   : " << box->defaultSampleFlags()
                  << " ("
                  << "is_leading : " << static_cast<uint16_t>(flags.isLeading) << ", "
                  << "depends_on : " << static_cast<uint16_t>(flags.dependsON) << ", "
                  << "is_depended_on : " << static_cast<uint16_t>(flags.isDependedOn) << ", "
                  << "has_redundancy : " << static_cast<uint16_t>(flags.hasRedundancy) << ", "
                  << "padding_value : " << static_cast<uint16_t>(flags.paddingValue) << ", "
                  << "is_non_sync_sample : " << static_cast<uint16_t>(flags.isNonSyncSample) << ", "
                  << "degradation_priority : " << static_cast<uint16_t>(flags.degradationPriority)
                  << ")" << std::endl;
      }
      return;
    };

    printMap[(toFcc("tfdt"))] = [&](const BoxItem& item, const std::string& treeSpaces) {
      auto box = std::dynamic_pointer_cast<CTrackFragmentMDTBox>(item);

      std::cout << treeSpaces
                << "-- Box Version            : " << static_cast<uint32_t>(box->version())
                << std::endl;
      std::cout << treeSpaces << "-- Box Flags              : " << box->flags() << std::endl;
      std::cout << treeSpaces << "-- Base Media Decode Time : " << box->baseMediaDecodeTime()
                << std::endl;
      return;
    };

    printMap[(toFcc("mp4a"))] = [&](const BoxItem& item, const std::string& treeSpaces) {
      auto box = std::dynamic_pointer_cast<CMp4aSampleEntry>(item);
      std::cout << treeSpaces << "-- Channel Count : " << box->channelCount() << std::endl;
      std::cout << treeSpaces << "-- Sample Size   : " << box->sampleSize() << std::endl;
      std::cout << treeSpaces << "-- Sample Rate   : " << box->sampleRate() << std::endl;
      return;
    };

    printMap[(toFcc("mha1"))] = [&](const BoxItem& item, const std::string& treeSpaces) {
      auto box = std::dynamic_pointer_cast<CMhaSampleEntry>(item);
      std::cout << treeSpaces << "-- Channel Count : " << box->channelCount() << std::endl;
      std::cout << treeSpaces << "-- Sample Size   : " << box->sampleSize() << std::endl;
      std::cout << treeSpaces << "-- Sample Rate   : " << box->sampleRate() << std::endl;
      return;
    };

    printMap[(toFcc("mha2"))] = printMap[(toFcc("mha1"))];

    printMap[(toFcc("mhm1"))] = [&](const BoxItem& item, const std::string& treeSpaces) {
      auto box = std::dynamic_pointer_cast<CMhmSampleEntry>(item);
      std::cout << treeSpaces << "-- Channel Count : " << box->channelCount() << std::endl;
      std::cout << treeSpaces << "-- Sample Size   : " << box->sampleSize() << std::endl;
      std::cout << treeSpaces << "-- Sample Rate   : " << box->sampleRate() << std::endl;
      return;
    };

    printMap[(toFcc("mhm2"))] = printMap[(toFcc("mhm1"))];

    printMap[(toFcc("avc1"))] = [&](const BoxItem& item, const std::string& treeSpaces) {
      auto box = std::dynamic_pointer_cast<CAvcSampleEntry>(item);
      std::cout << treeSpaces << "-- Width                  : " << box->width() << std::endl;
      std::cout << treeSpaces << "-- Height                 : " << box->height() << std::endl;
      std::cout << treeSpaces << "-- HorzRes                : " << std::hex << std::uppercase
                << "0x" << std::setfill('0') << std::setw(8)
                << static_cast<uint32_t>(box->horizresolution()) << std::dec << " ("
                << (box->horizresolution() >> 16) << " dpi)" << std::endl;
      std::cout << treeSpaces << "-- VertRes                : " << std::hex << std::uppercase
                << "0x" << std::setfill('0') << std::setw(8)
                << static_cast<uint32_t>(box->horizresolution()) << std::dec << " ("
                << (box->vertresolution() >> 16) << " dpi)" << std::endl;
      std::cout << treeSpaces << "-- Frame Count per Sample : " << box->framecount() << std::endl;
      std::cout << treeSpaces << "-- Compressor Name        : " << box->compressorname()
                << std::endl;
      std::cout << treeSpaces << "-- Depth [Coded, no bits] : " << std::hex << std::uppercase
                << "0x" << std::setfill('0') << std::setw(4) << static_cast<uint32_t>(box->depth())
                << std::dec << std::endl;
    };

    printMap[(toFcc("avc3"))] = printMap[(toFcc("avc1"))];

    printMap[(toFcc("hvc1"))] = [&](const BoxItem& item, const std::string& treeSpaces) {
      auto box = std::dynamic_pointer_cast<CHevcSampleEntry>(item);
      std::cout << treeSpaces << "-- Width                  : " << box->width() << std::endl;
      std::cout << treeSpaces << "-- Height                 : " << box->height() << std::endl;
      std::cout << treeSpaces << "-- HorzRes                : " << std::hex << std::uppercase
                << "0x" << std::setfill('0') << std::setw(8)
                << static_cast<uint32_t>(box->horizresolution()) << std::dec << " ("
                << (box->horizresolution() >> 16) << " dpi)" << std::endl;
      std::cout << treeSpaces << "-- VertRes                : " << std::hex << std::uppercase
                << "0x" << std::setfill('0') << std::setw(8)
                << static_cast<uint32_t>(box->horizresolution()) << std::dec << " ("
                << (box->vertresolution() >> 16) << " dpi)" << std::endl;
      std::cout << treeSpaces << "-- Frame Count per Sample : " << box->framecount() << std::endl;
      std::cout << treeSpaces << "-- Compressor Name        : " << box->compressorname()
                << std::endl;
      std::cout << treeSpaces << "-- Depth [Coded, no bits] : " << std::hex << std::uppercase
                << "0x" << std::setfill('0') << std::setw(4) << static_cast<uint32_t>(box->depth())
                << std::dec << std::endl;
    };

    printMap[(toFcc("hev1"))] = printMap[(toFcc("hvc1"))];

    printMap[(toFcc("jxsm"))] = [&](const BoxItem& item, const std::string& treeSpaces) {
      auto box = std::dynamic_pointer_cast<CJxsSampleEntry>(item);
      std::cout << treeSpaces << "-- Width                  : " << box->width() << std::endl;
      std::cout << treeSpaces << "-- Height                 : " << box->height() << std::endl;
      std::cout << treeSpaces << "-- HorzRes                : " << std::hex << std::uppercase
                << "0x" << std::setfill('0') << std::setw(8)
                << static_cast<uint32_t>(box->horizresolution()) << std::dec << " ("
                << (box->horizresolution() >> 16) << " dpi)" << std::endl;
      std::cout << treeSpaces << "-- VertRes                : " << std::hex << std::uppercase
                << "0x" << std::setfill('0') << std::setw(8)
                << static_cast<uint32_t>(box->horizresolution()) << std::dec << " ("
                << (box->vertresolution() >> 16) << " dpi)" << std::endl;
      std::cout << treeSpaces << "-- Frame Count per Sample : " << box->framecount() << std::endl;
      std::cout << treeSpaces << "-- Compressor Name        : " << box->compressorname()
                << std::endl;
      std::cout << treeSpaces << "-- Depth [Coded, no bits] : " << std::hex << std::uppercase
                << "0x" << std::setfill('0') << std::setw(4) << static_cast<uint32_t>(box->depth())
                << std::dec << std::endl;
    };

    printMap[(toFcc("jpvi"))] = [&](const BoxItem& item, const std::string& treeSpaces) {
      auto box = std::dynamic_pointer_cast<CJPEGXSVideoInformationBox>(item);
      std::cout << treeSpaces << "-- Maximum bit rate     : " << box->brat() << std::endl;
      std::cout << treeSpaces << "-- frat                 : " << std::hex << std::uppercase << "0x"
                << std::setfill('0') << std::setw(8) << box->frat() << std::dec << std::endl;
      std::cout << treeSpaces << "-- schar                : " << std::hex << std::uppercase << "0x"
                << std::setfill('0') << std::setw(4) << box->schar() << std::dec << std::endl;
      uint8_t hour = static_cast<uint8_t>((box->tcod() & 0xFF000000) > 24);
      uint8_t minutes = static_cast<uint8_t>((box->tcod() & 0x00FF0000) > 16);
      uint8_t seconds = static_cast<uint8_t>((box->tcod() & 0x0000FF00) > 8);
      uint8_t frames = static_cast<uint8_t>(box->tcod() & 0x000000FF);
      std::cout << treeSpaces << "-- Time Code            : " << std::dec << std::uppercase
                << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(hour) << " "
                << std::setw(2) << static_cast<uint16_t>(minutes) << " " << std::setw(2)
                << static_cast<uint16_t>(seconds) << " " << std::setw(2)
                << static_cast<uint16_t>(frames) << " (HH MM SS FF)" << std::endl;
    };

    printMap[(toFcc("jxpl"))] = [&](const BoxItem& item, const std::string& treeSpaces) {
      auto box = std::dynamic_pointer_cast<CJXPLProfileandLevelBox>(item);
      std::cout << treeSpaces << "-- Profile              : " << std::hex << std::uppercase << "0x"
                << std::setfill('0') << std::setw(4) << box->ppih() << std::dec << std::endl;
      std::cout << treeSpaces << "-- Level                : " << std::hex << std::uppercase << "0x"
                << std::setfill('0') << std::setw(4) << box->plev() << std::dec << std::endl;
    };

    printMap[(toFcc("colr"))] = [&](const BoxItem& item, const std::string& treeSpaces) {
      auto box = std::dynamic_pointer_cast<CColourInformationBox>(item);
      std::cout << treeSpaces << "-- Colour Type               : " << toString(box->colourType())
                << std::endl;
      if (box->hasColourPrimaries()) {
        std::cout << treeSpaces << "-- Colour Primaries          : " << box->colourPrimaries()
                  << std::endl;
      }
      if (box->hasTransferCharacteristics()) {
        std::cout << treeSpaces
                  << "-- Transfer Characteristics  : " << box->transferCharacteristics()
                  << std::endl;
      }
      if (box->hasMatrixCoefficients()) {
        std::cout << treeSpaces << "-- Matrix Coefficients       : " << box->matrixCoefficients()
                  << std::endl;
      }
      if (box->hasFullRangeFlag()) {
        std::cout << treeSpaces << "-- Full Range Flag           : " << box->fullRangeFlag()
                  << std::endl;
      }
      if (box->hasIccProfile()) {
        std::cout << treeSpaces << "-- ICC Profile               : ";
        ilo::ByteBuffer iccProfile = box->iccProfile();
        for (uint8_t i = 0; i < iccProfile.size(); i++) {
          if (i > 10u) {
            std::cout << " ... ";
            break;
          }
          std::cout << std::hex << std::uppercase << "0x" << std::setfill('0') << std::setw(2)
                    << static_cast<uint16_t>(iccProfile.at(i)) << std::dec << " ";
        }

        std::cout << " { length: " << iccProfile.size() << " }" << std::endl;
      }
    };

    printMap[(toFcc("tlou"))] = [&](const BoxItem& item, const std::string& treeSpaces) {
      auto box = std::dynamic_pointer_cast<CLoudnessBaseBox>(item);
      std::cout << treeSpaces
                << "-- Box Version                   : " << static_cast<uint32_t>(box->version())
                << std::endl;
      std::cout << treeSpaces << "-- Box Flags                     : " << box->flags() << std::endl;

      auto lbSets = box->loudnessBaseSets();
      std::cout << treeSpaces << "  -- Loudness Base Count         : " << lbSets.size()
                << std::endl;
      for (size_t i = 0; i < lbSets.size(); ++i) {
        std::cout << treeSpaces << "  -- Loudness Base Set[" << i + 1 << "/" << lbSets.size() << "]"
                  << std::endl;
        std::cout << treeSpaces << "    -- Downmix ID                : "
                  << static_cast<uint32_t>(lbSets[i].downmixId) << std::endl;
        std::cout << treeSpaces << "    -- DRC Set ID                : "
                  << static_cast<uint32_t>(lbSets[i].drcSetId) << std::endl;
        std::cout << treeSpaces
                  << "    -- BS Sample Peak Level      : " << lbSets[i].bsSamplePeakLevel
                  << std::endl;
        std::cout << treeSpaces
                  << "    -- BS True Peak Level        : " << lbSets[i].bsTruePeakLevel
                  << std::endl;
        std::cout << treeSpaces << "    -- Measurement System For TP : "
                  << static_cast<uint32_t>(lbSets[i].measurementSystemForTp) << std::endl;
        std::cout << treeSpaces << "    -- Reliability For TP        : "
                  << static_cast<uint32_t>(lbSets[i].reliabilityForTp) << std::endl;
        std::cout << treeSpaces
                  << "    -- Measurement Count         : " << lbSets[i].measurementSets.size()
                  << std::endl;

        auto sets = lbSets[i].measurementSets;
        for (size_t j = 0; j < sets.size(); ++j) {
          std::cout << treeSpaces << "    -- Measurement[" << j + 1 << "/" << sets.size() << "]"
                    << std::endl;
          std::cout << treeSpaces << "      -- Method Definition       : "
                    << static_cast<uint32_t>(sets[j].methodDefinition) << std::endl;
          std::cout << treeSpaces << "      -- Method Value            : "
                    << static_cast<uint32_t>(sets[j].methodValue) << std::endl;
          std::cout << treeSpaces << "      -- Measurement System      : "
                    << static_cast<uint32_t>(sets[j].measurementSystem) << std::endl;
          std::cout << treeSpaces << "      -- Reliability             : "
                    << static_cast<uint32_t>(sets[j].reliability) << std::endl;
        }
      }
      return;
    };

    printMap[(toFcc("alou"))] = printMap[(toFcc("tlou"))];

    printMap[(toFcc("sgpd"))] = [&](const BoxItem& item, const std::string& treeSpaces) {
      auto box = std::dynamic_pointer_cast<CSampleGroupDescriptionBox>(item);
      std::cout << treeSpaces
                << "-- Box Version                      : " << static_cast<uint32_t>(box->version())
                << std::endl;
      std::cout << treeSpaces << "-- Box Flags                        : " << box->flags()
                << std::endl;
      std::cout << treeSpaces
                << "-- Grouping Type                    : " << ilo::toString(box->groupingType())
                << std::endl;
      std::cout << treeSpaces << "-- Default Length                   : " << box->defaultLength()
                << std::endl;
      std::cout << treeSpaces
                << "-- Default Sample Description Index : " << box->defaultSampleDescriptionIndex()
                << std::endl;
      std::cout << treeSpaces << "-- Entry Count                      : "
                << box->sampleGroupDescriptionEntries().size() << std::endl;

      auto entries = box->sampleGroupDescriptionEntries();

      if (box->groupingType() == ilo::toFcc("prol")) {
        auto castedEntries = box->downCastSampleGroupEntries<CAudioPreRollEntry>();
        for (size_t i = 0; i < entries.size(); ++i) {
          std::cout << treeSpaces << "  -- Audio PreRoll Entry[" << i + 1 << "/" << entries.size()
                    << "]" << std::endl;
          std::cout << treeSpaces
                    << "    -- Description Length           : " << entries[i].descriptionLength
                    << std::endl;
          std::cout << treeSpaces << "    -- Entry Size                   : "
                    << static_cast<uint32_t>(castedEntries[i]->entrySize()) << std::endl;
          std::cout << treeSpaces
                    << "    -- Roll distance                : " << castedEntries[i]->rollDistance()
                    << std::endl;
        }
      }

      if (box->groupingType() == ilo::toFcc("roll")) {
        auto castedEntries = box->downCastSampleGroupEntries<CAudioRollRecoveryEntry>();
        for (size_t i = 0; i < entries.size(); ++i) {
          std::cout << treeSpaces << "  --  Audio Roll Recovery Entry([" << i + 1 << "/"
                    << entries.size() << "]" << std::endl;
          std::cout << treeSpaces
                    << "    -- Description Length           :" << entries[i].descriptionLength
                    << std::endl;
          std::cout << treeSpaces << "    -- Entry Size                   :"
                    << static_cast<uint32_t>(castedEntries[i]->entrySize()) << std::endl;
          std::cout << treeSpaces
                    << "    -- Roll distance                :" << castedEntries[i]->rollDistance()
                    << std::endl;
        }
      }
      return;
    };

    printMap[(toFcc("sbgp"))] = [&](const BoxItem& item, const std::string& treeSpaces) {
      auto box = std::dynamic_pointer_cast<CSampleToGroupBox>(item);
      std::cout << treeSpaces
                << "-- Box Version             : " << static_cast<uint32_t>(box->version())
                << std::endl;
      std::cout << treeSpaces << "-- Box Flags               : " << box->flags() << std::endl;
      std::cout << treeSpaces
                << "-- Grouping Type           : " << ilo::toString(box->groupingType())
                << std::endl;
      std::cout << treeSpaces << "-- Grouping Type Parameter : " << box->groupingTypeParameter()
                << std::endl;
      std::cout << treeSpaces << "-- Sbgp Entry Count        : " << box->sampleGroupEntries().size()
                << std::endl;

      auto entries = box->sampleGroupEntries();
      for (size_t i = 0; i < entries.size() && i < maxEntryPrintNr; ++i) {
        std::cout << treeSpaces << "-- Sbgp Entry              : {";
        std::cout << " Sample Count: " << entries[i].sampleCount << ",";
        std::cout << " Group Description Index: " << entries[i].groupDescriptionIndex;
        std::cout << " }";
        std::cout << std::endl;
      }

      if (maxEntryPrintNr < entries.size()) {
        std::cout << treeSpaces << "-- Sbgp Entry              : ... "
                  << entries.size() - maxEntryPrintNr << " more" << std::endl;
      }
      return;
    };

    printMap[(toFcc("iods"))] = [&](const BoxItem& item, const std::string& treeSpaces) {
      auto box = std::dynamic_pointer_cast<CObjectDescriptorBox>(item);
      std::cout << treeSpaces
                << "-- Box Version                 : " << static_cast<uint32_t>(box->version())
                << std::endl;
      std::cout << treeSpaces << "-- Box Flags                   : " << box->flags() << std::endl;
      std::cout << treeSpaces << "-- AudioProfileLevelIndication : " << std::hex << std::uppercase
                << "0x" << std::setfill('0') << std::setw(2)
                << static_cast<uint32_t>(box->audioProfileLevelIndication()) << std::dec << " ("
                << static_cast<uint32_t>(box->audioProfileLevelIndication()) << ")" << std::endl;
      return;
    };

    printMap[(toFcc("elst"))] = [&](const BoxItem& item, const std::string& treeSpaces) {
      auto box = std::dynamic_pointer_cast<CEditListBox>(item);
      std::cout << treeSpaces << "-- Box Version      : " << std::to_string(box->version())
                << std::endl;
      std::cout << treeSpaces << "-- Box Flags        : " << box->flags() << std::endl;
      std::cout << treeSpaces << "-- Elst Entry Count : " << box->entries().size() << std::endl;

      auto entries = box->entries();
      for (size_t i = 0; i < entries.size() && i < maxEntryPrintNr; ++i) {
        std::cout << treeSpaces << "-- Elst Entry       : {";
        std::cout << " Segment Duration: " << entries[i].segmentDuration << ",";
        std::cout << " Media Time: " << entries[i].mediaTime << ",";
        std::cout << " Media Rate Integer: " << entries[i].mediaRateInteger << ",";
        std::cout << " Media Rate Fraction: " << entries[i].mediaRateFraction;
        std::cout << " }";
        std::cout << std::endl;
      }

      if (maxEntryPrintNr < entries.size()) {
        std::cout << treeSpaces << "-- Elst Entry       : ... " << entries.size() - maxEntryPrintNr
                  << " more" << std::endl;
      }
      return;
    };

    printMap[(toFcc("vvc1"))] = [&](const BoxItem& item, const std::string& treeSpaces) {
      auto box = std::dynamic_pointer_cast<CVvcSampleEntry>(item);
      std::cout << treeSpaces << "-- Width                  : " << box->width() << std::endl;
      std::cout << treeSpaces << "-- Height                 : " << box->height() << std::endl;
      std::cout << treeSpaces << "-- HorzRes                : " << std::hex << std::uppercase
                << "0x" << std::setfill('0') << std::setw(8)
                << static_cast<uint32_t>(box->horizresolution()) << std::dec << " ("
                << (box->horizresolution() >> 16) << " dpi)" << std::endl;
      std::cout << treeSpaces << "-- VertRes                : " << std::hex << std::uppercase
                << "0x" << std::setfill('0') << std::setw(8)
                << static_cast<uint32_t>(box->horizresolution()) << std::dec << " ("
                << (box->vertresolution() >> 16) << " dpi)" << std::endl;
      std::cout << treeSpaces << "-- Frame Count per Sample : " << box->framecount() << std::endl;
      std::cout << treeSpaces << "-- Compressor Name        : " << box->compressorname()
                << std::endl;
      std::cout << treeSpaces << "-- Depth [Coded, no bits] : " << std::hex << std::uppercase
                << "0x" << std::setfill('0') << std::setw(4) << static_cast<uint32_t>(box->depth())
                << std::dec << std::endl;
      return;
    };

    printMap[(toFcc("vvi1"))] = printMap[(toFcc("vvc1"))];

    printMap[(toFcc("vvcC"))] = [&](const BoxItem& item, const std::string& treeSpaces) {
      auto decoderConfigBox = std::dynamic_pointer_cast<CDecoderConfigurationFullBox>(item);

      if (decoderConfigBox == nullptr) {
        return;
      }

      std::cout << treeSpaces << "-- BoxVersion                  : "
                << std::to_string(decoderConfigBox->version()) << std::endl;
      std::cout << treeSpaces << "-- BoxFlags                    : " << decoderConfigBox->flags()
                << std::endl;
      std::cout << treeSpaces << "-- ###ConfigRecordData###" << std::endl;

      ByteBuffer decoderConfigBlob = decoderConfigBox->decoderConfiguration();
      ByteBuffer::const_iterator iter = decoderConfigBlob.begin();

      auto configRecord = CVvcDecoderConfigRecord(iter, decoderConfigBlob.end());
      std::cout << treeSpaces << "-- LengthSizeMinusOne          : "
                << std::to_string(configRecord.lengthSizeMinusOne()) << std::endl;
      std::cout << treeSpaces << "-- PtlPresentFlag              : "
                << std::to_string(configRecord.vvcPtlPresent()) << std::endl;

      if (configRecord.vvcPtlPresent()) {
        const auto& pptl = configRecord.vvcPtl();
        std::cout << treeSpaces
                  << "-- OlsIdx                      : " << std::to_string(pptl.olsIdx)
                  << std::endl;
        std::cout << treeSpaces
                  << "-- NumSublayers                : " << std::to_string(pptl.numSublayers)
                  << std::endl;
        std::cout << treeSpaces
                  << "-- ConstantFramerate           : " << std::to_string(pptl.constantFrameRate)
                  << std::endl;
        std::cout << treeSpaces
                  << "-- ChomaFromatIdc              : " << std::to_string(pptl.chromaFormatIdc)
                  << std::endl;
        std::cout << treeSpaces
                  << "-- BitDepthMinus8              : " << std::to_string(pptl.bitDepthMinus8)
                  << std::endl;
        std::cout << treeSpaces << "-- ###VvcPtlRecord/NativePtl)###" << std::endl;
        std::cout << treeSpaces << "-- GeneralProfileIdc           : "
                  << std::to_string(pptl.nativePtl.generalProfileIdc) << std::endl;
        std::cout << treeSpaces << "-- GeneralTierFlag             : "
                  << std::to_string(pptl.nativePtl.generalTierFlag) << std::endl;
        std::cout << treeSpaces << "-- GeneralLevelIdc             : "
                  << std::to_string(pptl.nativePtl.generalLevelIdc) << std::endl;
        std::cout << treeSpaces << "-- PtlFrameOnlyConstaintFlag   : "
                  << std::to_string(pptl.nativePtl.ptlFrameOnlyConstraintFlag) << std::endl;
        std::cout << treeSpaces << "-- PtlMultilayerEnabledFlag    : "
                  << std::to_string(pptl.nativePtl.ptlMultiLayerEnabledFlag) << std::endl;
        std::cout << treeSpaces << "-- GeneralConstraintInfo       : " << std::hex
                  << std::uppercase;
        for (uint8_t value : pptl.nativePtl.generalConstraintInfo) {
          std::cout << "0x" << std::setfill('0') << std::setw(2) << std::to_string(value) << ' ';
        }
        std::cout << std::nouppercase << std::dec << std::endl;

        if (pptl.nativePtl.sublayerLevelIdcs.empty()) {
          std::cout << treeSpaces << "-- SublayerLevelIdcs           : { None }" << std::endl;
        } else {
          for (const auto& ptlSubglayerFlag : pptl.nativePtl.sublayerLevelIdcs) {
            std::cout << treeSpaces << "-- SublayerLevelIdc          : { Layer: "
                      << std::to_string(ptlSubglayerFlag.first)
                      << ", Value: " << std::to_string(ptlSubglayerFlag.second) << "}" << std::endl;
          }
        }

        std::cout << treeSpaces << "-- GeneralSubProfileIdcs       : { ";
        if (pptl.nativePtl.generalSubProfileIdcs.empty()) {
          std::cout << "None";
        } else {
          for (auto subProfileIdcs : pptl.nativePtl.generalSubProfileIdcs) {
            std::cout << treeSpaces << subProfileIdcs << ", ";
          }
        }
        std::cout << " }" << std::endl;

        std::cout << treeSpaces << "-- MaxPictureWidth             : " << pptl.maxPictureWidth
                  << std::endl;
        std::cout << treeSpaces << "-- MaxPictureHeight            : " << pptl.maxPictureHeight
                  << std::endl;
        std::cout << treeSpaces << "-- AvgFrameRate                : " << pptl.avgFrameRate
                  << std::endl;
      }

      std::cout << treeSpaces << "-- NumOfArrays                 : "
                << std::to_string(configRecord.nonVclArrays().size()) << std::endl;

      for (const auto& nonVCLArray : configRecord.nonVclArrays()) {
        std::cout << treeSpaces
                  << "-- NonVCL-Nalu: { NaluType: " << static_cast<uint32_t>(nonVCLArray.naluType);
        std::cout << ",  NaluCount: " << nonVCLArray.nalus.size();
        std::cout << ",  ArrayCompleteness: " << nonVCLArray.arrayCompleteness;
        std::cout << ",  NaluSizes: { ";

        for (const auto& nalu : nonVCLArray.nalus) {
          std::cout << nalu.size() << ", ";
        }
        std::cout << " } }" << std::endl;
      }

      return;
    };

    printMap[(toFcc("btrt"))] = [&](const BoxItem& item, const std::string& treeSpaces) {
      auto box = std::dynamic_pointer_cast<CBitRateBox>(item);
      std::cout << treeSpaces << "-- Buffer Size DB  : " << box->bufferSizeDB() << std::endl;
      std::cout << treeSpaces << "-- Max Bitrate     : " << box->maxBitrate() << std::endl;
      std::cout << treeSpaces << "-- Average Bitrate : " << box->avgBitrate() << std::endl;
      return;
    };

    printMap[(toFcc("sidx"))] = [&](const BoxItem& item, const std::string& treeSpaces) {
      auto box = std::dynamic_pointer_cast<CSegmentIndexBox>(item);

      std::cout << treeSpaces
                << "-- BoxVersion                  : " << std::to_string(box->version())
                << std::endl;
      std::cout << treeSpaces << "-- BoxFlags                    : " << box->flags() << std::endl;
      std::cout << treeSpaces << "-- Reference Id                : " << box->referenceId()
                << std::endl;
      std::cout << treeSpaces << "-- Timescale                   : " << box->timescale()
                << std::endl;
      std::cout << treeSpaces
                << "-- Earliest Presentation Time  : " << box->earliestPresentationTime()
                << std::endl;
      std::cout << treeSpaces << "-- First Offset                : " << box->firstOffset()
                << std::endl;
      std::cout << treeSpaces << "-- Reference Count             : " << box->referenceCount()
                << std::endl;

      const auto& refs = box->references();

      for (size_t i = 0; i < refs.size(); ++i) {
        std::cout << treeSpaces << "  -- Reference[" << i + 1 << "/" << refs.size() << "]"
                  << std::endl;
        std::cout << treeSpaces << "    -- Reference Type          : " << refs[i].referenceType
                  << std::endl;
        std::cout << treeSpaces << "    -- Reference Size          : " << refs[i].referenceSize
                  << std::endl;
        std::cout << treeSpaces << "    -- Subsegment Duration     : " << refs[i].subsegmentDuration
                  << std::endl;
        std::cout << treeSpaces << "    -- Starts With Sap         : " << refs[i].startsWithSap
                  << std::endl;
        std::cout << treeSpaces
                  << "    -- SapType                 : " << std::to_string(refs[i].sapType)
                  << std::endl;
        std::cout << treeSpaces << "    -- Sap Delta Time          : " << refs[i].sapDeltaTime
                  << std::endl;
      }
    };
  }

  uint64_t timescale = 0;
  size_t maxEntryPrintNr = 20;

  std::map<Fourcc, std::function<void(const BoxItem&, const std::string&)>> printMap;
};

void doWork(int argc, char** argv) {
  std::string fileUri = std::string(argv[1]);
  bool logging = false;
  BoxPrinter pm;

  for (int i = 2; i < argc; ++i) {
    if (std::string(argv[i]) == "enableLogging") {
      logging = true;
    } else {
      pm.maxEntryPrintNr = static_cast<size_t>(std::stoi(std::string(argv[i])));
    }
  }

  if (!logging) {
    disableLogging();
  } else {
    setLogLevel(LogLevel::verbose);
    redirectLoggingToFile("mmtisobmff_print_mp4_boxes.log", RedirectMode::overwrite);
  }

  mmt::isobmff::setupServicesOnce();

  std::unique_ptr<IIsobmffInput> input = ilo::make_unique<CIsobmffFileInput>(fileUri);

  BoxTree tree;

  try {
    parseTree(tree, input);
  } catch (std::exception& e) {
    std::cerr << "Exception occured: " << e.what() << std::endl;
    std::cerr << "Printing successfully parsed content." << std::endl;
  }

  visitAllOf(tree, [&pm](const BoxElement& boxElement, int32_t level) {
    std::string boxName = toString(boxElement.item->type());
    uint64_t boxSize = boxElement.item->size();

    std::string treeSpaces(level, ' ');
    std::cout << treeSpaces << boxName << " (" << boxSize << ")" << std::endl;
    treeSpaces.append("|");

    if (std::dynamic_pointer_cast<box::CInvalidBox>(boxElement.item) != nullptr) {
      std::cout << treeSpaces << " <invalid box>" << std::endl;
      return;
    }

    if (std::dynamic_pointer_cast<box::CUnknownBox>(boxElement.item) != nullptr) {
      std::cout << treeSpaces << " <unknown box>" << std::endl;
      return;
    }

    if (std::dynamic_pointer_cast<box::CContainerBox>(boxElement.item) != nullptr) {
      return;
    }

    try {
      pm.printMap.at(boxElement.item->type())(boxElement.item, treeSpaces);
    } catch (std::out_of_range&) {
      // Printing is not implemented for this box.
      std::cout << treeSpaces << " <unknown details>" << std::endl;
    }
  });

  auto overheadInfo = calculateOverhead(tree);
  uint64_t totalSize = overheadInfo.sizeOverhead + overheadInfo.sizePayload;
  std::cout << "\n\nOverhead Info: " << std::endl;
  std::cout << "--Total Size: " << totalSize << " [Byte]" << std::endl;
  std::cout << "--Payload Size: " << overheadInfo.sizePayload << " [Byte]" << std::endl;
  std::cout << "--Overhead: " << overheadInfo.sizeOverhead << " [Byte] ("
            << (float)overheadInfo.sizeOverhead * 100 / totalSize << "%)" << std::endl;

  // Check if we actually have moov data to print some extra infos.
  if (findFirstBoxWithFourccAndType<box::CContainerBox>(tree, ilo::toFcc("moov"))) {
    extraInfo(argc, argv);
  } else {
    std::cout << "\nInfo: Extra Info block is not being printed "
                 "since this file does not seem to have a 'moov' box."
              << std::endl;
  }
}

int main(int argc, char** argv) {
  if (argc < 2 || argc > 4) {
    std::cout << "Usage: printMP4 <File-URI> [maxEntryPrintNr] ['enableLogging']" << std::endl;
    return 1;
  }

  try {
    doWork(argc, argv);
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
#if (_WIN32 && _MSC_VER == 1800)
#pragma warning(pop)
#endif
