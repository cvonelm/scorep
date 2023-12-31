#!/bin/sh

##
## This file is part of the Score-P software ecosystem (http://www.score-p.org)
##
## Copyright (c) 2009-2013,
## RWTH Aachen University, Germany
##
## Copyright (c) 2009-2013,
## Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##
## Copyright (c) 2009-2014,
## Technische Universitaet Dresden, Germany
##
## Copyright (c) 2009-2013,
## University of Oregon, Eugene, USA
##
## Copyright (c) 2009-2013, 2017,
## Forschungszentrum Juelich GmbH, Germany
##
## Copyright (c) 2009-2013,
## German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
##
## Copyright (c) 2009-2013,
## Technische Universitaet Muenchen, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##


##      usage: ./generate_library_dependencies.sh <tag> <file_1>.la [<file_2>.la] > <result>
##
##      see also build-config/generate-library-dependencies-la-object.hpp
##
##      Parse given *.la files and recursively all *.la files listed
##      in dependency_libs of the given *.la files. Generate a C++
##      representation of the library dependencies and write it
##      to stdout. To be used with the <component>-config tools.
##      Specify a <tag> to distinguish functions generated by
##      different script invocations.
##
##      Generates a function
##      void add_library_dependencies_<tag>( std::deque<std::string>& libs,
##                                           std::deque<std::string>& ldflags,
##                                           std::deque<std::string>& rpaths,
##                                           std::deque<std::string>& dependency_las,
##                                           std::map< std::string, la_object>* la_objects )
##      {
##          <tag>_dependency_1( libs, ldflags, rpaths, dependency_las, la_objects );
##          <tag>_dependency_2( libs, ldflags, rpaths, dependency_las, la_objects );
##          ...
##          <tag>_dependency_n( libs, ldflags, rpaths, dependency_las, la_objects );
##      }
##      to be called by the beneficiary of this generator.
##
##      The functions <tag>_dependency_<n> are also generated and work
##      on the temporary libs, ldflags, rpaths, and dependency_las to
##      fill the provided la_objects map, which will contain the
##      entire dependency information indexed by the library
##      name. Uses the struct la_object defined in
##      generate-library-dependencies-la-object.hpp
##
##      An example <tag>_dependency_<n> would look like:
##
##      libs.clear();
##      ldflags.clear();
##      rpaths.clear();
##      dependency_las.clear();
##      libs.push_back( "-lbfd" );
##      libs.push_back( "-lpapi" );
##      libs.push_back( "-lm" );
##      ldflags.push_back( "-L/opt/packages/papi/4.1.2.1/lib" );
##      if ( la_objects.find( "libscorep_mpi" ) != la_objects.end() )
##      {
##          la_objects[ "libscorep_mpi" ] =
##              la_object( "libscorep_mpi",
##                         "/home/roessel/silc/build/trunk/gcc_openmpi_static_x86-32/build-backend",
##                         "/home/roessel/silc/build/trunk/gcc_openmpi_static_x86-32/install/lib",
##                         libs,
##                         ldflags,
##                         rpaths,
##                         dependency_las );
##       }

set -e

parse_la ()
{
    IFS=${default_ifs}
    la_file="$1"

    lib_name=`basename $la_file`
    lib_name=${lib_name%.la}

    # change to .la directory
    old_pwd=`pwd`
    build_dir=`dirname $la_file`
    cd "${build_dir}"
    build_dir=`pwd`

    # extract information from .la"s libdir (=install_dir)
    # later, use @EGREP@
    eval `GREP_OPTIONS= grep -E "^libdir=" "${la_file}"`
    install_dir="${libdir}"

    # ignore convenience libraries
    if test "x${libdir}" = "x"; then
        return
    fi

    # extract information from .la"s dependency_libs
    eval `GREP_OPTIONS= grep -E "^dependency_libs=" "${la_file}"`
    # use @AWK@
    eval `echo ${dependency_libs} | awk '
    {
        for (i=1; i<= NF; i++)
        {
            if      (index($i, "-L") == 1) {ldflags = ldflags $i ":"}
            else if (index($i, "-R") == 1) {rpath = rpath $i ":"}
            else if (index($i, "-l") == 1) {libs = libs $i ":"}
            else                           {dependency_la = dependency_la $i ":"}
        }
    }
    END {
        gsub(/:$/, "", ldflags)
        gsub(/:$/, "", rpath)
        gsub(/:$/, "", libs)
        gsub(/:$/, "", dependency_la)
        print "ldflags=\"" ldflags "\"; " "rpath=\"" rpath "\"; " "libs=\"" libs "\"; " "dependency_la=\"" dependency_la "\""
    }'`

    # generate output
    fct_counter=$(($fct_counter+1))
    fct_name="${tag}_dependency_${fct_counter}"
    echo "static"
    echo "void ${fct_name}( std::deque<std::string>& libs, std::deque<std::string>& ldflags, std::deque<std::string>& rpaths, std::deque<std::string>& dependency_las, map< std::string, la_object>* la_objects )"
    echo "{"
    echo "    libs.clear();"
    echo "    ldflags.clear();"
    echo "    rpaths.clear();"
    echo "    dependency_las.clear();"

    IFS=${colon_ifs}
    for i in ${libs}; do
        printf '    libs.push_back( "%s" );\n' "${i}"
    done

    for i in ${ldflags}; do
        printf '    ldflags.push_back( "%s" );\n' "${i}"
    done

    for i in ${rpath}; do
        printf '    rpaths.push_back( "%s" );\n' "${i}"
    done

    for i in ${dependency_la}; do
        dependency_la_name=`basename ${i}`
        dependency_la_name=${dependency_la_name%.la}
        printf '    dependency_las.push_back( "%s" );\n' "${dependency_la_name}"
    done

    printf '    if ( la_objects->find( "%s" ) == la_objects->end() )\n' "${lib_name}"
    echo   "    {"
    printf '        (*la_objects)[ "%s" ] =\n' "${lib_name}"
    printf '            la_object( "%s",\n' "${lib_name}"
    printf '                       "%s",\n' "${build_dir}"
    printf '                       "%s",\n' "${install_dir}"
    echo   "                       libs,"
    echo   "                       ldflags,"
    echo   "                       rpaths,"
    echo   "                       dependency_las );"
    echo   "    }"
    echo   "}"
    echo

    cd "${old_pwd}"

    # call parse_la recursively on all libs in $dependency_la
    for i in ${dependency_la}; do
         parse_la ${i}
    done

    IFS="${default_ifs}"
}

echo "#include <generate-library-dependencies-la-object.hpp>"
echo

tag=$1
shift

fct_counter=0
default_ifs="${IFS}"
colon_ifs=":"
for i; do
    parse_la ${i}
done

echo "static"
echo "void add_library_dependencies_${tag}( std::deque<std::string>& libs, std::deque<std::string>& ldflags, std::deque<std::string>& rpaths, std::deque<std::string>& dependency_las, std::map< std::string, la_object>* la_objects )"
echo "{"
# do not use the single-printf trick, if ${fct_counter} == 0, then printf still
# prints the format-specifier once with 0 as the default value for not specified
# arguments
for i in $(seq ${fct_counter}); do
    printf "    ${tag}_dependency_%d( libs, ldflags, rpaths, dependency_las, la_objects );\n" $i
done
echo "}"
