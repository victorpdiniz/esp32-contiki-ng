#!/bin/bash
# ESP-IDF Setup Script for Contiki-NG ESP32-S3
# This script installs ESP-IDF in the standard location

set -e

ESP_IDF_VERSION="v5.5.1"
ESP_IDF_PATH="${HOME}/.espressif/esp-idf"

echo "================================================"
echo "ESP-IDF Setup for Contiki-NG ESP32-S3"
echo "================================================"
echo ""
echo "Target Version: $ESP_IDF_VERSION"
echo "Install Path: $ESP_IDF_PATH"
echo ""

# Check if ESP-IDF already installed
if [ -d "$ESP_IDF_PATH" ]; then
    echo "✓ ESP-IDF found at $ESP_IDF_PATH"
    cd "$ESP_IDF_PATH"
    
    # Check current version
    if git rev-parse --git-dir > /dev/null 2>&1; then
        CURRENT_VERSION=$(git describe --tags 2>/dev/null || echo "unknown")
        echo "  Current version: $CURRENT_VERSION"
        
        if [ "$CURRENT_VERSION" != "$ESP_IDF_VERSION" ]; then
            echo ""
            read -p "Update to $ESP_IDF_VERSION? (y/N): " -n 1 -r
            echo
            if [[ $REPLY =~ ^[Yy]$ ]]; then
                echo "Updating ESP-IDF..."
                git fetch --tags
                git checkout "$ESP_IDF_VERSION"
                git submodule update --init --recursive
            else
                echo "Keeping current version."
            fi
        else
            echo "  Already at target version."
        fi
    fi
else
    echo "Installing ESP-IDF $ESP_IDF_VERSION..."
    echo ""
    
    # Create directory
    mkdir -p "$(dirname $ESP_IDF_PATH)"
    cd "$(dirname $ESP_IDF_PATH)"
    
    # Clone ESP-IDF
    echo "Cloning ESP-IDF repository..."
    git clone -b "$ESP_IDF_VERSION" --recursive https://github.com/espressif/esp-idf.git
    cd esp-idf
fi

# Install ESP-IDF tools for ESP32-S3
echo ""
echo "Installing ESP-IDF tools for ESP32-S3..."
cd "$ESP_IDF_PATH"
./install.sh esp32s3

echo ""
echo "================================================"
echo "✅ ESP-IDF Setup Complete!"
echo "================================================"
echo ""
echo "Next steps:"
echo ""
echo "1. Add to your shell configuration (~/.bashrc or ~/.zshrc):"
echo ""
echo "   export IDF_PATH=$ESP_IDF_PATH"
echo "   alias get_idf='. \$IDF_PATH/export.sh'"
echo ""
echo "2. Reload your shell configuration:"
echo ""
echo "   source ~/.bashrc    # or ~/.zshrc"
echo ""
echo "3. Activate ESP-IDF environment (run this in every new terminal):"
echo ""
echo "   get_idf"
echo ""
echo "4. Build Contiki-NG ESP32-S3 examples:"
echo ""
echo "   cd examples/esp32-s3/hello-world"
echo "   make build"
echo ""
echo "================================================"
