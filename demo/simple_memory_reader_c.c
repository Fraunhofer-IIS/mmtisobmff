/*-----------------------------------------------------------------------------
Software License for The Fraunhofer FDK MPEG-H Software

Copyright (c) 2019 - 2023 Fraunhofer-Gesellschaft zur Förderung der angewandten
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
 * Content: Simple demo how to use the Memory Reader using the C interface
 */

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>

#include "mmtisobmff/mmtisobmff_c.h"

int readFileIntoMemory(const char* inputFile, uint8_t** memoryFile, size_t* memoryFileSize) {
  FILE* file = NULL;
  size_t size = 0;

  printf("Reading input file %s into memory\n\n", inputFile);

#if defined(WIN32) || defined(_WIN32)
  fopen_s(&file, inputFile, "rb");
#else
  file = fopen(inputFile, "rb");
#endif

  if (file == NULL) {
    printf("Error: Unable to read input file %s into memory.\n", inputFile);
    return 1;
  }

  if (fseek(file, 0, SEEK_END) != 0) {
    printf("Error: Unable to perform seek operation on input file %s.\n", inputFile);
    fclose(file);
    return 1;
  }

  size = ftell(file);
  if (size <= 0) {
    printf("Error: Unable to optain file size of input file %s.\n", inputFile);
    fclose(file);
    return 1;
  }

  if (fseek(file, 0, SEEK_SET) != 0) {
    printf("Error: Unable to perform seek operation on input file %s.\n", inputFile);
    fclose(file);
    return 1;
  }

  *memoryFile = (uint8_t*)malloc(size * sizeof(uint8_t));
  if (*memoryFile == NULL) {
    printf("Error: Unable to allocate memory for file %s with size %zu.\n", inputFile, size);
    fclose(file);
    return 1;
  }

  *memoryFileSize = fread(*memoryFile, 1, size, file);

  if (fclose(file) != 0) {
    printf("Warning: Failed to close the file handle after copying data to memoryFile.\n");
  }
  file = NULL;

  if (*memoryFileSize != size) {
    printf(
        "Error: Unable to read input file %s into memory! File size is %zu, but data read was %zu "
        "Bytes.\n",
        inputFile, size, *memoryFileSize);
    free(*memoryFile);
    *memoryFile = NULL;
    *memoryFileSize = 0;
    return 1;
  }
  return 0;
}

void printErrorType(ISOBMFF_ERR err) {
  switch (err) {
    case ISOBMFF_OK:
      printf("Error: HandleError was called with error being ISOBMFF_OK.\n");
      break;
    case ISOBMFF_UNKNOWN_ERR:
      printf("Error: An unknown error has occured.\n");
      break;
    case ISOBMFF_PARAM_ERR:
      printf("Error: An error related to the input parameters occured.\n");
      break;
    case ISOBMFF_ALREADY_INIT_ERR:
      printf(
          "Error: The handle has already a created instance attached to it. "
          "Create cannot be called twice with the same handle.\n");
      break;
    case ISOBMFF_LIB_ERR:
      printf("Error: Library reported an error.\n");
      break;
    case ISOBMFF_NOT_IMPL_ERR:
      printf("Error: Feature is not implemented.\n");
      break;
    default:
      printf("Error: No error printouts available.\n");
      break;
  }
}

void prettyPrintTrackType(const TrackType_C trackType) {
  switch (trackType) {
    case TrackType_Audio:
      printf("Audio\n");
      break;
    case TrackType_Video:
      printf("Video\n");
      break;
    case TrackType_Hint:
      printf("Hint\n");
      break;
    case TrackType_Undefined:
      printf("Undefined\n");
      break;
    default:
      printf("Track type unknown\n");
      break;
  }
}

void prettyPrintFourCC(const char* fourCC, uint32_t size) {
  uint32_t i = 0;

  for (i = 0; i < size; i++) {
    printf("%c", fourCC[i]);
  }

  if (size != 4) {
    printf(" (Warning: FourCC should have 4 characters)");
  }
  printf("\n");
}

ISOBMFF_ERR printDscInfo(TrackReader* track) {
  ISOBMFF_ERR err = ISOBMFF_OK;
  uint8_t* dcrData = NULL;
  uint32_t dcrDataSize = 0;
  uint32_t i = 0;

  printf("Dsc Info: \n");
  printf("########################################\n");

  err = isobmff_getDecoderSpecificConfig(track, &dcrData, &dcrDataSize);
  if (err) {
    printf("Error: Failed to get decoder specific config.\n");
    return err;
  }

  printf("Decoder Specific Config available?: ");
  if (dcrDataSize > 0) {
    printf("Yes\n");
    printf("Dsc data: ");
    for (i = 0; i < dcrDataSize; i++) {
      printf("%02x ", dcrData[i]);
    }
    printf("\n");
  } else {
    printf("No\n");
  }
  printf("\n");

  return err;
}

int main(int argc, char* argv[]) {
  ISOBMFF_ERR err = ISOBMFF_OK;
  ISOBMFF_Reader* isobmff = NULL;

  TrackReader* track = NULL;
  uint32_t trackCount = 0;
  uint32_t trackId = 0;
  char* handler = NULL;
  uint32_t handlerSize = 0;
  char* codingName = NULL;
  uint32_t codingNameSize = 0;
  TrackType_C trackType = TrackType_Undefined;
  uint64_t trackDuration = 0;
  uint32_t trackTimeScale = 0;

  uint32_t i = 0;

  uint64_t maxSampleSize;
  Sample* sample = NULL;
  uint8_t* sampleData = NULL;
  uint64_t sampleSize = 0;
  uint64_t sampleCounter = 0;

  uint8_t* memoryFile = NULL;
  size_t memoryFileSize = 0;

  if (argc != 2) {
    printf("Usage: simpleMemoryReader_c <File-URI>\n\n");
    goto error;
  }

  /* Code preparing a byte buffer from an actual file. This is just to demonstrate
   * the memory interface of mmtisobmff. */
  if (readFileIntoMemory(argv[1], &memoryFile, &memoryFileSize) != 0 || memoryFile == NULL ||
      memoryFileSize == 0) {
    goto error;
  }

  /* Starting point of the actual ISOBMFF calling sequence */
  err = isobmff_disableLogging();
  if (err) {
    printf("Error: Disabling logging failed.\n");
    goto error;
  }

  /* Create an ISOBMFF_MemoryReader instance and initialize the reader.
   * It also makes a copy of the buffer. */
  err = isobmff_createMemoryReader(&isobmff, memoryFile, memoryFileSize);
  if (err) {
    printf("Error: Creation of isobmff instance failed.\n");
    goto error;
  }

  /* For this simple demo we just free the memory block afterwards. */
  free(memoryFile);
  memoryFile = NULL;

  err = isobmff_getTrackCount(isobmff, &trackCount);
  if (err) {
    printf("Error: Can't query number of tracks.\n");
    goto error;
  }

  /* Get some information about the available tracks */
  printf("Found %u tracks in input file.\n", trackCount);

  for (i = 0; i < trackCount; ++i) {
    err = isobmff_getTrack(isobmff, &track, i);
    if (err) {
      printf("Error: Failed to get track.\n");
      goto error;
    }

    err = isobmff_getTrackId(track, &trackId);
    if (err) {
      printf("Error: Failed to get track id.\n");
      goto error;
    }

    err = isobmff_getTrackHandler(track, &handler, &handlerSize);
    if (err) {
      printf("Error: Failed to get track handler.\n");
      goto error;
    }

    err = isobmff_getTrackCodingName(track, &codingName, &codingNameSize);
    if (err) {
      printf("Error: Failed to get track coding name.\n");
      goto error;
    }

    err = isobmff_getTrackType(track, &trackType);
    if (err) {
      printf("Error: Failed to get track track type.\n");
      goto error;
    }

    err = isobmff_getTrackDuration(track, &trackDuration);
    if (err) {
      printf("Error: Failed to get track duration.\n");
      goto error;
    }

    err = isobmff_getTrackTimeScale(track, &trackTimeScale);
    if (err) {
      printf("Error: Failed to get track time scale.\n");
      goto error;
    }

    printf("########################################\n");
    printf("-TrackInfo: \n");
    printf("-- ID       : %u\n", trackId);
    printf("-- Handler  : ");
    prettyPrintFourCC(handler, handlerSize);
    printf("-- Type     : ");
    prettyPrintTrackType(trackType);
    printf("-- Codec    : ");
    prettyPrintFourCC(codingName, codingNameSize);
    printf("-- Duration : %" PRIu64 "\n", trackDuration);
    printf("-- Timescale: %u\n", trackTimeScale);

    printf("\nCreating reader for track with ID %u ... ", trackId);

    printf("Done.\n\n");

    /* Get decoder config record data */
    err = printDscInfo(track);
    if (err) {
      printf("Error: Failed to print decoder config record data.\n");
      goto error;
    }

    /* Get the size of the biggest sample in the track */
    err = isobmff_getMaxSampleSize(track, &maxSampleSize);
    if (err) {
      printf("Error: Failed to get max sample size.\n");
      goto error;
    }

    printf("Sample Info:\n");
    printf("########################################\n");
    printf("Max Sample Size: %" PRIu64 " Bytes\n", maxSampleSize);
    printf("\n");

    printf("Reading all samples of this track\n");
    printf("########################################\n");

    /* Create a sample and pre-allocate it with max sample size to avoid
       re-allocation of memory. The sample can be re-used for each nextSample call. */
    err = isobmff_createSample(&sample, maxSampleSize);
    if (err) {
      printf("Error: Failed to create a sample.\n");
      goto error;
    }

    /* Get all samples in order. Each call fetches the next sample. */
    do {
      err = isobmff_getNextSample(track, sample);
      if (err) {
        printf("Error: Failed to read next sample.\n");
        goto error;
      }

      err = isobmff_getSampleData(sample, &sampleData, &sampleSize);
      if (err) {
        printf("Error: Failed to get sample data.\n");
        goto error;
      }

      sampleCounter++;

      printf("Samples processed: %" PRIu64 "\r", sampleCounter);

    } while (sampleData != NULL && sampleSize != 0);

    printf("\n");

    /* Destroy sample */
    err = isobmff_destroySample(sample);
    sample = NULL;
    if (err) {
      printf("Error: Failed to destroy sample.\n");
      goto error;
    }

    /* Destroy the track */
    err = isobmff_destroyTrack(track);
    track = NULL;
    if (err) {
      printf("Error: Failed to destroy track.\n");
      goto error;
    }
  }

  /* Destroy instance of isobmff */
  err = isobmff_destroy(isobmff);
  isobmff = NULL;
  if (err) {
    printf("Error: Destruction of isobmff instance failed.\n");
    goto error;
  }

  return 0;

/* Error handling */
error:

  if (err) {
    printErrorType(err);
  }

  if (memoryFile != NULL) {
    free(memoryFile);
    memoryFile = NULL;
  }

  if (sample != NULL) {
    err = isobmff_destroySample(sample);
    sample = NULL;
    if (err) {
      printf("Error: Failed to destroy sample.\n");
      printErrorType(err);
    }
  }

  if (track != NULL) {
    err = isobmff_destroyTrack(track);
    track = NULL;
    if (err) {
      printf("Error: Failed to destroy track reader instance.\n");
      printErrorType(err);
    }
  }

  if (isobmff != NULL) {
    err = isobmff_destroy(isobmff);
    isobmff = NULL;
    if (err) {
      printf("Error: Failed to destroy isobmff reader instance.\n");
      printErrorType(err);
    }
  }
  return 1;
}
