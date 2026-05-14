cmake --build build --config Debug --target all

/opt/homebrew/bin/geode package new "$PWD" \
    --binary "$PWD/build/vex.super-dash-menu.dylib" \
    --output "$PWD/build/vex.super-dash-menu.geode"

mkdir -p "/Users/holdensibary/Library/Application Support/Steam/steamapps/common/Geometry Dash/Geometry Dash.app/Contents/geode/mods"
cp "$PWD/build/vex.super-dash-menu.geode" "/Users/holdensibary/Library/Application Support/Steam/steamapps/common/Geometry Dash/Geometry Dash.app/Contents/geode/mods"

open -a "Geometry Dash"