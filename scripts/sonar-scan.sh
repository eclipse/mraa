#!/bin/bash
#
# The script is used for determining options and running a static code
# analysis scan via SonarCloud.
#
# Author: Alex Tereschenko <alext.mkrs@gmail.com>
# Copyright (c) 2017 Alex Tereschenko.
# SPDX-License-Identifier: MIT
#
# All environment variables used are passed from either Travis or docker-compose.
# See details at https://docs.sonarqube.org/display/SONAR/Analysis+Parameters.
#
# Travis ones are:
#   Created by us:
#   - SONAR_ORG - SonarCloud "organization", under which the project is located.
#   - SONAR_PROJ_KEY - SonarCloud project key (name) to report to.
#   - SONAR_TOKEN - access token for that project (must be protected in Travis).
#   - GITHUB_TOKEN - GH OAuth token used by SonarCloud's GH plugin to report status in PRs.
#     See details at https://docs.sonarqube.org/display/PLUG/GitHub+Plugin. Must be protected.
#   Default:
#   - All TRAVIS_* variables. They are described in Travis docs
#     at https://docs.travis-ci.com/user/environment-variables
#
# docker-compose ones are:
#  - MRAA_SRC_DIR - path to mraa's git clone in the Docker container.

# Check required environment variables and exit if they are not set
function check_environment {
  VAR_NAME=$1
  VAR_VALUE=$2
  # Check required parameters
  VAR_NAME=${VAR_NAME:?value not provided}
  # Chek if variable is set
  if [ -z "${VAR_VALUE}" ]; then
    echo "Required environment variable ${VAR_NAME} is not defined. Skipping Execution..."
    exit 0;
  else
    echo "Required environment variable ${VAR_NAME} is set."
  fi
}

check_environment "MRAA_SRC_DIR" ${MRAA_SRC_DIR}
check_environment "SONAR_PROJ_KEY" ${SONAR_PROJ_KEY}
check_environment "SONAR_ORG" ${SONAR_ORG}
check_environment "SONAR_TOKEN" ${SONAR_TOKEN}

bw_output_path="${MRAA_SRC_DIR}/build/bw-output"

sonar_cmd_base="build-wrapper-linux-x86-64 --out-dir ${bw_output_path} make clean all && \
    sonar-scanner \
        --debug \
        -Dsonar.projectKey=${SONAR_PROJ_KEY} \
        -Dsonar.projectBaseDir=${MRAA_SRC_DIR} \
        -Dsonar.sources=${MRAA_SRC_DIR} \
        -Dsonar.inclusions='api/**/*,CMakeLists.txt,examples/**/*,imraa/**/*,include/**/*,src/**/*,tests/**/*' \
        -Dsonar.java.binaries='src' \
        -Dsonar.coverage.exclusions='**/*' \
        -Dsonar.cfamily.build-wrapper-output=${bw_output_path} \
        -Dsonar.host.url=https://sonarcloud.io \
        -Dsonar.organization=${SONAR_ORG} \
        -Dsonar.login=${SONAR_TOKEN} \
"

# Some useful data for logs
echo "TRAVIS_BRANCH: ${TRAVIS_BRANCH}"
echo "TRAVIS_PULL_REQUEST: ${TRAVIS_PULL_REQUEST}"
echo "TRAVIS_PULL_REQUEST_SLUG: ${TRAVIS_PULL_REQUEST_SLUG}"
echo "TRAVIS_REPO_SLUG: ${TRAVIS_REPO_SLUG}"

if [ "${TRAVIS_BRANCH}" == "master" -a "${TRAVIS_PULL_REQUEST}" == "false" ]; then
    # Master branch push - do a full-blown scan
    echo "Performing master branch push scan"
    sonar_cmd="${sonar_cmd_base}"
elif [ "${TRAVIS_PULL_REQUEST}" != "false" -a "${TRAVIS_PULL_REQUEST_SLUG}" == "${TRAVIS_REPO_SLUG}" ]; then
    # Internal PR - do a preview scan with report to the PR
    check_environment "GITHUB_TOKEN" ${GITHUB_TOKEN}

    echo "Performing internal pull request scan"
    sonar_cmd="${sonar_cmd_base} \
               -Dsonar.analysis.mode=preview \
               -Dsonar.github.pullRequest=${TRAVIS_PULL_REQUEST} \
               -Dsonar.github.repository=${TRAVIS_REPO_SLUG} \
               -Dsonar.github.oauth=${GITHUB_TOKEN} \
    "
else
    echo "Skipping the scan - external pull request or non-master branch push"
    exit 0
fi

echo "About to run the scan, the command is:"
echo "${sonar_cmd}"

eval "${sonar_cmd}"
