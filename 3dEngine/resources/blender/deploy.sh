#!/bin/bash

set -e
cd "$(dirname "$0")"

blenderVersion=$(blender --version | grep -oP '\d.\d+')
targetPath="$HOME/.config/blender/$blenderVersion/scripts/addons/"

cp "./io_export_urchin.py" "$targetPath"

echo "Script deployed into: $targetPath"
echo "Press F3 in Blender, search for 'reload' and select 'Reload Scripts'"
