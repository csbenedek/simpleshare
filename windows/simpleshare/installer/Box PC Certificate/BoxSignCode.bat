@echo OFF
@echo signing  %1
REM @echo caling signtool.exe sign /f box_key.pfx /p hhrb251427 %1
@signtool.exe sign /f box_key.pfx /p hhrb251427 %1
REM @echo.

REM @echo calling signtool.exe timestamp -t http://timestamp.verisign.com/scripts/timestamp.dll %1
@signtool.exe timestamp -t http://timestamp.verisign.com/scripts/timestamp.dll %1
REM @echo.

REM @echo signTool.exe Verify -pa -tw %1
@signTool.exe Verify -pa -tw %1
REM @echo.

:Done
@echo Done.
