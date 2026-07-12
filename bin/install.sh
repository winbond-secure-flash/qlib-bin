#!/bin/sh

# Change to the directory where this script resides
cd "$(dirname "$0")"

# 1. Setup variables
ARCH=$(uname -m)
ARCH_DIR="build-$ARCH"

LIB_NAME="libqlib.so"
DEST_DIR="/usr/lib"
SRC_PATH="./$ARCH_DIR/$LIB_NAME"

echo "Checking installation environment..."

# 2. Verify source library exists
if [ ! -f "$SRC_PATH" ]; then
    echo "ERROR: Source library not found at $SRC_PATH"
    echo "Please ensure the folder structure is: ./$ARCH_DIR/$LIB_NAME"
    exit 1
fi

# 3. Check if /usr/lib is writable, try remount if not
if [ ! -w "$DEST_DIR" ]; then
    echo "Notice: $DEST_DIR is read-only. Attempting to remount..."
    mount -o remount,rw / 2>/dev/null
    if [ $? -ne 0 ]; then
        echo "ERROR: Could not remount $DEST_DIR as writable."
        exit 1
    fi
fi

# 4. Copy and set permissions
echo "Installing $LIB_NAME ($ARCH) to $DEST_DIR..."
cp "$SRC_PATH" "$DEST_DIR/"
chmod 755 "$DEST_DIR/$LIB_NAME"

# 5. Refresh the library cache
if command -v ldconfig >/dev/null 2>&1; then
    ldconfig
    echo "Success: Library cache updated."
else
    echo "Warning: ldconfig not found. You may need to reboot or set LD_LIBRARY_PATH."
fi

echo "Done."
