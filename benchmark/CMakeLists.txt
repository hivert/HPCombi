#****************************************************************************#
#       Copyright (C) 2016 Florent Hivert <Florent.Hivert@lri.fr>,           #
#                                                                            #
#  Distributed under the terms of the GNU General Public License (GPL)       #
#                                                                            #
#    This code is distributed in the hope that it will be useful,            #
#    but WITHOUT ANY WARRANTY; without even the implied warranty of          #
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       #
#   General Public License for more details.                                 #
#                                                                            #
#  The full text of the GPL is available at:                                 #
#                                                                            #
#                  http://www.gnu.org/licenses/                              #
#****************************************************************************#

find_package(PkgConfig REQUIRED) 
pkg_check_modules(BENCHMARK REQUIRED IMPORTED_TARGET benchmark)

include_directories(
  ${CMAKE_SOURCE_DIR}/include
  ${PROJECT_BINARY_DIR})

set(benchmark_src
  bench_epu8.cpp bench_perm16.cpp bench_bmat8.cpp sort.cpp inverse.cpp)

foreach(f ${benchmark_src})
  get_filename_component(benchName ${f} NAME_WE)
  add_executable (${benchName} ${f})
  target_link_libraries(${benchName} PkgConfig::BENCHMARK pthread)
  # install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${benchName}
  #   DESTINATION bin
  #   RENAME ${CMAKE_PROJECT_NAME}-${benchName})
endforeach(f)
