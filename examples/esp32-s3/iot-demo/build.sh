#!/bin/bash

# ESP32-S3 Contiki-NG Build Helper Script
# Usage: ./build.sh [command]

set -e

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
IDF_PATH="${IDF_PATH:-$HOME/esp/esp-idf}"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Print colored message
print_info() {
    echo -e "${BLUE}ℹ${NC} $1"
}

print_success() {
    echo -e "${GREEN}✓${NC} $1"
}

print_error() {
    echo -e "${RED}✗${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}⚠${NC} $1"
}

# Check if ESP-IDF is available
check_idf() {
    if [ ! -f "$IDF_PATH/export.sh" ]; then
        print_error "ESP-IDF not found at $IDF_PATH"
        print_info "Please install ESP-IDF or set IDF_PATH environment variable"
        exit 1
    fi
    
    # Source ESP-IDF environment
    print_info "Sourcing ESP-IDF environment..."
    source "$IDF_PATH/export.sh" > /dev/null 2>&1
    print_success "ESP-IDF environment loaded"
}

# Build the project
build() {
    print_info "Building project..."
    cd "$PROJECT_DIR"
    idf.py build
    print_success "Build complete!"
}

# Clean build
clean() {
    print_info "Cleaning build files..."
    cd "$PROJECT_DIR"
    idf.py fullclean
    print_success "Clean complete!"
}

# Flash to device
flash() {
    local PORT="${1:-/dev/ttyUSB0}"
    
    print_info "Flashing to device on port $PORT..."
    cd "$PROJECT_DIR"
    
    if [ ! -e "$PORT" ]; then
        print_error "Port $PORT not found"
        print_info "Available ports:"
        ls /dev/ttyUSB* /dev/ttyACM* 2>/dev/null || echo "  No USB serial devices found"
        exit 1
    fi
    
    idf.py -p "$PORT" flash
    print_success "Flash complete!"
}

# Monitor serial output
monitor() {
    local PORT="${1:-/dev/ttyUSB0}"
    
    print_info "Monitoring device on port $PORT..."
    print_info "Press Ctrl+] to exit"
    cd "$PROJECT_DIR"
    idf.py -p "$PORT" monitor
}

# Build and flash
build_flash() {
    local PORT="${1:-/dev/ttyUSB0}"
    build
    flash "$PORT"
}

# Build, flash, and monitor
all() {
    local PORT="${1:-/dev/ttyUSB0}"
    build
    flash "$PORT"
    monitor "$PORT"
}

# Show device info
device_info() {
    local PORT="${1:-/dev/ttyUSB0}"
    
    print_info "Device information for $PORT:"
    cd "$PROJECT_DIR"
    idf.py -p "$PORT" flash monitor | head -n 50
}

# List available ports
list_ports() {
    print_info "Available serial ports:"
    ls -l /dev/ttyUSB* /dev/ttyACM* 2>/dev/null | awk '{print "  " $NF}' || echo "  No USB serial devices found"
}

# Configure WiFi credentials
configure_wifi() {
    local MAIN_FILE="$PROJECT_DIR/main/iot-app.c"
    
    print_info "Configure WiFi credentials"
    echo ""
    read -p "Enter WiFi SSID: " SSID
    read -sp "Enter WiFi Password: " PASSWORD
    echo ""
    
    # Backup original file
    cp "$MAIN_FILE" "$MAIN_FILE.bak"
    
    # Update WiFi credentials
    sed -i "s/#define WIFI_SSID.*/#define WIFI_SSID     \"$SSID\"/g" "$MAIN_FILE"
    sed -i "s/#define WIFI_PASSWORD.*/#define WIFI_PASSWORD \"$PASSWORD\"/g" "$MAIN_FILE"
    
    print_success "WiFi credentials updated in $MAIN_FILE"
    print_warning "Original file backed up to $MAIN_FILE.bak"
}

# Show menu
menu() {
    echo ""
    echo "╔════════════════════════════════════════════════════════════╗"
    echo "║       ESP32-S3 Contiki-NG Build Helper                    ║"
    echo "╚════════════════════════════════════════════════════════════╝"
    echo ""
    echo "Usage: $0 [command] [port]"
    echo ""
    echo "Commands:"
    echo "  build              Build the project"
    echo "  clean              Clean build files"
    echo "  flash [port]       Flash to device (default: /dev/ttyUSB0)"
    echo "  monitor [port]     Monitor serial output"
    echo "  build-flash [port] Build and flash"
    echo "  all [port]         Build, flash, and monitor"
    echo "  ports              List available serial ports"
    echo "  wifi               Configure WiFi credentials"
    echo "  info [port]        Show device information"
    echo "  help               Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0 all                    # Build, flash, monitor (default port)"
    echo "  $0 flash /dev/ttyACM0     # Flash to specific port"
    echo "  $0 wifi                   # Configure WiFi credentials"
    echo ""
}

# Main script
main() {
    local COMMAND="${1:-help}"
    local PORT="${2:-/dev/ttyUSB0}"
    
    case "$COMMAND" in
        build)
            check_idf
            build
            ;;
        clean)
            check_idf
            clean
            ;;
        flash)
            check_idf
            flash "$PORT"
            ;;
        monitor)
            check_idf
            monitor "$PORT"
            ;;
        build-flash)
            check_idf
            build_flash "$PORT"
            ;;
        all)
            check_idf
            all "$PORT"
            ;;
        ports)
            list_ports
            ;;
        wifi)
            configure_wifi
            ;;
        info)
            check_idf
            device_info "$PORT"
            ;;
        help|--help|-h)
            menu
            ;;
        *)
            print_error "Unknown command: $COMMAND"
            menu
            exit 1
            ;;
    esac
}

# Run main function
main "$@"
