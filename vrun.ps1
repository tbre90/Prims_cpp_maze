if (Test-Path .\build\main.exe)
{
    .\build\main.exe $args 
}
else
{
    Write-Output(".\build\main.exe does not exist!")
}
