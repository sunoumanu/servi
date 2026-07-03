# servi
ESP32 family board controls the SG90 micro servo motor

Uses IDF framework for ESP32 
https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/index.html
https://github.com/espressif/esp-idf

Instructions for windows 11

# Driver installation
`idf-env.exe driver install --espressif`

# Find the COM port to program (powershell)
`Get-PnpDevice -PresentOnly | Where-Object { $_.InstanceId -match "USB" -and $_.Name -match "COM" } | Select-Object Name, FriendlyName`

```
Name                     FriendlyName
----                     ------------
USB Composite Device     USB Composite Device
USB Serial Device (COM5) USB Serial Device (COM5)
USB Composite Device     USB Composite Device
```

# Set appropriate target
`idf.py set-target esp32s3`

# Build
`idf.py build`

# Flash and monitor (use COM port from 2)
`idf.py -p COM5 flash monitor`

6) Enjoy rotating servo - see showcase
https://youtube.com/shorts/qaIh2n1oFwc?si=qEFv2e9FymnzXn8B
