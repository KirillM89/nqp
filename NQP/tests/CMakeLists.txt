target_sources(nnls_tests PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/qld.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/qp_utils.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/qp.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/test_utils.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp 

    ${CMAKE_CURRENT_SOURCE_DIR}/qp_utils.h
    ${CMAKE_CURRENT_SOURCE_DIR}/qp.h
    ${CMAKE_CURRENT_SOURCE_DIR}/test_utils.h
    ${CMAKE_CURRENT_SOURCE_DIR}/test_data.h
)
add_subdirectory(gtest)
