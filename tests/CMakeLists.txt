set (TARGET_NAME ${MVM_LIB}_test)

set (MVM_TST
    base_stack_test.cpp
    base_vm_test.cpp
    instr_set_test.cpp
    list_test.cpp
    num_parse_test.cpp
    reflect_test.cpp
)

create_test_sourcelist( 
    ${TARGET_NAME}
    mvm_test_driver.cpp
    ${MVM_TST}
)

add_executable(${TARGET_NAME} mvm_test_driver.cpp ${MVM_TST} test_common.h)
set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "tests")
target_link_libraries(${TARGET_NAME} ${MVM_LIB} gtest gtest_main)
target_compile_features(${TARGET_NAME} PUBLIC cxx_std_17)

foreach(TST ${MVM_TST})
    get_filename_component(TNAME ${TST} NAME_WE)
    add_test(NAME ${TNAME} COMMAND ${TARGET_NAME} ${TNAME})
endforeach()