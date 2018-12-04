Convert to sparse file utility

The utility is intended for converting files into sparse ones on Windows NTFS file system.
Sparse files are those in which long zero blocks are not stored on hard disk, but replaced
to information about them (metadata) instead.
These files are very useful for saving space on storing half-empty ISO files,
half-downloaded torrent files, virtual machine disk images.

More about them: https://en.wikipedia.org/wiki/Sparse_file
http://msdn.microsoft.com/en-us/library/windows/desktop/aa365564(v=vs.85).aspx

[![Build status](https://ci.appveyor.com/api/projects/status/a35yskq44fsei2nu?svg=true)](https://ci.appveyor.com/project/WallyCZ/cvt2sparse)

Compiled executable file: https://github.com/WallyCZ/cvt2sparse/releases/latest

-- dennis(a)yurichev.com
