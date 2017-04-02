#!/usr/bin/env bash
cpplint \
    $@ \
    --filter=-build/include_subdir,-build/include_order \
    --linelength=80 \
    --counting=detailed \
$(find . -name "*.cpp" -or -name "*.h" | grep -vE "^./libs/")