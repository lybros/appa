#!/usr/bin/env bash
cpplint --filter=-build/include_subdir --linelength=80 --counting=detailed $(find . -name "*.cpp" -or -name "*.h" | grep -vE "^./libs/")