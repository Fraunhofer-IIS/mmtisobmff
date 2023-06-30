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
 * Content: track run box class
 */

#pragma once

// System headers
#include <vector>

// External headers
#include "ilo/string_utils.h"

// Internal headers
#include "mmtisobmff/types.h"
#include "box.h"

namespace mmt {
namespace isobmff {
namespace box {

class CTrunEntry {
 public:
  CTrunEntry();
  ~CTrunEntry();

  void setSampleDuration(uint32_t sampleDuration);

  void setSampleSize(uint32_t sampleSize);

  void setSampleFlags(uint32_t sampleFlags);

  void setSampleCtsOffset(int64_t sampleCtsOffset);

  uint32_t sampleDuration() const;

  uint32_t sampleSize() const;

  uint32_t sampleFlags() const;

  int64_t sampleCtsOffset() const;

 private:
  uint32_t m_duration;
  uint32_t m_size;
  uint32_t m_flags;
  int64_t m_ctsOffset;
  bool m_durationPresent;
  bool m_sizePresent;
  bool m_flagsPresent;
  bool m_ctsOffsetPresent;
};

class CTrackRunBox : public CFullBox {
 public:
  struct STrunBoxWriteConfig : SFullBoxWriteConfig {
    bool dataOffsetPresent = false;
    bool firstSampleFlagsPresent = false;
    bool sampleDurationPresent = true;
    bool sampleSizePresent = true;
    bool sampleFlagsPresent = false;
    bool sampleCtsOffsetPresent = false;
    uint32_t sampleCount = 0;
    uint32_t dataoffset = 0;
    uint32_t firstSampleFlags = 0;
    std::vector<CTrunEntry> trunEntries;

    STrunBoxWriteConfig() : SFullBoxWriteConfig(ilo::toFcc("trun"), 0, 301) {}
  };

  //! constructor to init member variables through parsing
  CTrackRunBox(ilo::ByteBuffer::const_iterator& begin, const ilo::ByteBuffer::const_iterator& end);

  //! constructor to init member variables by setting
  explicit CTrackRunBox(const STrunBoxWriteConfig& trunWriteConfig);

 public:
  bool dataOffsetPresent() const { return (flags() & 0x1u) != 0; }

  bool firstSampleFlagsPresent() const { return (flags() & 0x4u) != 0; }

  bool sampleDurationPresent() const { return (flags() & 0x100u) != 0; }

  bool sampleSizePresent() const { return (flags() & 0x200u) != 0; }

  bool sampleFlagsPresent() const { return (flags() & 0x400u) != 0; }

  bool sampleCtsOffsetPresent() const { return (flags() & 0x800u) != 0; }

  uint32_t sampleCount() const { return m_sampleCount; }

  uint32_t dataOffset() const;

  uint32_t firstSampleFlags() const;

  const std::vector<CTrunEntry>& trunEntries() const { return m_trunEntries; }

  SAttributeList getAttributeList() const override;

 protected:
  void updateSize(uint64_t size) final;

  void writeBox(ilo::ByteBuffer& buffer, ilo::ByteBuffer::iterator& position) const override;

 private:
  void parseBox(ilo::ByteBuffer::const_iterator& begin, const ilo::ByteBuffer::const_iterator& end);

  void flagSanityCheck();

 private:
  uint32_t m_sampleCount;
  int32_t m_dataOffset;
  uint32_t m_firstSampleFlags;
  std::vector<CTrunEntry> m_trunEntries;
};
}  // namespace box
}  // namespace isobmff
}  // namespace mmt