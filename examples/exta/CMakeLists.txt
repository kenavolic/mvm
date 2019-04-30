set(TARGET_NAME exta)

add_executable(${TARGET_NAME} main.cpp)
set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "examples")
target_compile_features(${TARGET_NAME} PUBLIC cxx_std_17)
target_link_libraries(${TARGET_NAME} ${MVM_LIB})
install(TARGETS ${TARGET_NAME} RUNTIME DESTINATION bin)