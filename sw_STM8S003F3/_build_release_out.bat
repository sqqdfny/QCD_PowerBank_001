@echo off
del  s19\*release*s19
copy Release\main.s19 s19\QCD_PowerBank_001_release_%date:~0,4%_%date:~5,2%_%date:~8,2%.s19
copy Release\main.s19 s19_history\QCD_PowerBank_001_release_%date:~0,4%_%date:~5,2%_%date:~8,2%.s19











