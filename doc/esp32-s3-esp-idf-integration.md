# ESP-IDF Integration Guide

## Question: Should ESP-IDF Be in the Same Repository?

**Short Answer: NO - Keep ESP-IDF separate as an external dependency.**

## Recommended Approach: External ESP-IDF

### ✅ Advantages

1. **Standard Practice**: Follows ESP-IDF and embedded development norms
2. **Size**: ESP-IDF is ~500MB+ with toolchains
3. **Updates**: Easy to update ESP-IDF independently
4. **Multiple Projects**: One ESP-IDF installation serves multiple Contiki-NG projects
5. **CI/CD**: Cache ESP-IDF separately, faster builds
6. **Licensing**: Clear separation of licenses

### 📁 Recommended Structure

```
~/contiki-ng/                    # Your Contiki-NG fork
├── arch/
│   ├── platform/esp32-s3/
│   └── cpu/esp32-s3/
├── examples/esp32-s3/
└── ...

~/.espressif/                    # ESP-IDF installation (standard location)
└── esp-idf/                     # ESP-IDF v5.x
    ├── components/
    ├── tools/
    └── ...
```

### 🔧 Setup Script (Recommended)

Create `tools/esp32-s3/setup-esp-idf.sh`:

```bash
#!/bin/bash
# ESP-IDF Setup Script for Contiki-NG ESP32-S3

set -e

ESP_IDF_VERSION="v5.1.2"  # Specify version
ESP_IDF_PATH="${HOME}/.espressif/esp-idf"

echo "Setting up ESP-IDF for Contiki-NG ESP32-S3..."

# Check if ESP-IDF already installed
if [ -d "$ESP_IDF_PATH" ]; then
    echo "ESP-IDF found at $ESP_IDF_PATH"
    cd "$ESP_IDF_PATH"
    CURRENT_VERSION=$(git describe --tags)
    echo "Current version: $CURRENT_VERSION"
    
    if [ "$CURRENT_VERSION" != "$ESP_IDF_VERSION" ]; then
        echo "Updating to $ESP_IDF_VERSION..."
        git fetch
        git checkout "$ESP_IDF_VERSION"
        git submodule update --init --recursive
    fi
else
    echo "Installing ESP-IDF $ESP_IDF_VERSION..."
    mkdir -p "$(dirname $ESP_IDF_PATH)"
    cd "$(dirname $ESP_IDF_PATH)"
    git clone -b "$ESP_IDF_VERSION" --recursive https://github.com/espressif/esp-idf.git
    cd esp-idf
fi

# Install tools
echo "Installing ESP-IDF tools..."
./install.sh esp32s3

echo ""
echo "✅ ESP-IDF setup complete!"
echo ""
echo "Add to your ~/.bashrc or ~/.zshrc:"
echo "  export IDF_PATH=$ESP_IDF_PATH"
echo "  alias get_idf='. \$IDF_PATH/export.sh'"
echo ""
echo "Then run: source ~/.bashrc && get_idf"
```

Make it executable:
```bash
chmod +x tools/esp32-s3/setup-esp-idf.sh
./tools/esp32-s3/setup-esp-idf.sh
```

### 🐳 Docker Approach (Best for CI/CD)

Create `tools/esp32-s3/Dockerfile`:

```dockerfile
FROM espressif/idf:v5.1.2

# Install additional tools if needed
RUN apt-get update && apt-get install -y \
    git \
    make \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /workspace

# Copy Contiki-NG project
COPY . /workspace/

# Set IDF_PATH
ENV IDF_PATH=/opt/esp/idf

CMD ["/bin/bash"]
```

Build and use:
```bash
docker build -t contiki-esp32s3 -f tools/esp32-s3/Dockerfile .
docker run -it --rm -v $(pwd):/workspace contiki-esp32s3
```

## ❌ Why NOT Include ESP-IDF in Repository

### Problems with Git Submodule

```
# DON'T DO THIS
git submodule add https://github.com/espressif/esp-idf.git tools/esp-idf
```

**Issues:**
- Massive repository size (500MB+)
- Slow clones: `git clone --recursive` takes forever
- Submodule hell: Nested submodules (ESP-IDF has ~20 submodules)
- Version conflicts between projects
- Unnecessary for users who already have ESP-IDF

### Problems with Vendoring (Copying ESP-IDF)

**Issues:**
- Repository bloat
- Update nightmare
- Licensing complications
- Violates ESP-IDF's expected installation pattern

## 🎯 Implementation: Project Requirements File

Create `tools/esp32-s3/requirements.txt`:

```ini
# ESP32-S3 Development Requirements

[esp-idf]
version = v5.1.2
url = https://github.com/espressif/esp-idf.git
install_path = ~/.espressif/esp-idf
targets = esp32s3

[python-packages]
# Additional Python packages if needed
# Example: pyserial, pytest, etc.
```

## 📋 Documentation in README

Update root `README.md`:

```markdown
## ESP32-S3 Development Setup

### Prerequisites

1. **Install ESP-IDF v5.1.2**
   ```bash
   ./tools/esp32-s3/setup-esp-idf.sh
   ```

2. **Configure environment** (add to `~/.bashrc`):
   ```bash
   export IDF_PATH=~/.espressif/esp-idf
   alias get_idf='. $IDF_PATH/export.sh'
   ```

3. **Activate ESP-IDF environment**:
   ```bash
   get_idf
   ```

### Build ESP32-S3 Examples

```bash
cd examples/esp32-s3/hello-world
make build
make flash monitor
```

See [ESP32-S3 Platform Guide](arch/platform/esp32-s3/README.md) for details.
```

## 🔄 CI/CD Integration (GitHub Actions)

Create `.github/workflows/esp32-s3-build.yml`:

```yaml
name: ESP32-S3 Build

on: [push, pull_request]

jobs:
  build-esp32s3:
    runs-on: ubuntu-latest
    
    steps:
      - name: Checkout repository
        uses: actions/checkout@v3
      
      - name: Setup ESP-IDF
        uses: espressif/esp-idf-ci-action@v1
        with:
          esp_idf_version: v5.1.2
          target: esp32s3
      
      - name: Build hello-world
        run: |
          cd examples/esp32-s3/hello-world
          idf.py build
      
      - name: Build iot-demo
        run: |
          cd examples/esp32-s3/iot-demo
          idf.py build
```

## 📝 Summary

| Approach | Size Impact | Maintenance | CI/CD | Verdict |
|----------|------------|-------------|-------|---------|
| **External (Recommended)** | 0 MB | Easy | Cache-friendly | ✅ Best |
| Git Submodule | +500 MB | Hard | Slow | ❌ Avoid |
| Vendored Copy | +500 MB | Very Hard | Slow | ❌ Never |
| Docker Image | 0 MB (base) | Easy | Fast | ✅ Good for CI |

## 🎓 Final Recommendation

1. **Keep ESP-IDF external** - Use standard installation at `~/.espressif/esp-idf`
2. **Provide setup script** - Automate ESP-IDF installation for users
3. **Document clearly** - Show users how to install ESP-IDF once
4. **Use Docker for CI** - Leverage official ESP-IDF Docker images
5. **Version pinning** - Specify tested ESP-IDF version in docs

This approach:
- Keeps your repository clean and fast
- Follows industry standards
- Makes updates manageable
- Works great with CI/CD
- Respects user's existing ESP-IDF installations
