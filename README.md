This file is part of sisu.
sisu is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

sisu is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with sisu.  If not, see <http://www.gnu.org/licenses/>.

sisu productivity framework
=======

SISU is a unit test framework intended to facilitate the transition 
of code from unit tests into many small libraries and executables.

Tests are written in test directories and code is written in source
directories. A special entry point is included in sisuTest which links 
in all libraries and tests. However, separate from this, the framework 
is arbitrary and may include arbitary separations of executable, shared
and static library releases.

SISU is still under development and runs on Android, Windows 7 and 
Linux. A transition to CMAKE is planned.
