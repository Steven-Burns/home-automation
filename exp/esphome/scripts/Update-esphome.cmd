winget upgrade --id Python.Python.3.12  --silent
python.exe -m pip install --upgrade pip
; upgrading esphome in-place has sometimes failed for me, 
; so I scorch and re-install
rd "$env:USERPROFILE\.platformio" -recurse -force
pip3 uninstall esphome 
pip3 install esphome