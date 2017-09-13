#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/src/MLC/Clustering/boost_plugin.o \
	${OBJECTDIR}/src/MLC/Clustering/ccbc_context.o \
	${OBJECTDIR}/src/MLC/Clustering/cluster.o \
	${OBJECTDIR}/src/MLC/Clustering/cluster_context.o \
	${OBJECTDIR}/src/MLC/Clustering/clusterdb.o \
	${OBJECTDIR}/src/MLC/Clustering/clustering.o \
	${OBJECTDIR}/src/MLC/Clustering/clusterproperties.o \
	${OBJECTDIR}/src/MLC/Clustering/comparison.o \
	${OBJECTDIR}/src/MLC/Clustering/double_matrix.o \
	${OBJECTDIR}/src/MLC/Clustering/double_vector.o \
	${OBJECTDIR}/src/MLC/Clustering/eigenval.o \
	${OBJECTDIR}/src/MLC/Clustering/log.o \
	${OBJECTDIR}/src/MLC/Clustering/matrix_context.o \
	${OBJECTDIR}/src/MLC/Clustering/mlc_context.o \
	${OBJECTDIR}/src/MLC/Clustering/nfieldbase.o \
	${OBJECTDIR}/src/MLC/Clustering/std_plugin.o \
	${OBJECTDIR}/src/MLC/Clustering/task.o \
	${OBJECTDIR}/src/MLC/Clustering/taskmap.o \
	${OBJECTDIR}/src/MLC/Clustering/toolbox.o \
	${OBJECTDIR}/src/MLC/Clustering/tostring.o \
	${OBJECTDIR}/src/MLC/Clustering/uclust_context.o \
	${OBJECTDIR}/src/MLC/Clustering/wait.o \
	${OBJECTDIR}/src/MLC/MfcCluster/consoleappentrypoint.o \
	${OBJECTDIR}/src/MLC/MfcCluster/mfcclusterbusiness.o \
	${OBJECTDIR}/src/MLC/MfcCluster/optimizer.o \
	${OBJECTDIR}/src/MLC/MfcCluster/testfieldbase.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-pthread -DBOOST_SYSTEM_NO_DEPRECATED -lboost_system
CXXFLAGS=-pthread -DBOOST_SYSTEM_NO_DEPRECATED -lboost_system

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/cluster

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/cluster: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/cluster ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/src/MLC/Clustering/boost_plugin.o: src/MLC/Clustering/boost_plugin.cpp
	${MKDIR} -p ${OBJECTDIR}/src/MLC/Clustering
	${RM} "$@.d"
	$(COMPILE.cc) -g -Isrc/Eigen323 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/MLC/Clustering/boost_plugin.o src/MLC/Clustering/boost_plugin.cpp

${OBJECTDIR}/src/MLC/Clustering/ccbc_context.o: src/MLC/Clustering/ccbc_context.cpp
	${MKDIR} -p ${OBJECTDIR}/src/MLC/Clustering
	${RM} "$@.d"
	$(COMPILE.cc) -g -Isrc/Eigen323 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/MLC/Clustering/ccbc_context.o src/MLC/Clustering/ccbc_context.cpp

${OBJECTDIR}/src/MLC/Clustering/cluster.o: src/MLC/Clustering/cluster.cpp
	${MKDIR} -p ${OBJECTDIR}/src/MLC/Clustering
	${RM} "$@.d"
	$(COMPILE.cc) -g -Isrc/Eigen323 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/MLC/Clustering/cluster.o src/MLC/Clustering/cluster.cpp

${OBJECTDIR}/src/MLC/Clustering/cluster_context.o: src/MLC/Clustering/cluster_context.cpp
	${MKDIR} -p ${OBJECTDIR}/src/MLC/Clustering
	${RM} "$@.d"
	$(COMPILE.cc) -g -Isrc/Eigen323 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/MLC/Clustering/cluster_context.o src/MLC/Clustering/cluster_context.cpp

${OBJECTDIR}/src/MLC/Clustering/clusterdb.o: src/MLC/Clustering/clusterdb.cpp
	${MKDIR} -p ${OBJECTDIR}/src/MLC/Clustering
	${RM} "$@.d"
	$(COMPILE.cc) -g -Isrc/Eigen323 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/MLC/Clustering/clusterdb.o src/MLC/Clustering/clusterdb.cpp

${OBJECTDIR}/src/MLC/Clustering/clustering.o: src/MLC/Clustering/clustering.cpp
	${MKDIR} -p ${OBJECTDIR}/src/MLC/Clustering
	${RM} "$@.d"
	$(COMPILE.cc) -g -Isrc/Eigen323 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/MLC/Clustering/clustering.o src/MLC/Clustering/clustering.cpp

${OBJECTDIR}/src/MLC/Clustering/clusterproperties.o: src/MLC/Clustering/clusterproperties.cpp
	${MKDIR} -p ${OBJECTDIR}/src/MLC/Clustering
	${RM} "$@.d"
	$(COMPILE.cc) -g -Isrc/Eigen323 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/MLC/Clustering/clusterproperties.o src/MLC/Clustering/clusterproperties.cpp

${OBJECTDIR}/src/MLC/Clustering/comparison.o: src/MLC/Clustering/comparison.cpp
	${MKDIR} -p ${OBJECTDIR}/src/MLC/Clustering
	${RM} "$@.d"
	$(COMPILE.cc) -g -Isrc/Eigen323 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/MLC/Clustering/comparison.o src/MLC/Clustering/comparison.cpp

${OBJECTDIR}/src/MLC/Clustering/double_matrix.o: src/MLC/Clustering/double_matrix.cpp
	${MKDIR} -p ${OBJECTDIR}/src/MLC/Clustering
	${RM} "$@.d"
	$(COMPILE.cc) -g -Isrc/Eigen323 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/MLC/Clustering/double_matrix.o src/MLC/Clustering/double_matrix.cpp

${OBJECTDIR}/src/MLC/Clustering/double_vector.o: src/MLC/Clustering/double_vector.cpp
	${MKDIR} -p ${OBJECTDIR}/src/MLC/Clustering
	${RM} "$@.d"
	$(COMPILE.cc) -g -Isrc/Eigen323 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/MLC/Clustering/double_vector.o src/MLC/Clustering/double_vector.cpp

${OBJECTDIR}/src/MLC/Clustering/eigenval.o: src/MLC/Clustering/eigenval.cpp
	${MKDIR} -p ${OBJECTDIR}/src/MLC/Clustering
	${RM} "$@.d"
	$(COMPILE.cc) -g -Isrc/Eigen323 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/MLC/Clustering/eigenval.o src/MLC/Clustering/eigenval.cpp

${OBJECTDIR}/src/MLC/Clustering/log.o: src/MLC/Clustering/log.cpp
	${MKDIR} -p ${OBJECTDIR}/src/MLC/Clustering
	${RM} "$@.d"
	$(COMPILE.cc) -g -Isrc/Eigen323 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/MLC/Clustering/log.o src/MLC/Clustering/log.cpp

${OBJECTDIR}/src/MLC/Clustering/matrix_context.o: src/MLC/Clustering/matrix_context.cpp
	${MKDIR} -p ${OBJECTDIR}/src/MLC/Clustering
	${RM} "$@.d"
	$(COMPILE.cc) -g -Isrc/Eigen323 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/MLC/Clustering/matrix_context.o src/MLC/Clustering/matrix_context.cpp

${OBJECTDIR}/src/MLC/Clustering/mlc_context.o: src/MLC/Clustering/mlc_context.cpp
	${MKDIR} -p ${OBJECTDIR}/src/MLC/Clustering
	${RM} "$@.d"
	$(COMPILE.cc) -g -Isrc/Eigen323 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/MLC/Clustering/mlc_context.o src/MLC/Clustering/mlc_context.cpp

${OBJECTDIR}/src/MLC/Clustering/nfieldbase.o: src/MLC/Clustering/nfieldbase.cpp
	${MKDIR} -p ${OBJECTDIR}/src/MLC/Clustering
	${RM} "$@.d"
	$(COMPILE.cc) -g -Isrc/Eigen323 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/MLC/Clustering/nfieldbase.o src/MLC/Clustering/nfieldbase.cpp

${OBJECTDIR}/src/MLC/Clustering/std_plugin.o: src/MLC/Clustering/std_plugin.cpp
	${MKDIR} -p ${OBJECTDIR}/src/MLC/Clustering
	${RM} "$@.d"
	$(COMPILE.cc) -g -Isrc/Eigen323 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/MLC/Clustering/std_plugin.o src/MLC/Clustering/std_plugin.cpp

${OBJECTDIR}/src/MLC/Clustering/task.o: src/MLC/Clustering/task.cpp
	${MKDIR} -p ${OBJECTDIR}/src/MLC/Clustering
	${RM} "$@.d"
	$(COMPILE.cc) -g -Isrc/Eigen323 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/MLC/Clustering/task.o src/MLC/Clustering/task.cpp

${OBJECTDIR}/src/MLC/Clustering/taskmap.o: src/MLC/Clustering/taskmap.cpp
	${MKDIR} -p ${OBJECTDIR}/src/MLC/Clustering
	${RM} "$@.d"
	$(COMPILE.cc) -g -Isrc/Eigen323 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/MLC/Clustering/taskmap.o src/MLC/Clustering/taskmap.cpp

${OBJECTDIR}/src/MLC/Clustering/toolbox.o: src/MLC/Clustering/toolbox.cpp
	${MKDIR} -p ${OBJECTDIR}/src/MLC/Clustering
	${RM} "$@.d"
	$(COMPILE.cc) -g -Isrc/Eigen323 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/MLC/Clustering/toolbox.o src/MLC/Clustering/toolbox.cpp

${OBJECTDIR}/src/MLC/Clustering/tostring.o: src/MLC/Clustering/tostring.cpp
	${MKDIR} -p ${OBJECTDIR}/src/MLC/Clustering
	${RM} "$@.d"
	$(COMPILE.cc) -g -Isrc/Eigen323 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/MLC/Clustering/tostring.o src/MLC/Clustering/tostring.cpp

${OBJECTDIR}/src/MLC/Clustering/uclust_context.o: src/MLC/Clustering/uclust_context.cpp
	${MKDIR} -p ${OBJECTDIR}/src/MLC/Clustering
	${RM} "$@.d"
	$(COMPILE.cc) -g -Isrc/Eigen323 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/MLC/Clustering/uclust_context.o src/MLC/Clustering/uclust_context.cpp

${OBJECTDIR}/src/MLC/Clustering/wait.o: src/MLC/Clustering/wait.cpp
	${MKDIR} -p ${OBJECTDIR}/src/MLC/Clustering
	${RM} "$@.d"
	$(COMPILE.cc) -g -Isrc/Eigen323 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/MLC/Clustering/wait.o src/MLC/Clustering/wait.cpp

${OBJECTDIR}/src/MLC/MfcCluster/consoleappentrypoint.o: src/MLC/MfcCluster/consoleappentrypoint.cpp
	${MKDIR} -p ${OBJECTDIR}/src/MLC/MfcCluster
	${RM} "$@.d"
	$(COMPILE.cc) -g -Isrc/Eigen323 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/MLC/MfcCluster/consoleappentrypoint.o src/MLC/MfcCluster/consoleappentrypoint.cpp

${OBJECTDIR}/src/MLC/MfcCluster/mfcclusterbusiness.o: src/MLC/MfcCluster/mfcclusterbusiness.cpp
	${MKDIR} -p ${OBJECTDIR}/src/MLC/MfcCluster
	${RM} "$@.d"
	$(COMPILE.cc) -g -Isrc/Eigen323 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/MLC/MfcCluster/mfcclusterbusiness.o src/MLC/MfcCluster/mfcclusterbusiness.cpp

${OBJECTDIR}/src/MLC/MfcCluster/optimizer.o: src/MLC/MfcCluster/optimizer.cpp
	${MKDIR} -p ${OBJECTDIR}/src/MLC/MfcCluster
	${RM} "$@.d"
	$(COMPILE.cc) -g -Isrc/Eigen323 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/MLC/MfcCluster/optimizer.o src/MLC/MfcCluster/optimizer.cpp

${OBJECTDIR}/src/MLC/MfcCluster/testfieldbase.o: src/MLC/MfcCluster/testfieldbase.cpp
	${MKDIR} -p ${OBJECTDIR}/src/MLC/MfcCluster
	${RM} "$@.d"
	$(COMPILE.cc) -g -Isrc/Eigen323 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/MLC/MfcCluster/testfieldbase.o src/MLC/MfcCluster/testfieldbase.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
