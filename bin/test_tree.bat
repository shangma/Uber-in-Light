VLC_tester.exe -s -t long_test\test.rand -if Tree24.avi -zero 12 -one 9 -m 0 -v
VLC_tester.exe -s -t long_test\test.rand -if Tree24.avi -zero 12 -one 9 -m 1 -v
VLC_tester.exe -s -t long_test\test.rand -if Tree24.avi -zero 12 -one 9 -m 0 -v -ec 1
VLC_tester.exe -s -t long_test\test.rand -if Tree24.avi -zero 12 -one 9 -m 1 -v -ec 1
rem VLC_tester.exe -s -t long_test\test.rand -if Tree24.avi -zero 12 -one 9 -m 0 -v -ec 2
rem VLC_tester.exe -s -t long_test\test.rand -if Tree24.avi -zero 12 -one 9 -m 1 -v -ec 2
VLC_tester.exe -s -t long_test\test.rand -if Helicopter24.avi -zero 12 -one 9 -m 0 -v
VLC_tester.exe -s -t long_test\test.rand -if Helicopter24.avi -zero 12 -one 9 -m 1 -v
VLC_tester.exe -s -t long_test\test.rand -if Helicopter24.avi -zero 12 -one 9 -m 0 -v -ec 1
VLC_tester.exe -s -t long_test\test.rand -if Helicopter24.avi -zero 12 -one 9 -m 1 -v -ec 1
rem VLC_tester.exe -s -t long_test\test.rand -if Helicopter24.avi -zero 12 -one 9 -m 0 -v -ec 2
rem VLC_tester.exe -s -t long_test\test.rand -if Helicopter24.avi -zero 12 -one 9 -m 1 -v -ec 2
VLC_tester.exe -s -t long_test\test.rand -if desktop24.avi -zero 12 -one 9 -m 0 -v
VLC_tester.exe -s -t long_test\test.rand -if desktop24.avi -zero 12 -one 9 -m 1 -v
VLC_tester.exe -s -t long_test\test.rand -if desktop24.avi -zero 12 -one 9 -m 0 -v -ec 1
VLC_tester.exe -s -t long_test\test.rand -if desktop24.avi -zero 12 -one 9 -m 1 -v -ec 1
rem VLC_tester.exe -s -t long_test\test.rand -if desktop24.avi -zero 12 -one 9 -m 0 -v -ec 2
rem VLC_tester.exe -s -t long_test\test.rand -if desktop24.avi -zero 12 -one 9 -m 1 -v -ec 2
VLC_tester.exe -s -t long_test\test.rand -if img2.jpg -zero 12 -one 9 -m 0
VLC_tester.exe -s -t long_test\test.rand -if img2.jpg -zero 12 -one 9 -m 1
rem VLC_tester.exe -s -t long_test\test.rand -if img2.jpg -zero 12 -one 9 -m 0 -ec 1
rem VLC_tester.exe -s -t long_test\test.rand -if img2.jpg -zero 12 -one 9 -m 1 -ec 1

VLC_tester.exe -s -t long_test\test.rand -if CoastalTown24.avi -zero 12 -one 9 -m 0 -v
VLC_tester.exe -s -t long_test\test.rand -if CoastalTown24.avi -zero 12 -one 9 -m 1 -v
rem VLC_tester.exe -s -t long_test\test.rand -if CoastalTown24.avi -zero 12 -one 9 -m 0 -v -ec 1
rem VLC_tester.exe -s -t long_test\test.rand -if CoastalTown24.avi -zero 12 -one 9 -m 1 -v -ec 1
rem VLC_tester.exe -s -t abcde -if Tree24.avi -m 2 -v
rem VLC_tester.exe -s -t abcde -if Tree24.avi -m 3 -v
rem throughput coastal
VLC_tester.exe -s -t 4x4.rand -side 2 -if CoastalTown24.avi -zero 12 -one 9 -m 5 -v
VLC_tester.exe -s -t 4x4.rand -side 4 -if CoastalTown24.avi -zero 12 -one 9 -m 5 -v
VLC_tester.exe -s -t 8x8.rand -side 8 -if CoastalTown24.avi -zero 12 -one 9 -m 5 -v
VLC_tester.exe -s -t 16x16.rand -side 16 -if CoastalTown24.avi -zero 12 -one 9 -m 5 -v
VLC_tester.exe -s -t 32x32.rand -side 32 -if CoastalTown24.avi -zero 12 -one 9 -m 5 -v
rem throughput tree
VLC_tester.exe -s -t 2x2.rand -side 2 -if Tree24.avi -zero 12 -one 9 -m 5 -v
VLC_tester.exe -s -t 4x4.rand -side 4 -if Tree24.avi -zero 12 -one 9 -m 5 -v
VLC_tester.exe -s -t 8x8.rand -side 8 -if Tree24.avi -zero 12 -one 9 -m 5 -v
VLC_tester.exe -s -t 16x16.rand -side 16 -if Tree24.avi -zero 12 -one 9 -m 5 -v
VLC_tester.exe -s -t 32x32.rand -side 32 -if Tree24.avi -zero 12 -one 9 -m 5 -v
