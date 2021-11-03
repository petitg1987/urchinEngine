#!/bin/bash

set -e
cd "$(dirname "$0")"

blenderVersion=$(blender --version | grep -oP '\d.\d+')
targetPath="$HOME/.config/blender/$blenderVersion/scripts/addons/"
mkdir -p $targetPath

cp "./io_export_urchin.py" "$targetPath"

echo "Script deployed into: $targetPath"
echo "[ONE SHOT] Enable exporter in Edit > Preferences... > Add-ons"
echo "[EVERYTIME] Press F3 in Blender, search for 'reload' and select 'Reload Scripts'"
