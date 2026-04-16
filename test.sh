#!/bin/bash
# test.sh
# Summary: Validation suite for prol
# Author:  KaisarCode
# Website: https://kaisarcode.com
# License: https://www.gnu.org/licenses/gpl-3.0.html

# Check if the prol binary exists
# @return 0 if exists, 1 otherwise
kc_test_check_binary() {
    if [ ! -f "./prol" ]; then
        echo "[ERROR] prol binary not found. Please compile first."
        return 1
    fi
    return 0
}

# Run a single detection test
# @param 1 text to detect
# @param 2 expected language code
# @return 0 on success, 1 on failure
kc_test_run_case() {
    local text="$1"
    local expected="$2"
    local result
    
    result=$(echo "$text" | ./prol)
    
    if [ "$result" = "$expected" ]; then
        echo "[PASS] '$text' -> $result"
        return 0
    else
        echo "[FAIL] '$text' -> Expected $expected, got '$result'"
        return 1
    fi
}

# Run the complete test suite
# @return 0 if all tests pass, 1 otherwise
kc_test_main() {
    local failed=0
    
    kc_test_check_binary || exit 1
    
    echo "Starting prol validation suite..."
    echo "--------------------------------"
    
    kc_test_run_case "Hello world" "en" || failed=$((failed + 1))
    kc_test_run_case "¿Cómo estás?" "es" || failed=$((failed + 1))
    kc_test_run_case "Bom dia" "pt" || failed=$((failed + 1))
    kc_test_run_case "Bonjour" "fr" || failed=$((failed + 1))
    kc_test_run_case "Ciao" "it" || failed=$((failed + 1))
    kc_test_run_case "Guten Morgen" "de" || failed=$((failed + 1))
    kc_test_run_case "привет" "ru" || failed=$((failed + 1))
    kc_test_run_case "γεια σας" "el" || failed=$((failed + 1))
    kc_test_run_case "안녕하세요" "ko" || failed=$((failed + 1))
    kc_test_run_case "こんにちは" "ja" || failed=$((failed + 1))
    kc_test_run_case "क्या हाल है" "hi" || failed=$((failed + 1))
    kc_test_run_case "مرحبا بك" "ar" || failed=$((failed + 1))
    
    echo "--------------------------------"
    if [ "$failed" -eq 0 ]; then
        echo "[SUCCESS] All tests passed!"
        return 0
    else
        echo "[FAILURE] $failed tests failed."
        return 1
    fi
}

kc_test_main
