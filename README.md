# SSDT-Hook
VS2015+WDK10.0.15063.0
  This is a ssdt-hook program working on Windows7 home premium x86 (6.1.7600 version)
  Not sure whether it would work on other versions.
  If you want to compile by yourself ,remember to set the projecr perference-Driver Seetings-Target OS to Windows7 and Target Platfform to Desktop.By the way, add Wdmsec.lib to this project,otherwise,you
  may can't use the IoCreateDeviceSecure().
  It's not finished yet,we just get the address of NtWriteFile in kernel.
  If there is any other problems,please commit. :)
