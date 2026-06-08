#!/bin/bash

fatal() {
    echo "$0: error occurred, exit"
    exit 1
}

if [[ $CI_PIPELINE_SOURCE == "schedule" ]]; then
  # Clean hardcoded path in .gcno files because tests can be run by a different worker in a different directory
  export GCOV_PREFIX_STRIP=$(echo ${CI_PROJECT_DIR} |awk -F / '{print NF-1}')
  export GCOV_PREFIX=${CI_PROJECT_DIR}
  COVERAGE="--coverage"
fi

source ci/env_makefile.sh
cd src

# Test esmumps and ptcheck first to avoid them deleting generated files needed by GCOV
make -j escheck TMPDIR=. COVERAGE=$COVERAGE || fatal
make -j ptcheck${JOBCHECK} TMPDIR=. COVERAGE=$COVERAGE || fatal
make -j check${JOBCHECK} TMPDIR=. COVERAGE=$COVERAGE || fatal

cd ..

if [[ $CI_PIPELINE_SOURCE == "schedule" ]]; then
  gcovr --xml-pretty --exclude-unreachable-branches --gcov-ignore-parse-errors --print-summary -o ${JOBNAME}.cov --root . || fatal
fi
