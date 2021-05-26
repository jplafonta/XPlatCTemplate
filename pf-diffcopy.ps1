$folderSrc = "C:\git\pf\sdks\XPlatCSdk\"
$folderDest = "C:\git\pf\sdks\XPlatCSdkWorkingCopy\"

Get-ChildItem -Path "C:\git\pf\sdks\XPlatCSdk" -Recurse -ErrorAction SilentlyContinue -Include *.h,*.cpp,*.autogen,*.vcxproj*,*.props -Force -Name | % { 
	# Write-Output ($_) 
	Write-Host -NoNewLine .

	$fileA = $folderSrc + $_
	$fileB = $folderDest + $_
	if(!(Test-Path -Path $fileB -PathType Leaf))
	{
		Write-Output ("")
		Write-Output ($fileA + " is new. copying")		
		Copy-Item $fileA -Destination $fileB
	}
	else 
	{
		if(Compare-Object -ReferenceObject $(Get-Content $fileA) -DifferenceObject $(Get-Content $fileB)) 
		{
			Write-Output ("")
			Write-Output ($fileA + " is different. copying")		
			Copy-Item $fileA -Destination $fileB
		}
	}
}	

