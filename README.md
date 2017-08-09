# SSDT-Hook
VS2015+WDK10.0.15063.0

This is a ssdt-hook program working on Windows7 home premium x86 (6.1.7600 version)

Not sure whether it would work on other versions.

If you want to compile by yourself ,remember to set the Project preference - Driver Settings - Target OS to Windows7 and Target Platform to Desktop. By the way, add Wdmsec.lib to this project, otherwise, you may can't use the IoCreateDeviceSecure().

Q:why not on x64 platform?

A:Because I can't get across the PG :( while at the same time, loading driver on x64 needs we sign the driver, but I can't do that. I can't promise, but I'll try to do that in the future.

If there is any other problems, please comment in issues. :)
