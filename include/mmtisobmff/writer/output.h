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

/*!
 * @file output.h
 * @brief Interface for os agnostic file and memory output
 * \defgroup output Main interface for handling file and memory output
 *
 * Main interface for file and memory output abstraction
 */

#pragma once

// System includes
#include <string>
#include <cstdlib>

// External includes
#include "ilo/common_types.h"
#include "ilo/memory.h"
#include "ilo/fileio.h"

// Internal includes
#include "mmtisobmff/types.h"

namespace mmt {
namespace isobmff {
/*!
 * @brief Output interface
 *
 * Output interface that is used by @ref CIsobmffWriter to write files to either disk or memory
 */
struct IIsobmffOutput {
  virtual ~IIsobmffOutput() {}

  /*!
   * @brief Write data from a buffer to the output
   *
   * The amount of data is specified by the range between the begin and end iterator of the buffer.
   */
  virtual void write(const ilo::ByteBuffer::const_iterator& inBegin,
                     const ilo::ByteBuffer::const_iterator& inEnd) = 0;
  /*!
   * @brief Read data back from the current open write handle.
   *
   * @note Depending on the configuration of the concrete implementation reading might not be
   * supported.
   */
  virtual ilo::CUniqueBuffer read(size_t offset = 0, size_t size = 0) = 0;
  //! Function to seek to a fixed position in the output stream
  virtual void seek(pos_type pos) = 0;
  //! Function to seek relative to a given origin
  virtual void seek(offset_type offset, SeekingOrigin origin) = 0;
  //! Function to get the current writing position in the stream in bytes
  virtual pos_type tell() = 0;
};

/*!
 * @brief Implementation of a file output writer
 *
 * Can optionally be opened in a special read/write mode by enabling modeWriteExtended.
 *
 * \ingroup output
 */
struct CIsobmffFileOutput : public IIsobmffOutput {
  /*!
   * @brief File output constructor
   *
   * @param filename Path to the output file
   * @param modeWriteExtended If enabled, file is opened in read/write mode. Otherwise, it is opened
   * in write-only mode.
   *
   * @note Output will always clear/overwrite existing files.
   */
  CIsobmffFileOutput(const std::string& filename, const bool modeWriteExtended = false)
      : m_file(filename, modeWriteExtended ? ilo::CFileWrapper::OpenMode::writeExtended
                                           : ilo::CFileWrapper::OpenMode::write),
        m_modeExtended(modeWriteExtended),
        m_fileStreamSize(0) {}

  ~CIsobmffFileOutput() {}

  /*!
   * @brief Write data to disk
   *
   * The amount of data is specified by the range between the begin and end iterator of the buffer.
   */
  virtual void write(const ilo::ByteBuffer::const_iterator& inBegin,
                     const ilo::ByteBuffer::const_iterator& inEnd) override;

  /*!
   * @brief Read data back from the current open write handle.
   *
   * @param offset Position to start reading at relative to the beginning of the file
   * @param size The number of bytes to read. If size is 0, everything from offset to the end is
   * read.
   * @note Only possible for writing mode 'modeWriteExtended'. Reading will not alter the write
   * pointer. Will throw an exception if 'modeWriteExended' is disabled.
   */
  virtual ilo::CUniqueBuffer read(size_t offset = 0, size_t size = 0) override;

  /*!
   * @brief Function to seek to a fixed position in the output file
   *
   * When called, the write pointer for the next @ref write call is set to parameter pos.
   *
   * @param pos Position in bytes relativ to file start at which to continue writing with the next
   * @ref write call
   */
  virtual void seek(pos_type pos) override;

  /*!
   * @brief Function to seek relative to a given origin
   *
   * When called, the pointer where to write to with a future @ref write call is set to an
   * offset relative to origin.
   *
   * @param offset Offset in bytes to seek to (relative to origin).
   *              A positive value indicates seeking towards the end,
   *              a negative value seeking towards the front.
   * @param origin Origin to start seeking at
   */
  virtual void seek(offset_type offset, SeekingOrigin origin) override;

  //! Function to get the current writing position in the stream in bytes
  virtual pos_type tell() override { return static_cast<pos_type>(ilo_ftello(m_file.get())); }

  ilo::CFileWrapper m_file;
  bool m_modeExtended;
  size_t m_fileStreamSize;
};

/*!
 * @brief Implementation of a memory output writer
 *
 * Can write data into a buffer with optional preallocation.
 *
 * \ingroup output
 */
struct CIsobmffMemoryOutput : IIsobmffOutput {
  /*!
   * @brief Memory output constructor
   *
   * This output works on an internally managed buffer into which data can be independently written
   * to and read from.
   *
   * @param prealloc_size If > 0, reserve this amount of memory to avoid reallocation with every
   * write
   */
  CIsobmffMemoryOutput(uint32_t prealloc_size = 0U) : buffer(prealloc_size), ptr(buffer.begin()) {}

  /*!
   * @brief Write data into the buffer
   *
   * The amount of data is specified by the range between the begin and end iterator of the buffer.
   */
  virtual void write(const ilo::ByteBuffer::const_iterator& inBegin,
                     const ilo::ByteBuffer::const_iterator& inEnd) override;

  /*!
   * @brief Read data from the buffer
   *
   * Read data from the buffer.
   *
   * @param offset Position relative to the start of the buffer
   * @param size Number of bytes to read starting from the given offset. If size is 0, everything
   * from offset to the end is read.
   *
   * @note Read does not store the last read position. It always operates on the given
   * offset relative to the start of the buffer.
   */
  virtual ilo::CUniqueBuffer read(size_t offset = 0, size_t size = 0) override;

  /*!
   * @brief Function to seek to a fixed position in the output buffer
   *
   * When called, the write pointer for the next @ref write call is set to parameter pos.
   *
   * @param pos Position in bytes relative to buffer start at which to continue writing with the
   * next @ref write call.
   */
  virtual void seek(pos_type pos) override;

  /*!
   * @brief Function to seek relative to a given origin
   *
   * When called, the write pointer for the next @ref write call is set to an
   * offset relative to origin.
   *
   * @param offset Offset in bytes to seek to (relative to dir).
   *              A positive value indicates seeking towards the end,
   *              a negative value seeking towards the front.
   * @param origin Origin to start seeking at
   */
  virtual void seek(offset_type offset, SeekingOrigin origin) override;

  //! Function to get the current writing position in the stream in bytes
  virtual pos_type tell() override { return static_cast<pos_type>(ptr - buffer.begin()); }

  ilo::ByteBuffer buffer;

 private:
  ilo::ByteBuffer::iterator ptr;
};
}  // namespace isobmff
}  // namespace mmt
