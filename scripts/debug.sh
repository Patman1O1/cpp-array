#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)/.."

conan build "${ROOT}" -o "&:build_tests=True" -s build_type=Debug
gdb "${ROOT}/build/Debug/bin/circular_array_run_time_test"

