#!/usr/bin/env bash
cpplint --linelength=120 --counting=detailed $( find . -name *.h -or -name *.ccp | grep -vE "^./libs/")