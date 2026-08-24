#!/usr/bin/env bash

set -euo pipefail

ROOT="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)/.."

conan build "${ROOT}" -o "&:build_tests=True"
"${ROOT}/build/Release/bin/array_test"
