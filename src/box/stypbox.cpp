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
 * Content: segment type box class
 */

// External headers
#include "ilo/bytebuffertools.h"

// Internal headers
#include "stypbox.h"
#include "common/logging.h"

namespace mmt {
namespace isobmff {
namespace box {

CSegmentTypeBox::CSegmentTypeBox(ilo::ByteBuffer::const_iterator& begin,
                                 const ilo::ByteBuffer::const_iterator& end)
    : CBox(begin, end), m_minorVersion(0), m_majorBrand(), m_compatibleBrands() {
  parse(begin, end);
}

CSegmentTypeBox::CSegmentTypeBox(const SStypBoxWriteConfig& stypBoxData) : CBox(stypBoxData) {
  m_minorVersion = stypBoxData.minorVersion;
  m_majorBrand = stypBoxData.majorBrand;
  m_compatibleBrands = stypBoxData.compatibleBrands;

  updateSize(0);
}

void CSegmentTypeBox::parse(ilo::ByteBuffer::const_iterator& begin,
                            const ilo::ByteBuffer::const_iterator& end) {
  ILO_ASSERT_WITH(CBox::type() == ilo::toFcc("styp"), std::invalid_argument,
                  "Expected box type styp, but found: %s", ilo::toString(CBox::type()).c_str());

  m_majorBrand = ilo::readFourCC(begin, end);
  m_minorVersion = ilo::readUint32(begin, end);

  while (begin < end) {
    ilo::Fourcc currentBrand;

    currentBrand = ilo::readFourCC(begin, end);
    m_compatibleBrands.push_back(currentBrand);
  }
}

void CSegmentTypeBox::updateSize(uint64_t sizeValue) {
  // size + version + brand + comp_brands
  CBox::updateSize(sizeValue + 4 + 4 + m_compatibleBrands.size() * 4);
}

SAttributeList CSegmentTypeBox::getAttributeList() const {
  SAttributeList attributesList;

  SAttribute attribute;
  attribute.key = "Major Brand";
  attribute.value = ilo::toString(m_majorBrand);
  attributesList.push_back(attribute);

  attribute.key = "Minor Version";
  attribute.value = std::to_string(m_minorVersion);
  attributesList.push_back(attribute);

  attribute.key = "Compatible Brands";
  attribute.value = "";
  for (auto compatibleBrand : m_compatibleBrands) {
    attribute.value += ilo::toString(compatibleBrand);
    attribute.value += ", ";
  }
  attribute.value = attribute.value.substr(0, attribute.value.size() - 2);
  attributesList.push_back(attribute);

  return attributesList;
}

void CSegmentTypeBox::writeBox(ilo::ByteBuffer& buffer, ilo::ByteBuffer::iterator& position) const {
  ilo::writeFourCC(buffer, position, m_majorBrand);
  ilo::writeUint32(buffer, position, m_minorVersion);

  for (const auto& brand : m_compatibleBrands) {
    ilo::writeFourCC(buffer, position, brand);
  }
}
}  // namespace box
}  // namespace isobmff
}  // namespace mmt

#include "box/boxregistryentry.h"

using namespace mmt;
using namespace mmt::isobmff;
using namespace mmt::isobmff::box;

BOXREGISTRY_DECLARE(styp, CSegmentTypeBox, CSegmentTypeBox::SStypBoxWriteConfig,
                    CContainerType::noContainer);
