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
 * Content: MHA Profile and Level Compatibility Set Box
 */

// System headers
#include <limits>

// External headers
#include "ilo/bytebuffertools.h"

// Internal headers
#include "mhapbox.h"
#include "common/logging.h"

namespace mmt {
namespace isobmff {
namespace box {

CMhaProfileLevelCompatibilitySetBox::CMhaProfileLevelCompatibilitySetBox(
    ilo::ByteBuffer::const_iterator& begin, const ilo::ByteBuffer::const_iterator& end)
    : CBox(begin, end), m_profileAndLevelCompatibleSets() {
  parseBox(begin, end);
}

CMhaProfileLevelCompatibilitySetBox::CMhaProfileLevelCompatibilitySetBox(
    const SMhaPBoxWriteConfig& config)
    : CBox(config), m_profileAndLevelCompatibleSets(config.profileAndLevelCompatibleSets) {
  updateSize(0);
  ILO_ASSERT(config.profileAndLevelCompatibleSets.size() <= std::numeric_limits<uint8_t>::max(),
             "Number of profile and level compatible sets exceeds the maximum number supported by "
             "the mhaP box");
}

void CMhaProfileLevelCompatibilitySetBox::parseBox(ilo::ByteBuffer::const_iterator& begin,
                                                   const ilo::ByteBuffer::const_iterator& end) {
  ILO_ASSERT_WITH(CBox::type() == ilo::toFcc("mhaP"), std::invalid_argument,
                  "Expected box type mhaP, but found: %s", ilo::toString(CBox::type()).c_str());

  uint8_t numCompatibleSets = ilo::readUint8(begin, end);

  for (uint8_t i = 0; i < numCompatibleSets; i++) {
    m_profileAndLevelCompatibleSets.push_back(ilo::readUint8(begin, end));
  }
}

void CMhaProfileLevelCompatibilitySetBox::updateSize(uint64_t sizeValue) {
  CBox::updateSize(sizeValue + 1 + m_profileAndLevelCompatibleSets.size());
}

SAttributeList CMhaProfileLevelCompatibilitySetBox::getAttributeList() const {
  SAttributeList attributesList;

  SAttribute attribute;
  attribute.key = "Num Compatible Sets";
  attribute.value = std::to_string(m_profileAndLevelCompatibleSets.size());
  attributesList.push_back(attribute);

  if (!m_profileAndLevelCompatibleSets.empty()) {
    attribute.key = "Compatible Sets Indications";
    std::stringstream ss;
    for (auto sIndication : m_profileAndLevelCompatibleSets) {
      ss << std::to_string(sIndication) << ";";
    }

    attribute.value = ss.str();
    attribute.value = attribute.value.substr(0, attribute.value.size() - 1);
    attributesList.push_back(attribute);
  }

  return attributesList;
}

void CMhaProfileLevelCompatibilitySetBox::writeBox(ilo::ByteBuffer& buffer,
                                                   ilo::ByteBuffer::iterator& position) const {
  ilo::writeUint8(buffer, position, static_cast<uint8_t>(m_profileAndLevelCompatibleSets.size()));
  for (uint8_t indication : m_profileAndLevelCompatibleSets) {
    ilo::writeUint8(buffer, position, indication);
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

BOXREGISTRY_DECLARE(mhaP, CMhaProfileLevelCompatibilitySetBox,
                    CMhaProfileLevelCompatibilitySetBox::SMhaPBoxWriteConfig,
                    CContainerType::noContainer);
