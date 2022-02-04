##*************************************************************************##
##  CUBE        http://www.scalasca.org/                                   ##
##*************************************************************************##
##  Copyright (c) 1998-2021                                                ##
##  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          ##
##                                                                         ##
##  Copyright (c) 2009-2015                                                ##
##  German Research School for Simulation Sciences GmbH,                   ##
##  Laboratory for Parallel Programming                                    ##
##                                                                         ##
##  This software may be modified and distributed under the terms of       ##
##  a BSD-style license.  See the COPYING file in the package base         ##
##  directory for details.                                                 ##
##*************************************************************************##


CONFIG	+= qt thread rtti exceptions warn_on  release plugin
TEMPLATE = lib
CUBE +=
CUBE_SRC = C:/Scalasca-Soft/Install/Cube2.0/$$(PREFIX)/install
GUI_SRC = ../../../src/GUI-qt
GUI_CONFIG = ../../../vpath/src
DEPENDPATH += $${GUI_SRC} $${GUI_SRC}/display
INCLUDEPATH +=  \
    	../include \
	$${CUBE_SRC}/include/cubelib \
	$${GUI_CONFIG}  \
	$${GUI_SRC} \
	$${GUI_SRC}/display   \
	$${GUI_SRC}/display/plugins \
	$${GUI_SRC}/display/utils   \
	$${GUI_SRC}/plugins/Advisor  \
	$${GUI_SRC}/plugins/Advisor/analyse/jsc-hybrid-audit/tests \
	$${GUI_SRC}/plugins/Advisor/analyse/jsc-hybrid-audit\
	$${GUI_SRC}/plugins/Advisor/analyse/knl/tests \
	$${GUI_SRC}/plugins/Advisor/analyse/knl\
	$${GUI_SRC}/plugins/Advisor/analyse/pop-audit/tests \
	$${GUI_SRC}/plugins/Advisor/analyse/pop-audit\
	$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-additive/tests \
	$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-additive\
	$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-bs/tests \
	$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-bs\
	$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit/tests \
	$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit\
	$${GUI_SRC}/plugins/Advisor/analyse\
	$${GUI_SRC}/plugins/Advisor/tests  	
	 
LIBS += -L$${CUBE_SRC}/lib -lcube  -L../GUI-qt-core/release -lcube4gui 

DEFINES+= CUBE_PREFIX=\\\"unknown\\\" 
DEFINES+= LIBDIR=\\\".\\\"


QMAKE_CXXFLAGS=-fexceptions

QT += widgets network printsupport concurrent 

# Input{
HEADERS += \
$${GUI_SRC}/plugins/Advisor/Bar.h \
$${GUI_SRC}/plugins/Advisor/CubeAdvice.h \
$${GUI_SRC}/plugins/Advisor/CubeAdvisorPlugin.h \
$${GUI_SRC}/plugins/Advisor/CubeAdvisorProgress.h \
$${GUI_SRC}/plugins/Advisor/CubeAdvisorTableWidgetItem.h \
$${GUI_SRC}/plugins/Advisor/CubeAdvisorToolBar.h \
$${GUI_SRC}/plugins/Advisor/CubeHelpButton.h\
$${GUI_SRC}/plugins/Advisor/CubeRatingWidget.h\
$${GUI_SRC}/plugins/Advisor/CubeTestWidget.h\
$${GUI_SRC}/plugins/Advisor/analyse/PerformanceAnalysis.h\
$${GUI_SRC}/plugins/Advisor/analyse/jsc-hybrid-audit/JSCHybridAuditAnalysis.h \
$${GUI_SRC}/plugins/Advisor/analyse/jsc-hybrid-audit/tests/JSCHybridAmdahlTest.h \
$${GUI_SRC}/plugins/Advisor/analyse/jsc-hybrid-audit/tests/JSCHybridCommunicationEfficiencyTest.h \
$${GUI_SRC}/plugins/Advisor/analyse/jsc-hybrid-audit/tests/JSCHybridComputationTimeTest.h \
$${GUI_SRC}/plugins/Advisor/analyse/jsc-hybrid-audit/tests/JSCHybridIPCTest.h \
$${GUI_SRC}/plugins/Advisor/analyse/jsc-hybrid-audit/tests/JSCHybridImbalanceTest.h \
$${GUI_SRC}/plugins/Advisor/analyse/jsc-hybrid-audit/tests/JSCHybridNoWaitINSTest.h \
$${GUI_SRC}/plugins/Advisor/analyse/jsc-hybrid-audit/tests/JSCHybridOMPImbalanceTest.h \
$${GUI_SRC}/plugins/Advisor/analyse/jsc-hybrid-audit/tests/JSCHybridOMPSerialisationTest.h \
$${GUI_SRC}/plugins/Advisor/analyse/jsc-hybrid-audit/tests/JSCHybridOMPTransferTest.h \
$${GUI_SRC}/plugins/Advisor/analyse/jsc-hybrid-audit/tests/JSCHybridSerialisationTest.h \
$${GUI_SRC}/plugins/Advisor/analyse/jsc-hybrid-audit/tests/JSCHybridStalledResourcesTest.h \
$${GUI_SRC}/plugins/Advisor/analyse/jsc-hybrid-audit/tests/JSCHybridTransferTest.h \
$${GUI_SRC}/plugins/Advisor/analyse/knl/KnlMemoryAnalysis.h\
$${GUI_SRC}/plugins/Advisor/analyse/knl/KnlVectorizationAnalysis.h\
$${GUI_SRC}/plugins/Advisor/analyse/knl/tests/KnlLLCMissTest.h\
$${GUI_SRC}/plugins/Advisor/analyse/knl/tests/KnlMemoryBandwidthTest.h\
$${GUI_SRC}/plugins/Advisor/analyse/knl/tests/KnlMemoryTransferTest.h\
$${GUI_SRC}/plugins/Advisor/analyse/knl/tests/L1CompToDataTest.h\
$${GUI_SRC}/plugins/Advisor/analyse/knl/tests/L2CompToDataTest.h\
$${GUI_SRC}/plugins/Advisor/analyse/knl/tests/VPUIntensityTest.h\
$${GUI_SRC}/plugins/Advisor/analyse/pop-audit/POPAuditAnalysis.h\
$${GUI_SRC}/plugins/Advisor/analyse/pop-audit/tests/POPCommunicationEfficiencyTest.h  \
$${GUI_SRC}/plugins/Advisor/analyse/pop-audit/tests/POPComputationTimeTest.h  \
$${GUI_SRC}/plugins/Advisor/analyse/pop-audit/tests/POPIPCTest.h  \
$${GUI_SRC}/plugins/Advisor/analyse/pop-audit/tests/POPImbalanceTest.h  \
$${GUI_SRC}/plugins/Advisor/analyse/pop-audit/tests/POPNoWaitINSTest.h  \
$${GUI_SRC}/plugins/Advisor/analyse/pop-audit/tests/POPParallelEfficiencyTest.h  \
$${GUI_SRC}/plugins/Advisor/analyse/pop-audit/tests/POPSerialisationTest.h  \
$${GUI_SRC}/plugins/Advisor/analyse/pop-audit/tests/POPStalledResourcesTest.h  \
$${GUI_SRC}/plugins/Advisor/analyse/pop-audit/tests/POPTransferTest.h  \
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-additive/POPHybridAuditAnalysisAdd.h\
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-additive/tests/POPHybridAmdahlTestAdd.h  \
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-additive/tests/POPHybridCommunicationEfficiencyTestAdd.h  \
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-additive/tests/POPHybridComputationTimeTestAdd.h  \
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-additive/tests/POPHybridIPCTestAdd.h  \
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-additive/tests/POPHybridImbalanceTestAdd.h  \
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-additive/tests/POPHybridNoWaitINSTestAdd.h  \
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-additive/tests/POPHybridOMPRegionEfficiencyTestAdd.h  \
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-additive/tests/POPHybridParallelEfficiencyTestAdd.h  \
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-additive/tests/POPHybridProcessEfficiencyTestAdd.h  \
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-additive/tests/POPHybridSerialisationTestAdd.h  \
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-additive/tests/POPHybridStalledResourcesTestAdd.h  \
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-additive/tests/POPHybridThreadEfficiencyTestAdd.h  \
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-additive/tests/POPHybridTransferTestAdd.h  \
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-bs/BSPOPHybridAuditAnalysis.h\
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-bs/tests/BSPOPHybridCommunicationEfficiencyTest.h \
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-bs/tests/BSPOPHybridComputationTimeTest.h \
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-bs/tests/BSPOPHybridIPCTest.h \
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-bs/tests/BSPOPHybridLoadBalanceTest.h \
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-bs/tests/BSPOPHybridMPICommunicationEfficiencyTest.h \
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-bs/tests/BSPOPHybridMPILoadBalanceTest.h \
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-bs/tests/BSPOPHybridMPIParallelEfficiencyTest.h \
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-bs/tests/BSPOPHybridMPISerialisationTest.h \
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-bs/tests/BSPOPHybridMPITransferTest.h \
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-bs/tests/BSPOPHybridNoWaitINSTest.h \
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-bs/tests/BSPOPHybridOMPCommunicationEfficiencyTest.h \
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-bs/tests/BSPOPHybridOMPLoadBalanceEfficiencyTest.h \
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-bs/tests/BSPOPHybridOMPParallelEfficiencyTest.h \
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-bs/tests/BSPOPHybridParallelEfficiencyTest.h \
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-bs/tests/BSPOPHybridStalledResourcesTest.h \
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit/POPHybridAuditAnalysis.h\
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit/tests/POPHybridAmdahlTest.h  \
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit/tests/POPHybridCommunicationEfficiencyTest.h  \
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit/tests/POPHybridComputationTimeTest.h  \
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit/tests/POPHybridIPCTest.h  \
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit/tests/POPHybridImbalanceTest.h  \
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit/tests/POPHybridNoWaitINSTest.h  \
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit/tests/POPHybridOMPRegionEfficiencyTest.h  \
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit/tests/POPHybridParallelEfficiencyTest.h  \
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit/tests/POPHybridProcessEfficiencyTest.h  \
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit/tests/POPHybridSerialisationTest.h  \
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit/tests/POPHybridStalledResourcesTest.h  \
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit/tests/POPHybridThreadEfficiencyTest.h  \
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit/tests/POPHybridTransferTest.h  \
$${GUI_SRC}/plugins/Advisor/tests/PerformanceTest.h\
$${GUI_SRC}/plugins/Advisor/tests/PerformanceTest_incl.h





SOURCES += \
$${GUI_SRC}/plugins/Advisor/Bar.cpp \
$${GUI_SRC}/plugins/Advisor/CubeAdvisorPlugin.cpp \
$${GUI_SRC}/plugins/Advisor/CubeAdvisorProgress.cpp \
$${GUI_SRC}/plugins/Advisor/CubeAdvisorTableWidgetItem.cpp \
$${GUI_SRC}/plugins/Advisor/CubeAdvisorToolBar.cpp \
$${GUI_SRC}/plugins/Advisor/CubeHelpButton.cpp \
$${GUI_SRC}/plugins/Advisor/CubeRatingWidget.cpp \
$${GUI_SRC}/plugins/Advisor/CubeTestWidget.cpp \
$${GUI_SRC}/plugins/Advisor/analyse/PerformanceAnalysis.cpp\
$${GUI_SRC}/plugins/Advisor/analyse/jsc-hybrid-audit/JSCHybridAuditAnalysis.cpp \
$${GUI_SRC}/plugins/Advisor/analyse/jsc-hybrid-audit/tests/JSCHybridAmdahlTest.cpp \
$${GUI_SRC}/plugins/Advisor/analyse/jsc-hybrid-audit/tests/JSCHybridCommunicationEfficiencyTest.cpp \
$${GUI_SRC}/plugins/Advisor/analyse/jsc-hybrid-audit/tests/JSCHybridComputationTimeTest.cpp \
$${GUI_SRC}/plugins/Advisor/analyse/jsc-hybrid-audit/tests/JSCHybridIPCTest.cpp \
$${GUI_SRC}/plugins/Advisor/analyse/jsc-hybrid-audit/tests/JSCHybridImbalanceTest.cpp \
$${GUI_SRC}/plugins/Advisor/analyse/jsc-hybrid-audit/tests/JSCHybridNoWaitINSTest.cpp \
$${GUI_SRC}/plugins/Advisor/analyse/jsc-hybrid-audit/tests/JSCHybridOMPImbalanceTest.cpp \
$${GUI_SRC}/plugins/Advisor/analyse/jsc-hybrid-audit/tests/JSCHybridOMPSerialisationTest.cpp \
$${GUI_SRC}/plugins/Advisor/analyse/jsc-hybrid-audit/tests/JSCHybridOMPTransferTest.cpp \
$${GUI_SRC}/plugins/Advisor/analyse/jsc-hybrid-audit/tests/JSCHybridSerialisationTest.cpp \
$${GUI_SRC}/plugins/Advisor/analyse/jsc-hybrid-audit/tests/JSCHybridStalledResourcesTest.cpp \
$${GUI_SRC}/plugins/Advisor/analyse/jsc-hybrid-audit/tests/JSCHybridTransferTest.cpp \
$${GUI_SRC}/plugins/Advisor/analyse/knl/KnlMemoryAnalysis.cpp\
$${GUI_SRC}/plugins/Advisor/analyse/knl/KnlVectorizationAnalysis.cpp\
$${GUI_SRC}/plugins/Advisor/analyse/knl/tests/KnlLLCMissTest.cpp\
$${GUI_SRC}/plugins/Advisor/analyse/knl/tests/KnlMemoryBandwidthTest.cpp\
$${GUI_SRC}/plugins/Advisor/analyse/knl/tests/KnlMemoryTransferTest.cpp\
$${GUI_SRC}/plugins/Advisor/analyse/knl/tests/L1CompToDataTest.cpp\
$${GUI_SRC}/plugins/Advisor/analyse/knl/tests/L2CompToDataTest.cpp \
$${GUI_SRC}/plugins/Advisor/analyse/knl/tests/VPUIntensityTest.cpp\
$${GUI_SRC}/plugins/Advisor/analyse/pop-audit/POPAuditAnalysis.cpp\
$${GUI_SRC}/plugins/Advisor/analyse/pop-audit/tests/POPCommunicationEfficiencyTest.cpp\
$${GUI_SRC}/plugins/Advisor/analyse/pop-audit/tests/POPComputationTimeTest.cpp\
$${GUI_SRC}/plugins/Advisor/analyse/pop-audit/tests/POPIPCTest.cpp\
$${GUI_SRC}/plugins/Advisor/analyse/pop-audit/tests/POPImbalanceTest.cpp\
$${GUI_SRC}/plugins/Advisor/analyse/pop-audit/tests/POPNoWaitINSTest.cpp\
$${GUI_SRC}/plugins/Advisor/analyse/pop-audit/tests/POPParallelEfficiencyTest.cpp\
$${GUI_SRC}/plugins/Advisor/analyse/pop-audit/tests/POPSerialisationTest.cpp\
$${GUI_SRC}/plugins/Advisor/analyse/pop-audit/tests/POPStalledResourcesTest.cpp\
$${GUI_SRC}/plugins/Advisor/analyse/pop-audit/tests/POPTransferTest.cpp\
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-additive/POPHybridAuditAnalysisAdd.cpp\
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-additive/tests/POPHybridAmdahlTestAdd.cpp\
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-additive/tests/POPHybridCommunicationEfficiencyTestAdd.cpp\
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-additive/tests/POPHybridComputationTimeTestAdd.cpp\
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-additive/tests/POPHybridIPCTestAdd.cpp\
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-additive/tests/POPHybridImbalanceTestAdd.cpp\
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-additive/tests/POPHybridNoWaitINSTestAdd.cpp\
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-additive/tests/POPHybridOMPRegionEfficiencyTestAdd.cpp\
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-additive/tests/POPHybridParallelEfficiencyTestAdd.cpp\
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-additive/tests/POPHybridProcessEfficiencyTestAdd.cpp\
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-additive/tests/POPHybridSerialisationTestAdd.cpp\
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-additive/tests/POPHybridStalledResourcesTestAdd.cpp\
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-additive/tests/POPHybridThreadEfficiencyTestAdd.cpp\
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-additive/tests/POPHybridTransferTestAdd.cpp\
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-bs/BSPOPHybridAuditAnalysis.cpp\
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-bs/tests/BSPOPHybridCommunicationEfficiencyTest.cpp \
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-bs/tests/BSPOPHybridComputationTimeTest.cpp \
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-bs/tests/BSPOPHybridIPCTest.cpp \
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-bs/tests/BSPOPHybridLoadBalanceTest.cpp \
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-bs/tests/BSPOPHybridMPICommunicationEfficiencyTest.cpp \
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-bs/tests/BSPOPHybridMPILoadBalanceTest.cpp \
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-bs/tests/BSPOPHybridMPIParallelEfficiencyTest.cpp \
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-bs/tests/BSPOPHybridMPISerialisationTest.cpp \
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-bs/tests/BSPOPHybridMPITransferTest.cpp \
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-bs/tests/BSPOPHybridNoWaitINSTest.cpp \
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-bs/tests/BSPOPHybridOMPCommunicationEfficiencyTest.cpp \
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-bs/tests/BSPOPHybridOMPLoadBalanceEfficiencyTest.cpp \
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-bs/tests/BSPOPHybridOMPParallelEfficiencyTest.cpp \
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-bs/tests/BSPOPHybridParallelEfficiencyTest.cpp \
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit-bs/tests/BSPOPHybridStalledResourcesTest.cpp \
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit/POPHybridAuditAnalysis.cpp\
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit/tests/POPHybridAmdahlTest.cpp\
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit/tests/POPHybridCommunicationEfficiencyTest.cpp\
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit/tests/POPHybridComputationTimeTest.cpp\
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit/tests/POPHybridIPCTest.cpp\
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit/tests/POPHybridImbalanceTest.cpp\
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit/tests/POPHybridNoWaitINSTest.cpp\
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit/tests/POPHybridOMPRegionEfficiencyTest.cpp\
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit/tests/POPHybridParallelEfficiencyTest.cpp\
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit/tests/POPHybridProcessEfficiencyTest.cpp\
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit/tests/POPHybridSerialisationTest.cpp\
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit/tests/POPHybridStalledResourcesTest.cpp\
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit/tests/POPHybridThreadEfficiencyTest.cpp\
$${GUI_SRC}/plugins/Advisor/analyse/pop-hybrid-audit/tests/POPHybridTransferTest.cpp\
$${GUI_SRC}/plugins/Advisor/tests/PerformanceTest.cpp\
$${GUI_SRC}/plugins/Advisor/tests/PerformanceTest_incl.cpp 


# Hack for IBM AIX
aix-g++ {
  QMAKE_CXXFLAGS_RELEASE += -O0
}
aix-g++-64 {
  QMAKE_CXXFLAGS_RELEASE += -O0
}
