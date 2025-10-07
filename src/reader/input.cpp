/*-----------------------------------------------------------------------------
Software License for The Fraunhofer FDK MPEG-H Software

Copyright (c) 2016 - 2025 Fraunhofer-Gesellschaft zur Förderung der angewandten
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
 * Content: mmtisobmff input class(es)
 */

// System includes
#include <stdexcept>

// External includes

// Internal includes
#include "mmtisobmff/reader/input.h"
#include "common/logging.h"

#if defined(WIN32) || defined(_WIN32)
#define SEEK_OFFSET_T int64_t
#elif defined __ANDROID__ && __ANDROID_API__ >= 24
#define SEEK_OFFSET_T off64_t
#else
#define SEEK_OFFSET_T off_t
#endif

namespace mmt {
namespace isobmff {
size_t CIsobmffFileInput::read(ilo::ByteBuffer::iterator inBegin, ilo::ByteBuffer::iterator inEnd) {
  size_t len = static_cast<size_t>(inEnd - inBegin);
  char* buffer = reinterpret_cast<char*>(&(*inBegin));
  size_t actuallyRead = fread(buffer, sizeof(uint8_t), len, m_file.get());

  return actuallyRead;
}

void CIsobmffFileInput::seek(pos_type pos) {
  int err = ilo_fseeko(m_file.get(), static_cast<SEEK_OFFSET_T>(pos), SEEK_SET);
  ILO_ASSERT(err == 0, "Could not seek to position");
}

void CIsobmffFileInput::seek(offset_type offset, SeekingOrigin origin) {
  int err = 0;

  switch (origin) {
    case SeekingOrigin::beg:
      seek(static_cast<pos_type>(offset));
      break;
    case SeekingOrigin::end:
      err = ilo_fseeko(m_file.get(), static_cast<SEEK_OFFSET_T>(offset), SEEK_END);
      break;
    case SeekingOrigin::cur:
      err = ilo_fseeko(m_file.get(), static_cast<SEEK_OFFSET_T>(offset), SEEK_CUR);
      break;
  }

  ILO_ASSERT(err == 0, "Could not seek to position");
}

bool CIsobmffFileInput::isEOI() {
  auto c = fgetc(m_file.get());
  ungetc(c, m_file.get());
  return feof(m_file.get()) != 0;
}

size_t CIsobmffMemoryInput::read(ilo::ByteBuffer::iterator inBegin,
                                 ilo::ByteBuffer::iterator inEnd) {
  auto copyCount = std::min(inEnd - inBegin, buffer->end() - ptr);

  std::copy(ptr, ptr + copyCount, inBegin);

  ptr += copyCount;
  return static_cast<size_t>(copyCount);
}

void CIsobmffMemoryInput::seek(pos_type pos) {
  ILO_ASSERT_WITH(pos <= buffer->size(), std::out_of_range, "Position to seek to is out of range");
  ptr = buffer->begin() + static_cast<ilo::ByteBuffer::difference_type>(pos);
}

void CIsobmffMemoryInput::seek(offset_type offset, SeekingOrigin origin) {
  switch (origin) {
    case SeekingOrigin::beg:
      seek((pos_type)offset);
      break;
    case SeekingOrigin::end:
      ILO_ASSERT_WITH(offset <= 0 && buffer->size() >= static_cast<uint64_t>(std::abs(offset)),
                      std::out_of_range, "Position to seek to is out of range");
      ptr = buffer->end() + static_cast<ilo::ByteBuffer::difference_type>(offset);
      break;
    case SeekingOrigin::cur:
      ILO_ASSERT_WITH(ptr + static_cast<std::ptrdiff_t>(offset) >= buffer->begin() &&
                          ptr + static_cast<std::ptrdiff_t>(offset) <= buffer->end(),
                      std::out_of_range, "Position to seek to is out of range");
      ptr += static_cast<ilo::ByteBuffer::difference_type>(offset);
      break;
  }
}
}  // namespace isobmff
}  // namespace mmt
