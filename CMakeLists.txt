cmake_minimum_required(VERSION 3.22)
project(PROYECT)

set(CMAKE_CXX_STANDARD 17)

# Agregar el directorio donde se encuentra `httplib.h` a las rutas de inclusión
# Asegúrate de que la ruta sea correcta. A continuación se asume que `cpp-httplib` está en el mismo directorio que tu CMakeLists.txt.
include_directories(${CMAKE_SOURCE_DIR}/cpp-httplib)

# Si `array.h` y `array.cpp` son parte de tu proyecto, asegúrate de incluirlos también.
add_executable(PROYECT main.cpp ) # Suponiendo que tu implementación de Array está en array.cpp

# Si `array.h` no tiene un archivo de implementación separado (array.cpp) y todo está en array.h, simplemente omite array.cpp:
# add_executable(PROYECT main.cpp)
