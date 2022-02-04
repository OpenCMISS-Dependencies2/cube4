:: 
:: This file is part of the CUBE software (http:\\www.scalasca.org\cube%
::
:: Copyright (c) 2009-2020, 
::    Forschungszentrum Juelich GmbH, Germany
::    German Research School for Simulation Sciences GmbH, Juelich\Aachen, Germany
::
:: Copyright (c) 2009-2014, 
::    RWTH Aachen University, Germany
::    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
::    Technische Universitaet Dresden, Germany
::    University of Oregon, Eugene, USA
::    Forschungszentrum Juelich GmbH, Germany
::    German Research School for Simulation Sciences GmbH, Juelich\Aachen, Germany
::    Technische Universitaet Muenchen, Germany
::
:: See the COPYING file in the package base directory for details.
::
:: maintainer Pavel Saviankou <p.saviankou@fz-juelich.de>


set MINGW="C:\QtSDK\5.13.1\mingw73_64"
set QTDIR="C:\QtSDK\5.13.1\mingw73_64"
set NSIS="C:\Program Files (x86)\NSIS"
set srcdir=..\..\
set PWD=%~dp0
set PREFIX=%1
set TARGET=C:\Scalasca-Soft\Install\Cube2.0\%PREFIX%\install


mkdir  %TARGET%\bin
mkdir  %TARGET%\lib
mkdir  %TARGET%\include\cube
mkdir  %TARGET%\include\cubew
mkdir  %TARGET%\include\cubegui
mkdir  %TARGET%\share\icons
mkdir  %TARGET%\share\doc\cubegui\examples
mkdir  %TARGET%\share\doc\cubegui\guide
mkdir  %TARGET%\share\doc\cubegui\plugins-guide

for %%f in ( 
GUI-qt-core 
GUI-qt
GUI-plugin-advanced-colormap
GUI-plugin-advisor
GUI-plugin-graphwidgetcommon
GUI-plugin-barplot
GUI-plugin-diff
GUI-plugin-heatmap
GUI-plugin-launch      
GUI-plugin-mean        
GUI-plugin-merge       
GUI-plugin-metriceditor
GUI-plugin-metricident 
GUI-plugin-scaling
GUI-plugin-scorepconfig
GUI-plugin-sourcecodeviewer
GUI-plugin-statistics
GUI-plugin-sunburst
GUI-plugin-systemboxplot
GUI-plugin-systemtopology
GUI-plugin-tau2cube
GUI-plugin-treeitemmarker
) do (
echo "Build %%f"
cd  %%f
qmake 
mingw32-make clean
mingw32-make

if errorlevel 1 (
   echo Failure during make : %errorlevel%
   exit /b %errorlevel%
)

cd %PWD%
echo "Copy %%f..."
copy %%f\release\*.exe %TARGET%\bin
copy %%f\release\*.dll %TARGET%\bin
copy %%f\release\*.a   %TARGET%\lib
)


::GOTO:EOF

echo "Copy system libraries..."
echo "Qt located  : %QTDIR%"
copy %MINGW%\bin\libgcc_s_*.dll %TARGET%\bin
copy %MINGW%\bin\libstdc*-6.dll %TARGET%\bin
copy %MINGW%\bin\libwinpthread*.dll %TARGET%\bin
copy %QTDIR%\bin\Qt5Core.dll %TARGET%\bin
copy %QTDIR%\bin\Qt5Concurrent.dll %TARGET%\bin
copy %QTDIR%\bin\Qt5Gui.dll %TARGET%\bin
copy %QTDIR%\bin\Qt5Network.dll %TARGET%\bin
copy %QTDIR%\bin\Qt5PrintSupport.dll %TARGET%\bin
copy %QTDIR%\bin\Qt5Help.dll %TARGET%\bin
copy %QTDIR%\bin\Qt5WebKit.dll %TARGET%\bin
copy %QTDIR%\bin\Qt5Widgets.dll %TARGET%\bin

mkdir %TARGET%\bin\platforms
mkdir %TARGET%\bin\bearer
mkdir %TARGET%\bin\styles
mkdir %TARGET%\bin\imageformats


copy %QTDIR%\plugins\platforms\qwindows.dll %TARGET%\bin\platforms
copy %QTDIR%\plugins\bearer\qgenericbearer.dll %TARGET%\bin\bearer
copy %QTDIR%\plugins\styles\qwindowsvistastyle.dll %TARGET%\bin\styles
copy %QTDIR%\plugins\imageformats\qgif.dll %TARGET%\bin\imageformats
copy %QTDIR%\plugins\imageformats\qicns.dll %TARGET%\bin\imageformats
copy %QTDIR%\plugins\imageformats\qico.dll %TARGET%\bin\imageformats
copy %QTDIR%\plugins\imageformats\qjpeg.dll %TARGET%\bin\imageformats
copy %QTDIR%\plugins\imageformats\qsvg.dll %TARGET%\bin\imageformats
copy %QTDIR%\plugins\imageformats\qtga.dll %TARGET%\bin\imageformats
copy %QTDIR%\plugins\imageformats\qtiff.dll %TARGET%\bin\imageformats
copy %QTDIR%\plugins\imageformats\qqbmp.dll %TARGET%\bin\imageformats
copy %QTDIR%\plugins\imageformats\qwebp.dll %TARGET%\bin\imageformats


echo "Copy GUI header files..."
copy %srcdir%\src\GUI-qt\display\*.h  %TARGET%\include\cubegui\.
copy %srcdir%\vpath\src\cubegui*.h  %TARGET%\include\cubegui\.
copy %srcdir%\src\GUI-qt\display\plugins\*.h  %TARGET%\include\cubegui\.

echo "Copy examples files..."
copy %srcdir%\examples\GUI\trace.cubex  %TARGET%\share\doc\cubegui\examples\.




@echo Copy Cube documentation..."
copy  %srcdir%\doc\*.pdf %TARGET%\share\doc\cubegui
echo "Copy html docu"
copy %srcdir%\doc\*.tar.gz .
tar xzf guide-html.tar.gz
echo "Copy unpacked User Guide" 
ls -la 
xcopy /hievry guide\html\* %TARGET%\share\doc\cubegui\guide\html
tar xzf plugins-guide-html.tar.gz
echo "Copy unpacked Plugin User Guide" 
ls -la 
xcopy /hievry plugins-guide\html\* %TARGET%\share\doc\cubegui\plugins-guide\html


@echo "Copy Cube images..."
copy  %srcdir%\share\icons\* %TARGET%\share\icons\.
@echo "Copy Cube license..."
copy  %srcdir%\COPYING %TARGET%\cubelicense.txt

copy  %srcdir%\vendor\windows\cubeinstaller.nsi %TARGET%\..


echo "NOW COMPILE NSIS script..." 

if errorlevel 1 (
   echo Failure during copy : %errorlevel%
   exit /b %errorlevel%
)


