cmake_minimum_required(VERSION 3.0)
project(Polynomials C)

if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default to Release")
    set(CMAKE_BUILD_TYPE "Release")
endif ()

# Nie chcemy oglądać komunikatów o procentowym postępie kompilacji.
set_property(GLOBAL PROPERTY RULE_MESSAGES OFF)

# Chcemy zobaczyć polecenia wywołane przez make.
set(CMAKE_VERBOSE_MAKEFILE ON)

# Ustawiamy wspólne opcje kompilowania dla wszystkich wariantów projektu.
set(CMAKE_C_FLAGS "-std=c11 -Wall -Wextra -O2")
# Domyślne opcje dla wariantów Release i Debug są sensowne.
# Jeśli to konieczne, ustawiamy tu inne.
# set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")
# set(CMAKE_C_FLAGS_DEBUG "-g")

# Wskazujemy pliki źródłowe (kalkulatora).
set(SOURCE_FILES
        src/poly_core/poly.c
        src/poly_core/poly.h
        src/poly_core/poly_structures.h
        src/calc_core/calc_engine.c
        src/calc_core/calc_engine.h
        src/calc_core/line_structures.h
        src/calc_core/calc.c
        src/calc_core/calc.h
        src/calc_core/line_parsing.c
        src/calc_core/line_parsing.h
        src/utils/safe_allocations.h
        src/utils/vector.c
        src/utils/vector.h
        )

# Wskazujemy pliki źródłowe (testów).
set(TEST_SOURCE_FILES
        src/poly_core/poly.c
        src/poly_core/poly.h
        src/poly_core/poly_structures.h
        src/poly_test.c
        )

# Wskazujemy plik wykonywalny (kalkulatora).
add_executable(poly ${SOURCE_FILES})

# Wskazujemy plik wykonywalny (testów).
add_executable(test EXCLUDE_FROM_ALL ${TEST_SOURCE_FILES})
set_target_properties(test PROPERTIES OUTPUT_NAME poly_test)

# Dodajemy obsługę Doxygena: sprawdzamy, czy jest zainstalowany i jeśli tak to:
find_package(Doxygen)
if (DOXYGEN_FOUND)
    # Wskazujemy lokalizacją pliku konfiguracyjnego i podajemy jego docelową lokalizację w folderze, gdzie następuje kompilacja.
    configure_file(Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    # Dodajemy cel doc: użycie make doc spowoduje wywołanie doxygena z odpowiednim plikiem konfiguracyjnym w folderze kompilacji.
    # Na wyjście zostanie wypisany odpowiedni komentarz.
    add_custom_target(doc
            ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            )
endif (DOXYGEN_FOUND)