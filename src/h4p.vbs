Dim strCurDir
Dim shell,command
Set shell = CreateObject("WScript.Shell")
rem msgBox WScript.Arguments.Item(0)
command = "powershell.exe -nologo -File C:\Users\phil\Documents\Arduino\libraries\H4Plugins\src\switch.ps1 " & WScript.Arguments.Item(0)
shell.Run command,0
