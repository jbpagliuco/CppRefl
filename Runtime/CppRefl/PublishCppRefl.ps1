# Directory of this project
$CppReflDir = $PSScriptRoot

# Directory containing source files
$SourceDir = "$CppReflDir/Source"

# Binaries of the built CppRefl project
$BinariesDir = "$CppReflDir/x64"

$BuildConfigs = "Debug","Release"


function RecreateDir {
	param (
        $Directory
    )
    
    $null = New-Item -Path $Directory -ItemType "directory" -Force
    Get-ChildItem -Path $Directory -Include *.* -Recurse | Remove-Item -Force
}


# Publish directory
$PublishDir = "$CppReflDir/../Publish/CppRefl"
Write-Output "Publishing to $PublishDir"
RecreateDir $PublishDir



# Copy Headers
$PublishIncludeDir = "$PublishDir/include"
RecreateDir $PublishIncludeDir
$null = robocopy "$SourceDir" "$PublishIncludeDir" /E "*.h"
$null = robocopy "$SourceDir" "$PublishIncludeDir" /E "*.inl"

# Copy Binaries
foreach ($config in $BuildConfigs)
{
    $PublishBinariesDir = "$PublishDir/lib/$config"
    RecreateDir $PublishBinariesDir
    Copy-Item -Path "$BinariesDir/$config/CppRefl.lib" -Destination $PublishBinariesDir
    Copy-Item -Path "$BinariesDir/$config/CppRefl.pdb" -Destination $PublishBinariesDir
}