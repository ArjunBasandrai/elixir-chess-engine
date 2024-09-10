Get-ChildItem -Recurse -Include *.cpp,*.h | ForEach-Object { clang-format -i $_.FullName }
