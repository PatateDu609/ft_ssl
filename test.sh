#!/bin/bash

make

echo "Launching hash tests"
bash tests/test_hash.sh

echo "Launching base64 tests"
bash tests/test_base64.sh
