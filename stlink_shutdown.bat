@echo off
for /f "tokens=5" %%a in ('netstat -ano ^| findstr :61234') do (
    taskkill /PID %%a /F
)
pause
