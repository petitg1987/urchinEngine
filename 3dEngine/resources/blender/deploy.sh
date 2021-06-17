#!/bin/bash

set -e
cd "$(dirname "$0")"

cp ./io_export_urchin.py /home/greg/.config/blender/2.83/scripts/addons/
echo "Press F3 in Blender, search for 'reload' and select 'Reload Scripts'"