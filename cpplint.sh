#!/usr/bin/env bash
cpplint --linelength=80 --counting=detailed $(find . -name "*.cpp" -or -name "*.h" | grep -vE "^./libs/")