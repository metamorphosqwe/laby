$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest

$root = $PSScriptRoot
$buildDir = Join-Path $env:TEMP ("MOIS-tests-" + [guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory -Path $buildDir | Out-Null
$passed = 0
$failed = 0

try {
    foreach ($number in 2..8) {
        $folder = Join-Path $root ("MOIS" + $number)
        $executable = Join-Path $buildDir ("MOIS" + $number + '.exe')
        & gcc -std=c11 -O2 -Wall -Wextra -Werror (Join-Path $folder 'solution.c') -o $executable
        if ($LASTEXITCODE -ne 0) {
            throw ("Compilation failed for MOIS" + $number)
        }

        $testFolder = Join-Path $folder 'tests'
        $testFiles = @(Get-ChildItem -LiteralPath $testFolder -Filter 'test*.txt' -File | Sort-Object Name)
        if ($testFiles.Count -ne 20) {
            throw ("MOIS" + $number + " must contain exactly 20 test*.txt files; found " + $testFiles.Count)
        }
        foreach ($test in $testFiles) {
            $output = & $executable $test.FullName 2>&1
            $exitCode = $LASTEXITCODE
            $actual = (@($output) -join "`n").TrimEnd("`r", "`n")
            $expectedPath = Join-Path (Join-Path $testFolder 'expected') $test.Name
            if (-not (Test-Path -LiteralPath $expectedPath -PathType Leaf)) {
                throw ("Missing expected output for " + $test.FullName)
            }
            $expected = [System.IO.File]::ReadAllText($expectedPath).TrimEnd("`r", "`n")
            if ($exitCode -eq 0 -and $actual -ceq $expected) {
                ++$passed
                Write-Host ("PASS MOIS" + $number + '\' + $test.Name)
            } else {
                ++$failed
                Write-Host ("FAIL MOIS" + $number + '\' + $test.Name)
                $output | ForEach-Object { Write-Host $_ }
            }
        }
    }
} finally {
    Remove-Item -LiteralPath $buildDir -Recurse -Force
}

Write-Host ("Result: " + $passed + " passed, " + $failed + " failed")
if ($failed -gt 0) { exit 1 }
