# Main Page {#mainpage}

## Description

The mmtisobmff is a modern ISO File Format (ISO-IEC 14496-12) implementation.
It uses C++11 features and is designed towards extensibility and
maintainability.

The library support reading and writing of plain and fragmented mp4 files.
It provides both a C++ and a C interface. The C++
interface also enables specific interfaces to the MP4 files for specific use
cases.

## Reading Support

To read from an MP4 file you need to create an instance of type
[CIsobmffReader](@ref mmt::isobmff::CIsobmffReader), which you then can query
file level (duration, etc.) information from. To read samples from tracks, use
the template method
[CIsobmffReader::trackByIndex](@ref mmt::isobmff::CIsobmffReader::trackByIndex)
with the type of track reader you want to use (needs to be of type
[ITrackReader](@ref mmt::isobmff::ITrackReader)).

### Minimal Code Example

The following lines provide the minimal steps to open an MP4 file and read a
sample from it.

```{.c}
#include "mmtisobmff/reader/input.h"
#include "mmtisobmff/reader/reader.h"

using namespace mmt;
using namespace mmt::isobmff;

...

std::string fileUri = "/path/to/file.mp4";
CIsobmffReader reader(ilo::make_unique<CIsobmffFileInput>(fileUri));

auto trackReader = reader.trackByIndex<CGenericTrackReader>(0 /*first track*/);

CSample sample;
trackReader->nextSample(sample);
```

## Writing Support

To write an MP4 file you need to create an instance of type
[CIsobmffWriter](@ref mmt::isobmff::CIsobmffWriter).
Afterwards, you can create tracks and write samples by using the template method [CIsobmffWriter::trackWriter](@ref mmt::isobmff::CIsobmffWriter::trackWriter) with the type of track writer you want to use (needs to be of type
[ITrackWriter](@ref mmt::isobmff::ITrackWriter)).

### Minimal Code Example

The following lines provide the minimal steps to open an MP4 file and write a sample to it.

```{.c}
#include "mmtisobmff/writer/writer.h"

using namespace mmt;
using namespace mmt::isobmff;

...

std::string fileUri = "/path/to/file.mp4";

CIsobmffFileWriter::SOutputConfig outputConfig;
outputConfig.outputUri = fileUri;

SMovieConfig movieConfig;
// Configure the movieConfig as needed
movieConfig.majorBrand = ilo::toFcc("mp42");

CIsobmffFileWriter fileWriter{outputConfig, movieConfig};

SMpeghMhm1TrackConfig mhm1Config;
// Configure the mhm1Config as needed

std::unique_ptr<CMpeghTrackWriter> mhm1TrackWriter =
      fileWriter->trackWriter<CMpeghTrackWriter>(mhm1Config);


CSample sample;
// Fill sample with MHAS data

mhm1TrackWriter->addSample(sample);

...

fileWriter.close();
```

### Demo Applications

For reading MP4 files using the C++ interface, please have a look at the
demo applications

- simple_file_reader.cpp
- advanced_file_reader.cpp
- seeking_and_timestamps_api_demo.cpp

For writing MP4 files using the C++ interface, please have a look at the
demo applications

- simple_file_writer.cpp

For reading MP4 files using the C interface, please have a look at the
demo application

- simple_file_reader_c.c
- simple_memory_reader_c.c

Furthermore, the demo folder contains the following additional demo applications

- mp4_combine.cpp - A demo application to multiplex / demultiplex MP4 files.
- mp4_defragment.cpp - A demo application to defragment a fragmented MP4 file.
- mp4_segment.cpp - A demo application to enrich a fragmented file with a 'sidx' box.
- print_mp4_boxes.cpp - Low level tool to print the box tree of an MP4 file.
