REM fully qualified path name of the installation directory is needed
REM target directory must exist
REM msiexec.exe /i python-2.5.msi TARGETDIR="D:\vapps_merge_target\Branch22\LHPBuilder_Parabuild\Python\Python25" /qb
msiexec.exe /i python-2.5.msi TARGETDIR="%1" /qb
