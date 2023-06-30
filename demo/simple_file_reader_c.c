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
 * Content: simple MP4 file reader demo using the C interface
 */

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#include "mmtisobmff/mmtisobmff_c.h"

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

void prettyPrintIsoLang(const char* fourCC, uint32_t size) {
  uint32_t i = 0;

  for (i = 0; i < size; i++) {
    printf("%c", fourCC[i]);
  }

  if (size != 3) {
    printf(" (Warning: IsoLang should have 3 characters)");
  }
  printf("\n");
}

ISOBMFF_ERR printDscInfo(TrackReader* track, TrackType_C trackType) {
  ISOBMFF_ERR err = ISOBMFF_OK;
  uint8_t* dcrData = NULL;
  uint32_t dcrDataSize = 0;
  uint32_t i = 0;

  printf("Dsc Info: \n");
  printf("########################################\n");

  if (trackType != TrackType_Audio) {
    printf("Currently not supported for non audio tracks.\n\n");
    return err;
  }

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

ISOBMFF_ERR printLudtInfo(TrackReader* track) {
  ISOBMFF_ERR err = ISOBMFF_OK;
  uint8_t* ludtInitData = NULL;
  uint32_t ludtInitDataSize = 0;
  uint32_t i = 0;

  printf("Loudness Info: \n");
  printf("########################################\n");

  err = isobmff_getInitLudtData(track, &ludtInitData, &ludtInitDataSize);
  if (err) {
    printf("Error: Failed to get ludt init data.\n");
    return err;
  }

  printf("Ludt init data available?: ");
  if (ludtInitDataSize > 0) {
    printf("Yes\n");
    printf("Ludt init data: ");
    for (i = 0; i < ludtInitDataSize; i++) {
      printf("%02x ", ludtInitData[i]);
    }
    printf("\n");
  } else {
    printf("No\n");
  }
  printf("\n");

  return err;
}

ISOBMFF_ERR printFragmentLudtInfo(TrackReader* track, uint32_t sampleFragmentNum) {
  ISOBMFF_ERR err = ISOBMFF_OK;
  uint8_t* ludtFragmentData = NULL;
  uint32_t ludtFragmentDataSize = 0;
  uint32_t i = 0;

  err = isobmff_getFragmentLudtData(track, sampleFragmentNum, &ludtFragmentData,
                                    &ludtFragmentDataSize);
  if (err) {
    printf("Error: Failed to get ludt fragment data of fragment numer %u data.\n",
           sampleFragmentNum);
    return err;
  }

  if (ludtFragmentDataSize > 0) {
    printf("Ludt fragment data of fragment nr %d: ", sampleFragmentNum);
    for (i = 0; i < ludtFragmentDataSize; i++) {
      printf("%02x ", ludtFragmentData[i]);
    }
    printf("\n");
  }

  return err;
}

int main(int argc, char* argv[]) {
  ISOBMFF_ERR err = ISOBMFF_OK;
  ISOBMFF_Reader* isobmff = NULL;
  uint8_t audioProfileLevelIndication = 0;
  uint8_t audioProfileLevelIndicationIsValid = 0;

  TrackReader* track = NULL;
  uint32_t trackCount = 0;
  uint32_t trackId = 0;
  char* handler = NULL;
  uint32_t handlerSize = 0;
  char* codingName = NULL;
  uint32_t codingNameSize = 0;
  TrackType_C trackType = TrackType_Undefined;
  Codec_C codec = Codec_Undefined;
  uint64_t trackDuration = 0;
  uint32_t trackTimeScale = 0;
  char* trackLanguage = NULL;
  uint32_t trackLanguageSize = 0;
  uint32_t audioSamplerate = 0;
  uint32_t audioChannelCount = 0;
  uint32_t mp4aAudioMaxBitrate = 0;
  uint32_t mp4aAudioAvgBitrate = 0;
  uint32_t mp4aAudioBufferSizeDb = 0;

  uint32_t i = 0;

  uint8_t hasLudtUpdates = 0;

  uint64_t maxSampleSize;
  Sample* sample = NULL;
  uint8_t* sampleData = NULL;
  uint64_t sampleSize = 0;
  uint32_t sampleFragmentNum = 0;
  uint32_t sampleFragmentNumLast = 0;
  uint64_t sampleCounter = 0;

  if (argc != 2) {
    printf("Usage: simpleFileReader_c <File-URI>\n\n");
    goto error;
  }

  err = isobmff_disableLogging();
  if (err) {
    printf("Error: Disabling logging failed.\n");
    goto error;
  }

  printf("Reading from input file %s\n\n", argv[1]);

  /* Create an ISOBMFF_Reader instance, open the mp4 file and initialize the reader */
  err = isobmff_createFileReader(&isobmff, argv[1]);
  if (err) {
    printf("Error: Creation of isobmff instance failed.\n");
    goto error;
  }

  err = isobmff_getIodsAudioProfileLevelIndication(isobmff, &audioProfileLevelIndication,
                                                   &audioProfileLevelIndicationIsValid);
  if (err) {
    printf("Error: Failed to retriev audioProfileLevelIndication.\n");
    goto error;
  }

  if (audioProfileLevelIndicationIsValid) {
    printf("Found audioProfileLevelIndication of %u.\n\n", audioProfileLevelIndication);
  }

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
      printf("Error: Failed to get track type.\n");
      goto error;
    }

    err = isobmff_getTrackCodec(track, &codec);
    if (err) {
      printf("Error: Failed to get track codec.\n");
      goto error;
    }

    err = isobmff_getTrackDuration(track, &trackDuration);
    if (err) {
      printf("Error: Failed to get track duration.\n");
      goto error;
    }

    err = isobmff_getTrackTimeScale(track, &trackTimeScale);
    if (err) {
      printf("Error: Failed to get track timescale.\n");
      goto error;
    }

    err = isobmff_getTrackLanguage(track, &trackLanguage, &trackLanguageSize);
    if (err) {
      printf("Error: Failed to get track language.\n");
      goto error;
    }

    if (trackType == TrackType_Audio) {
      err = isobmff_getAudioSampleRate(track, &audioSamplerate);
      if (err) {
        printf("Error: Failed to get audio samplerate.\n");
        goto error;
      }

      err = isobmff_getAudioChannelCount(track, &audioChannelCount);
      if (err) {
        printf("Error: Failed to get audio channel count.\n");
        goto error;
      }

      if (codec == Codec_Mp4a) {
        err = isobmff_getDcrMp4aAudioMaxBitrate(track, &mp4aAudioMaxBitrate);
        if (err) {
          printf("Error: Failed to get mp4a max bitrate.\n");
          goto error;
        }

        err = isobmff_getDcrMp4aAudioAvgBitrate(track, &mp4aAudioAvgBitrate);
        if (err) {
          printf("Error: Failed to get mp4a avg bitrate.\n");
          goto error;
        }

        err = isobmff_getDcrMp4aAudioBufferSizeDb(track, &mp4aAudioBufferSizeDb);
        if (err) {
          printf("Error: Failed to get mp4a buffer size db.\n");
          goto error;
        }
      }
    }

    printf("########################################\n");
    printf("-TrackInfo:\n");
    printf("-- ID               : %u\n", trackId);
    printf("-- Handler          : ");
    prettyPrintFourCC(handler, handlerSize);
    printf("-- Type             : ");
    prettyPrintTrackType(trackType);
    printf("-- Codec            : ");
    prettyPrintFourCC(codingName, codingNameSize);
    printf("-- Duration         : %" PRIu64 "\n", trackDuration);
    printf("-- Timescale        : %u\n", trackTimeScale);
    printf("-- Language         : ");
    prettyPrintIsoLang(trackLanguage, trackLanguageSize);

    if (trackType == TrackType_Audio) {
      printf("-- Audio Samplerate : %u\n", audioSamplerate);
      if (codec == Codec_Mp4a) {
        printf("-- MP4a max bitrate : %u\n", mp4aAudioMaxBitrate);
        printf("-- MP4a avg bitrate : %u\n", mp4aAudioAvgBitrate);
        printf("-- MP4a bufferSizeDb: %u\n", mp4aAudioBufferSizeDb);
      }
    }

    printf("\nCreating reader for track with ID %u ... ", trackId);

    printf("Done.\n\n");

    /* Get decoder config record data */
    err = printDscInfo(track, trackType);
    if (err) {
      printf("Error: Failed to print decoder config record data.\n");
      goto error;
    }

    /* Get init ludt data (if any) */
    err = printLudtInfo(track);
    if (err) {
      printf("Error: Failed to print ludt data.\n");
      goto error;
    }

    /* Check if track has ludt data updates */
    err = isobmff_hasLudtUpdates(track, &hasLudtUpdates);
    if (err) {
      printf("Error: Failed to check for ludt updates.\n");
      goto error;
    }

    printf("Track has ludt data updates?: ");
    if (hasLudtUpdates > 0) {
      printf("Yes\n");
    } else {
      printf("No\n");
    }
    printf("\n");

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
      printf("Error: Failed to create sample.\n");
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

      if (hasLudtUpdates) {
        err = isobmff_getSampleFragmentNum(sample, &sampleFragmentNum);
        if (err) {
          printf("Error: Failed to get sample fragment number.\n");
          goto error;
        }

        if (sampleFragmentNum != sampleFragmentNumLast) {
          err = printFragmentLudtInfo(track, sampleFragmentNum);
          if (err) {
            printf("Error: Failed to print fragmented ludt info.\n");
            goto error;
          }
        }
        sampleFragmentNumLast = sampleFragmentNum;
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
