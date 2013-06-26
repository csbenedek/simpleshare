@echo OFF

@pvk2pfx.exe -pvk VoltageKey.pvk -pi 1sirhc1 -spc VoltageCert.spc -pfx VoltagePFX.pfx -po 1sirhc1 -f

@echo Done.