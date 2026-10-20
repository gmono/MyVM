file(WRITE "${OUTPUT}" "sentinel")
set(INVALID_INPUT "${OUTPUT}.invalid.asm")
file(WRITE "${INVALID_INPUT}" ".code\nldl #0x100000000\n")
execute_process(
    COMMAND "${ASSEMBLER}" "${INVALID_INPUT}" -o "${OUTPUT}"
    RESULT_VARIABLE invalid_result
    OUTPUT_QUIET
    ERROR_QUIET
)
if(invalid_result EQUAL 0)
    message(FATAL_ERROR "myvm-asm accepted an out-of-range operand")
endif()
file(READ "${OUTPUT}" preserved)
if(NOT preserved STREQUAL "sentinel")
    message(FATAL_ERROR "failed assembly modified the previous output")
endif()

execute_process(
    COMMAND "${ASSEMBLER}" "${INPUT}" -o "${OUTPUT}"
    RESULT_VARIABLE result
    OUTPUT_VARIABLE stdout
    ERROR_VARIABLE stderr
)
if(NOT result EQUAL 0)
    message(FATAL_ERROR "myvm-asm failed (${result}): ${stdout}${stderr}")
endif()
file(READ "${OUTPUT}" actual HEX)
if(NOT actual STREQUAL "04000000341200000c00000000000000")
    message(FATAL_ERROR "myvm-asm produced unexpected bytes: ${actual}")
endif()
