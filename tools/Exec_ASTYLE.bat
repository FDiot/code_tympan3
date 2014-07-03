echo "Setup and execute AStyle on Code_TYMPAN"

echo "Setting ASTYLE PATH..."
set ASTYLEDIR=C:\Program Files\AStyle
set PATH=%ASTYLEDIR%\bin

echo "Doing AStyle on .cpp"
astyle --options=./astylerc ../Tympan/*.cpp

echo "Doing AStyle on .h"
astyle --options=./astylerc ../Tympan/*.h