#!/bin/sh

## 
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2009-2013, 
##    RWTH Aachen, Germany
##    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##    Technische Universitaet Dresden, Germany
##    University of Oregon, Eugene, USA
##    Forschungszentrum Juelich GmbH, Germany
##    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
##    Technische Universitaet Muenchen, Germany
##
## See the COPYING file in the package base directory for details.
##

## file generate_library_dependencies.sh
##      Parse all *.la files in the current working directory and
##      recursively the *.la files listed in dependency_libs. Generate
##      a C++ representation of the library dependencies and write it
##      to stdout. To be used with the <component>-config tools.
##
##      Assumes that teh client provide std::vector<std::string> libs,
##      ldflags, rpaths, dependency_las and a std::map<std::string,
##      la_object> la_objects. The vectors are temporary, the map will
##      contain the entire information indexed by the library name.The
##      la_object ctor signature is
##      la_object(std::string lib_name,
##                std::string build_dir,
##                std::string install_dir,
##                std::vector<std::string> libs,
##                std::vector<std::string> ldflags,
##                std::vector<std::string> rpaths,
##                std::vector<std::string> dependency_las)
##      e.g.:
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

#set -x
set -e

parse_la () 
{
    la_file="$1"

    lib_name=`basename $la_file`
    lib_name=${lib_name%.la}

    # change to .la directory
    old_pwd=`pwd`
    build_dir=`dirname $la_file`
    cd "${build_dir}"

    # extract information form .la"s libdir (=install_dir)
    # later, use @EGREP@
    eval `grep -E "^libdir=" "${la_file}"`
    install_dir="${libdir}"

    # ignore conveniece libraries
    if test "x${libdir}" = "x"; then
        return
    fi

    # extract information form .la"s dependency_libs
    eval `grep -E "^dependency_libs=" "${la_file}"`
    # use @AWK@
    save_ifs="${IFS}"
    IFS=${awk_ifs}
    eval `echo ${dependency_libs} | awk '
    {
        for (i=1; i<= NF; i++) 
        { 
            if      (index($i, "-L") == 1) {ldflags = ldflags $i ":"
                                            sub(/L/, "R", $i ) 
                                            rpath = rpath $i ":"} 
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
    IFS="${save_ifs}"

    # generate output
    echo "    libs.clear();"
    echo "    ldflags.clear();"
    echo "    rpaths.clear();"
    echo "    dependency_las.clear();"

    save_ifs="${IFS}"
    IFS=":"

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
    echo

    # call parse_la recursively on all libs in $dependency_la
    for i in ${dependency_la}; do
         (parse_la ${i})
    done

    IFS="${save_ifs}"
    cd "${old_pwd}"
}

awk_ifs="${IFS}"
for i; do
    (parse_la ${i})
done
