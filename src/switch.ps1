# nuget.exe install M2Mqtt -o c:\lib
# run with elevated privilege (runas administrator
#param([switch]$Elevated)
#function Check-Admin {
#    $currentUser = New-Object Security.Principal.WindowsPrincipal $([Security.Principal.WindowsIdentity]::GetCurrent())
#    $currentUser.IsInRole([Security.Principal.WindowsBuiltinRole]::Administrator)
#}

#if ((Check-Admin) -eq $false) {
#    if (!$elevated) { Start-Process powershell.exe -WindowStyle Hidden -Verb RunAs -ArgumentList ('-noprofile -noexit -file "{0}" -elevated' -f ($myinvocation.MyCommand.Definition)) }
#    exit
#}
#Write-host $args
Add-Type -Path "C:\lib\M2Mqtt.4.3.0.0\lib\net45\M2Mqtt.Net.dll"
$argie = $args[0]
$chip = $argie.substring($argie.length - 6, 6)
$MqttClient = [uPLibrary.Networking.M2Mqtt.MqttClient]("192.168.1.4")
$Mqttclient.Connect([guid]::NewGuid(), "", "")
$MqttClient.Publish("$chip/h4/toggle", [System.Text.Encoding]::UTF8.GetBytes(""))