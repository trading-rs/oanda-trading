#!/bin/bash
set +e
set -x

curl -sSL https://get.haskellstack.org/ | sh
pip install conan
conan install Poco/1.7.8p3@pocoproject/stable
