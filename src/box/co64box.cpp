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
 * Content: 64-bit chunk offset box class
 */

// System headers
#include <stdexcept>
#include <limits>

// External headers
#include "ilo/bytebuffertools.h"

// Internal headers
#include "co64box.h"
#include "common/logging.h"

namespace mmt {
namespace isobmff {
namespace box {

CChunkOffset64Box::CChunkOffset64Box(ilo::ByteBuffer::const_iterator& begin,
                                     const ilo::ByteBuffer::const_iterator& end)
    : CFullBox(begin, end) {
  parse(begin, end);
}

CChunkOffset64Box::CChunkOffset64Box(const SCo64BoxWriteConfig& co64BoxData)
    : CFullBox(co64BoxData), m_chunkOffsets(co64BoxData.chunkOffsets) {
  updateSize(0);
}

std::vector<uint64_t> CChunkOffset64Box::chunkOffsets() const {
  return m_chunkOffsets;
}

void CChunkOffset64Box::addChunkOffset(uint64_t chunkOffset) {
  ILO_ASSERT(
      m_chunkOffsets.size() + 1 <= std::numeric_limits<uint32_t>::max(),
      "Size of the chunk offsets vector exceeds the maximum length supported by the co64 box.");

  m_chunkOffsets.push_back(chunkOffset);
  updateSize(0);
}

void CChunkOffset64Box::parse(ilo::ByteBuffer::const_iterator& begin,
                              const ilo::ByteBuffer::const_iterator& end) {
  ILO_ASSERT_WITH(CBox::type() == ilo::toFcc("co64"), std::invalid_argument,
                  "Expected box type co64, but found: %s", ilo::toString(CBox::type()).c_str());

  ILO_ASSERT(CFullBox::version() == 0, "Version %s of co64 is not defined/implemented",
             std::to_string(CFullBox::version()).c_str());

  uint32_t entryCount = ilo::readUint32(begin, end);
  ILO_ASSERT_WITH(static_cast<uint32_t>(end - begin) >= entryCount, std::out_of_range,
                  "Malformed co64 box");
  m_chunkOffsets.resize(entryCount);

  for (uint32_t i = 0; i < entryCount; ++i) {
    m_chunkOffsets[i] = ilo::readUint64(begin, end);
  }
}

void CChunkOffset64Box::updateSize(uint64_t sizeValue) {
  // size + entry_count + chunk_offsets
  CFullBox::updateSize(sizeValue + 4 + m_chunkOffsets.size() * 8);
}

SAttributeList CChunkOffset64Box::getAttributeList() const {
  SAttributeList attributesList;

  SAttribute attribute;
  attribute.key = "Entry Count";
  attribute.value = std::to_string(static_cast<uint32_t>(m_chunkOffsets.size()));
  attributesList.push_back(attribute);

  if (!m_chunkOffsets.empty()) {
    attribute.key = "Chunk Offsets";
    std::stringstream ss;
    for (auto offset : m_chunkOffsets) {
      ss << std::to_string(offset) << ";";
    }

    attribute.value = ss.str();
    attribute.value = attribute.value.substr(0, attribute.value.size() - 1);
    attributesList.push_back(attribute);
  }

  return attributesList;
}

void CChunkOffset64Box::writeBox(ilo::ByteBuffer& buffer,
                                 ilo::ByteBuffer::iterator& position) const {
  ILO_ASSERT(
      m_chunkOffsets.size() <= std::numeric_limits<uint32_t>::max(),
      "Size of the chunk offsets vector exceeds the maximum length supported by the co64 box.");

  ilo::writeUint32(buffer, position, static_cast<uint32_t>(m_chunkOffsets.size()));

  for (uint64_t chunkOffset : m_chunkOffsets) {
    ilo::writeUint64(buffer, position, chunkOffset);
  }
}

}  // namespace box
}  // namespace isobmff
}  // namespace mmt

#include "box/boxregistryentry.h"
#include <memory>

using namespace mmt;
using namespace mmt::isobmff;
using namespace mmt::isobmff::box;

BOXREGISTRY_DECLARE(co64, CChunkOffset64Box, CChunkOffset64Box::SCo64BoxWriteConfig,
                    CContainerType::noContainer);
