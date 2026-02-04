#!/bin/bash
# Script to generate a manifest of all source files

set -e

MANIFEST_FILE="MANIFEST.txt"

echo "Generating manifest..."

{
    echo "# Prepayment Library Manifest"
    echo "# Generated on $(date)"
    echo ""
    echo "## Headers (include/)"
    find include -type f -name "*.h" | sort
    echo ""
    echo "## Source Files (src/)"
    find src -type f -name "*.cpp" | sort
    echo ""
    echo "## Examples (examples/)"
    find examples -type f -name "*.cpp" | sort
    echo ""
    echo "## Test Data (test_data/)"
    find test_data -type f | sort
    echo ""
    echo "## Build Files"
    echo "Makefile"
    echo ""
    echo "## Scripts (scripts/)"
    find scripts -type f -name "*.sh" | sort
} > "$MANIFEST_FILE"

echo "Manifest generated: $MANIFEST_FILE"
cat "$MANIFEST_FILE"
