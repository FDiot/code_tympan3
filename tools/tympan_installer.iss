[Setup]
AppName=Code_TYMPAN
AppVersion=4.2.9
DefaultDirName=C:\Code_TYMPAN
DefaultGroupName=Code_TYMPAN
OutputBaseFilename=setup_Code_TYMPAN
; won't require admin rights to install (per-user install)
PrivilegesRequired=lowest
ChangesEnvironment=true

[Files]
;; python 32 bits
Source: "Python27\*"; DestDir: "{app}\Python27"; Flags: recursesubdirs
;; Code_TYMPAN
; py scripts
Source: "install-Code_TYMPAN\bin\*"; DestDir: "{app}\bin"; Excludes: "*.pyc";
; python/cython modules
Source: "install-Code_TYMPAN\cython\*"; DestDir: "{app}\cython"; Excludes: "*.pyc"; Flags: recursesubdirs
; c++ acoustic solvers
Source: "install-Code_TYMPAN\plugins\*"; DestDir: "{app}\plugins";
; fonts/images
Source: "install-Code_TYMPAN\resources\*"; DestDir: "{app}\resources"; Flags: recursesubdirs
; C++ libs
Source: "install-Code_TYMPAN\lib\*"; DestDir: "{app}\libs";
; 3rd party libraries
Source: "install-Code_TYMPAN\*"; DestDir: "{app}";
; copy vc redist (post install)
Source: "vcredist\*.dll"; DestDir: "{app}";

[Registry]
Root: HKCU; Subkey: "Environment"; ValueType: string; ValueName: "CGAL_BINDINGS_PATH"; ValueData: "{app}\cython"; Flags: uninsdeletekey; Permissions: everyone-read
Root: HKCU; Subkey: "Environment"; ValueType: string; ValueName: "TYMPAN_PYTHON_INTERP"; ValueData: "{app}\Python27\python.exe"; Flags: uninsdeletekey; Permissions: everyone-read


