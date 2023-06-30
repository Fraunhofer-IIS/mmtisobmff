set(emscriptenLinkOptions
    "-fexceptions" 
    "-sFILESYSTEM=1" 
    "-sABORTING_MALLOC=0" 
    "-sALLOW_MEMORY_GROWTH=1" 
    "-sNODERAWFS=1" 
    "-sNO_DISABLE_EXCEPTION_CATCHING"
)

set(emscriptenCompileOptions
    "-fexceptions"
)

# Target : printMP4 demo binary
add_executable(printMP4 print_mp4_boxes.cpp)
target_link_libraries(printMP4 mmtisobmff)
target_compile_features(printMP4 PUBLIC cxx_std_11)
set_target_properties(printMP4 PROPERTIES CXX_EXTENSIONS OFF)

if (EMSCRIPTEN)
  target_link_options(printMP4 PUBLIC ${emscriptenLinkOptions})
  target_compile_options(printMP4 PUBLIC ${emscriptenCompileOptions})
endif()

# Target : simpleFileReader demo binary
add_executable(simpleFileReader simple_file_reader.cpp)
target_link_libraries(simpleFileReader mmtisobmff)
target_compile_features(simpleFileReader PUBLIC cxx_std_11)
set_target_properties(simpleFileReader PROPERTIES CXX_EXTENSIONS OFF)

if (EMSCRIPTEN)
  target_link_options(simpleFileReader PUBLIC ${emscriptenLinkOptions})
  target_compile_options(simpleFileReader PUBLIC ${emscriptenCompileOptions})
endif()

# Target : advancedFileReader demo binary
add_executable(advancedFileReader advanced_file_reader.cpp)
target_link_libraries(advancedFileReader mmtisobmff)
target_compile_features(advancedFileReader PUBLIC cxx_std_11)
set_target_properties(advancedFileReader PROPERTIES CXX_EXTENSIONS OFF)

if (EMSCRIPTEN)
  target_link_options(advancedFileReader PUBLIC ${emscriptenLinkOptions})
  target_compile_options(advancedFileReader PUBLIC ${emscriptenCompileOptions})
endif()

# Target : simpleFileReader_c demo binary
add_executable(simpleFileReader_c simple_file_reader_c.c)
target_link_libraries(simpleFileReader_c mmtisobmff_c)
target_compile_features(simpleFileReader_c PUBLIC cxx_std_11)
set_target_properties(simpleFileReader_c PROPERTIES CXX_EXTENSIONS OFF)

if (EMSCRIPTEN)
  target_link_options(simpleFileReader_c PUBLIC ${emscriptenLinkOptions})
  target_compile_options(simpleFileReader_c PUBLIC ${emscriptenCompileOptions})
endif()

# Target : simpleMemoryReader_c demo binary
add_executable(simpleMemoryReader_c simple_memory_reader_c.c)
target_link_libraries(simpleMemoryReader_c mmtisobmff_c)
target_compile_features(simpleMemoryReader_c PUBLIC cxx_std_11)
set_target_properties(simpleMemoryReader_c PROPERTIES CXX_EXTENSIONS OFF)

if (EMSCRIPTEN)
  target_link_options(simpleMemoryReader_c PUBLIC ${emscriptenLinkOptions})
  target_compile_options(simpleMemoryReader_c PUBLIC ${emscriptenCompileOptions})
endif()

# Target : seekingAndTimestampsApiDemo demo binary
add_executable(seekingAndTimestampsApiDemo seeking_and_timestamps_api_demo.cpp)
target_link_libraries(seekingAndTimestampsApiDemo mmtisobmff)
target_compile_features(seekingAndTimestampsApiDemo PUBLIC cxx_std_11)
set_target_properties(seekingAndTimestampsApiDemo PROPERTIES CXX_EXTENSIONS OFF)

if (EMSCRIPTEN)
  target_link_options(seekingAndTimestampsApiDemo PUBLIC ${emscriptenLinkOptions})
  target_compile_options(seekingAndTimestampsApiDemo PUBLIC ${emscriptenCompileOptions})
endif()

# Target : mp4Defragment demo binary
add_executable(mp4Defragment mp4_defragment.cpp)
target_link_libraries(mp4Defragment mmtisobmff)
target_compile_features(mp4Defragment PUBLIC cxx_std_11)
set_target_properties(mp4Defragment PROPERTIES CXX_EXTENSIONS OFF)

if (EMSCRIPTEN)
  target_link_options(mp4Defragment PUBLIC ${emscriptenLinkOptions})
  target_compile_options(mp4Defragment PUBLIC ${emscriptenCompileOptions})
endif()

# Target : mp4SamplesDiff demo binary
add_executable( mp4SamplesDiff mp4_samples_diff.cpp )
target_link_libraries( mp4SamplesDiff mmtisobmff )
target_compile_features(mp4SamplesDiff PUBLIC cxx_std_11)
set_target_properties(mp4SamplesDiff PROPERTIES CXX_EXTENSIONS OFF)

if (EMSCRIPTEN)
  target_link_options(mp4SamplesDiff PUBLIC ${emscriptenLinkOptions})
  target_compile_options(mp4SamplesDiff PUBLIC ${emscriptenCompileOptions})
endif()

# Target : mp4Combine demo binary
add_executable(mp4Combine mp4_combine.cpp)
target_link_libraries(mp4Combine mmtisobmff)
target_compile_features(mp4Combine PUBLIC cxx_std_11)
set_target_properties(mp4Combine PROPERTIES CXX_EXTENSIONS OFF)

if (EMSCRIPTEN)
  target_link_options(mp4Combine PUBLIC ${emscriptenLinkOptions})
  target_compile_options(mp4Combine PUBLIC ${emscriptenCompileOptions})
endif()

# Target : mp4Fragment demo binary
add_executable(mp4Fragment mp4_fragment.cpp)
target_link_libraries(mp4Fragment mmtisobmff)
target_compile_features(mp4Fragment PUBLIC cxx_std_11)
set_target_properties(mp4Fragment PROPERTIES CXX_EXTENSIONS OFF)

if (EMSCRIPTEN)
  target_link_options(mp4Fragment PUBLIC ${emscriptenLinkOptions})
  target_compile_options(mp4Fragment PUBLIC ${emscriptenCompileOptions})
endif()

# Target : mp4Segment demo binary
add_executable(mp4Segment mp4_segment.cpp)
target_link_libraries(mp4Segment mmtisobmff)
target_compile_features(mp4Segment PUBLIC cxx_std_11)
set_target_properties(mp4Segment PROPERTIES CXX_EXTENSIONS OFF)

if (EMSCRIPTEN)
  target_link_options(mp4Segment PUBLIC ${emscriptenLinkOptions})
  target_compile_options(mp4Segment PUBLIC ${emscriptenCompileOptions})
endif()

# Target : simpleFileWriter demo binary
add_executable(simpleFileWriter simple_file_writer.cpp)
target_link_libraries(simpleFileWriter mmtisobmff)
target_compile_features(simpleFileWriter PUBLIC cxx_std_11)
set_target_properties(simpleFileWriter PROPERTIES CXX_EXTENSIONS OFF)

if (EMSCRIPTEN)
  target_link_options(simpleFileWriter PUBLIC ${emscriptenLinkOptions})
  target_compile_options(simpleFileWriter PUBLIC ${emscriptenCompileOptions})
endif()

if(mmtisobmff_BUILD_TEST)
  
  list(APPEND testfilesAudio ${CMAKE_CURRENT_SOURCE_DIR}/../test/data/german_forest_3s_snippet_mp4a_regular.mp4)
  list(APPEND testfilesAudio ${CMAKE_CURRENT_SOURCE_DIR}/../test/data/german_forest_2s_snippet_mha_regular.mp4)
  list(APPEND testfilesAudio ${CMAKE_CURRENT_SOURCE_DIR}/../test/data/german_forest_3s_snippet_mhm_regular.mp4)
  list(APPEND testfilesAudio ${CMAKE_CURRENT_SOURCE_DIR}/../test/data/german_forest_3s_snippet_mp4a.mp4)
  list(APPEND testfilesAudio ${CMAKE_CURRENT_SOURCE_DIR}/../test/data/german_forest_2s_snipet_mha.mp4)
  
  foreach(testfile ${testfilesAudio})
    get_filename_component(testfile_name ${testfile} NAME)
    add_test(demo_printMP4.${testfile_name} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/printMP4 ${testfile})
    add_test(demo_simpleFileReader.${testfile_name} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/simpleFileReader ${testfile})
    add_test(demo_advancedFileReader.${testfile_name} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/advancedFileReader ${testfile})
    add_test(demo_simpleFileReader_c.${testfile_name} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/simpleFileReader_c ${testfile})
  endforeach()

  get_filename_component(testfile_name ${CMAKE_CURRENT_SOURCE_DIR}/../test/data/forest_short_av.mp4 NAME)
  add_test(simpleFileWriter.${testfile_name}        
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/simpleFileWriter
    ${CMAKE_CURRENT_SOURCE_DIR}/../test/data/forest_short_av.mp4     
    ${CMAKE_CURRENT_BINARY_DIR}/../Testing/simpleFileWriter.${testfile_name}_out.mp4)

  list(APPEND testfilesVideo ${CMAKE_CURRENT_SOURCE_DIR}/../test/data/german_forest_2s_snippet_avc1_regular.mp4)
  list(APPEND testfilesVideo ${CMAKE_CURRENT_SOURCE_DIR}/../test/data/german_forest_2s_snippet_hvc1_regular.mp4)
  list(APPEND testfilesVideo ${CMAKE_CURRENT_SOURCE_DIR}/../test/data/german_forest_2s_snipet_avc1.mp4)
  list(APPEND testfilesVideo ${CMAKE_CURRENT_SOURCE_DIR}/../test/data/german_forest_2s_snipet_hvc1.mp4)

  foreach(testfile ${testfilesVideo})
    get_filename_component(testfile_name ${testfile} NAME)
    add_test(demo_printMP4.${testfile_name} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/printMP4 ${testfile})
    add_test(demo_simpleFileReader.${testfile_name} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/simpleFileReader ${testfile})
    add_test(demo_advancedFileReader.${testfile_name} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/advancedFileReader ${testfile})
  endforeach()
endif()
