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
 * Content: ES descriptor class
 */

#pragma once

// external include
#include "ilo/common_types.h"

// Internal includes
#include "mmtisobmff/types.h"

#include "basedescriptor.h"
#include "decoderconfigdescriptor.h"
#include "slconfigdescriptor.h"

namespace mmt {
namespace isobmff {
namespace descriptor {
// ES Descriptor after ISO 14496-1
class CESDescriptor : public CBaseDescriptor {
 public:
  struct SESDescriptorWriteConfig : CBaseDescriptor::SBaseDescriptorWriteConfig {
    uint16_t ES_id = 0;  // According to 14496-14 (3.1.2)
    uint8_t streamDependenceFlag = 0;
    uint8_t URLflag = 0;
    uint8_t OCRstreamFlag = 0;
    uint8_t streamPriority = 0;
    uint16_t dependsOn_ES_ID = 0;
    uint8_t URLlength = 0;
    uint16_t OCR_ES_Id = 0;
    std::vector<uint8_t> URLstring;
    CDecoderConfigDescriptor dcd;
    CSLConfigDescriptor slConfigDescriptor;

    SESDescriptorWriteConfig() : SBaseDescriptorWriteConfig(EDescriptorTag::ESDescriptor) {}
  };

  // constructor to init member variables through parsing
  CESDescriptor(ilo::ByteBuffer::const_iterator& begin, const ilo::ByteBuffer::const_iterator& end);
  // constructor to init member variables by setting
  explicit CESDescriptor(const SESDescriptorWriteConfig& descriptorData);

 public:
  uint16_t ES_id() { return m_ES_id; }
  uint8_t streamDependenceFlag() { return m_streamDependenceFlag; }
  uint8_t URLflag() { return m_URLflag; }
  uint8_t OCRstreamFlag() { return m_OCRstreamFlag; }
  uint8_t streamPriority() { return m_streamPriority; }
  uint16_t dependsOn_ES_ID() { return m_dependsOn_ES_ID; }
  uint8_t URLlength() { return m_URLlength; }
  std::vector<uint8_t> URLstring() { return m_URLstring; }
  uint16_t OCR_ES_Id() { return m_OCR_ES_Id; }
  ilo::ByteBuffer remainingPayload() { return m_remainingPayload; }
  CDecoderConfigDescriptor decoderConfigDescriptor() { return m_dcd; }
  CSLConfigDescriptor slConfigDescriptor() { return m_slConfigDescriptor; }

  SAttributeList getAttributeList() const override;

 protected:
  void updateSize(uint32_t size) final;

  // function to write the descriptor-content
  void writeDescriptor(ilo::ByteBuffer& buffer, ilo::ByteBuffer::iterator& position) const override;

 private:
  // function to parse the descriptor
  void parse(ilo::ByteBuffer::const_iterator& begin, const ilo::ByteBuffer::const_iterator& end);

  uint16_t m_ES_id;
  uint8_t m_streamDependenceFlag;
  uint8_t m_URLflag;
  uint8_t m_OCRstreamFlag;
  uint8_t m_streamPriority;
  uint16_t m_dependsOn_ES_ID;
  uint8_t m_URLlength;
  uint16_t m_OCR_ES_Id;
  std::vector<uint8_t> m_URLstring;
  CDecoderConfigDescriptor m_dcd;
  CSLConfigDescriptor m_slConfigDescriptor;
  ilo::ByteBuffer m_remainingPayload;
};
}  // namespace descriptor
}  // namespace isobmff
}  // namespace mmt
