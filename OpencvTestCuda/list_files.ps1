param (
    [string]$Filter,         # Filter pattern, e.g., "nppi*.lib"
    [string]$OutputFile      # Output file name, e.g., "nppi_dep_list.txt"
)

# Check if both parameters are provided
if (-not $Filter -or -not $OutputFile) {
    Write-Host "Usage: .\list_files.ps1 -Filter 'pattern' -OutputFile 'output.txt'"
    exit
}

# Get matching file names, join them with a semicolon, and output to the file
$files = Get-ChildItem -Path . -Filter $Filter -Recurse | ForEach-Object { $_.Name }
$joinedFiles = $files -join ';'
$joinedFiles | Out-File -FilePath $OutputFile -Encoding UTF8

Write-Host "File list saved to $OutputFile"
