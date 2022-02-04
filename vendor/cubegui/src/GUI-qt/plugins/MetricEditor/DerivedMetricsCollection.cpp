/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2020                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include "config.h"
#include "DerivedMetricsCollection.h"
#include "MetricData.h"

#define EMPTY_METRIC "metric type: \n" \
    "display name: \n" \
    "unique name: \n" \
    "uom: \n" \
    "description: \n" \
    "cubepl expression: \n"


#define KENOBI_METRIC "metric type: postderived\n" \
    "display name: Average visit time\n" \
    "unique name: avg_visit_time\n" \
    "uom: sec\n" \
    "description: Calculates average time of region execution per visit.\n" \
    "Autor is Michael Knobloch.\n" \
    "cubepl expression: metric::time(i)/metric::visits(e)\n"

#define REGION_SELECTION_METRIC "metric type: postderived\n" \
    "display name: Region __EDIT__\n" \
    "unique name: region_name_selection\n" \
    "uom: occ\n" \
    "description: Returns 1 in for the reg.\n" \
    "Autor is Pavel Saviankou.\n" \
    "cubepl expression: { \n" \
    " ${a}=0;\n" \
    "  if ( ${cube::region::name}[${calculation::region::id}] =~  /^__EDIT__/ ) \n" \
    "     { ${a}=1; }; return ${a}; \n" \
    " }\n"

#define LEAVES_SELECTION_METRIC "metric type: prederived_exclusive\n" \
    "display name: Leaves selection\n" \
    "unique name: leaves_selection\n" \
    "uom: occ\n" \
    "description: Returns 1 for the callpath is a leaf.\n" \
    "Autor is Pavel Saviankou.\n" \
    "cubepl expression: { \n" \
    " ${a}=0;\n" \
    "  if (${cube::callpath::#children}[${calculation::callpath::id}] == 0 )\n" \
    "     { ${a}=1; }; return ${a}; \n" \
    " }\n"

#define ROOT_SELECTION_METRIC "metric type: prederived_exclusive\n" \
    "display name: Root selection\n" \
    "unique name: root_selection\n" \
    "uom: occ\n" \
    "description: Returns 1 for the root callpath.\n" \
    "Autor is Pavel Saviankou.\n" \
    "cubepl expression: { \n" \
    " ${a}=0;\n" \
    " if (${cube::callpath::parent::id}[${calculation::callpath::id}] == -1 )\n" \
    "     { ${a}=1; }; return ${a}; \n" \
    " }\n"


#define N_LEVEL_SELECTION_METRIC "metric type: prederived_exclusive\n" \
    "display name: Selection Level 1\n" \
    "unique name: selection_level_1\n" \
    "uom: occ\n" \
    "description: Returns 1 for the callpath of level 1.\n" \
    "To change the level of callpath one modifies variable \"level\".\n" \
    "Autor is Pavel Saviankou.\n" \
    "cubepl expression: { \n" \
    "${level}=1;\n" \
    "${index}=0;\n" \
    "${i}=${calculation::callpath::id};\n\n" \
    "while (${cube::callpath::parent::id}[${i}] != -1 )\n" \
    "{ \n" \
    "  ${i}= ${cube::callpath::parent::id}[${i}]; \n" \
    "  ${index}=${index}+1; \n" \
    "}; \n" \
    "  ${a}=0;\n" \
    "  if (${index} == ${level})\n" \
    "  {  ${a}=1; };\n" \
    "return ${a}; \n" \
    "}  "


#define RECURSION_LEVEL_METRIC "metric type: prederived_exclusive\n" \
    "display name: Recursion Level \n" \
    "unique name: recursion_level\n" \
    "uom: occ\n" \
    "description: Returns level of the recursion for the recursive callpath.\n" \
    "Autor is Pavel Saviankou.\n" \
    "cubepl expression: { \n" \
    "return ${recursion}[${calculation::callpath::id} ]; \n" \
    "} \n "  \
    "cubepl init expression: { \n" \
    "global(recursion);\n" \
    "${i}=0;\n" \
    "while( ${i}  < ${cube::#callpaths} )\n" \
    "{\n" \
    "${id}=${i};\n" \
    "${to_return}=0;\n" \
    "${region_i} = ${cube::callpath::calleeid}[${i}] ;\n" \
    "${recursion}[${i}]= 0;\n" \
    "while( ${cube::callpath::parent::id}[${id}] != -1  )\n" \
    "{\n" \
    "${region_id} = ${cube::callpath::calleeid}[${id}] ;\n" \
    "if (${cube::region::name}[${region_i} ] eq ${cube::region::name}[${region_id}] )\n" \
    "{\n" \
    "${to_return} = ${to_return}+1; \n" \
    "};\n" \
    "\n" \
    "${id}= ${cube::callpath::parent::id}[${id}];\n" \
    "};\n" \
    "\n" \
    "${recursion}[${i}] =  max(${to_return}-1, 0)/${cube::#locations};\n" \
    "${i} = ${i} + 1;\n" \
    "};\n" \
    "return 0;\n" \
    "}  "


#define N_RECURSIONS_METRIC "metric type: prederived_exclusive\n" \
    "display name: # Recursions \n" \
    "unique name: number_of_recursions\n" \
    "uom: occ\n" \
    "description: Returns number of recursion calls.\n" \
    "Autor is Pavel Saviankou.\n" \
    "cubepl expression: { \n" \
    "return ${recursion2}[${calculation::callpath::id} ]; \n" \
    "} \n "  \
    "cubepl init expression: { \n" \
    "global(recursion2);\n" \
    "${i}=0;\n" \
    "while( ${i}  < ${cube::#callpaths} )\n" \
    "{\n" \
    " ${id}=${cube::callpath::parent::id}[${i}];\n" \
    "${to_return}=0;\n" \
    "${region_i} = ${cube::callpath::calleeid}[${i}] ;\n" \
    "${recursion2}[${i}]= 0;\n" \
    "while( ${id} != -1   )\n" \
    "{\n" \
    "${region_id} = ${cube::callpath::calleeid}[${id}] ;\n" \
    "if (${cube::region::name}[${region_i} ] eq ${cube::region::name}[${region_id}] )\n" \
    "{\n" \
    "${to_return} = 1; \n" \
    "};\n" \
    "\n" \
    "${id}= ${cube::callpath::parent::id}[${id}];\n" \
    "};\n" \
    "\n" \
    "${recursion2}[${i}] =  ${to_return}/${cube::#locations};\n" \
    "${i} = ${i} + 1;\n" \
    "};\n" \
    "return 0;\n" \
    "}  "


#define MAX_GROUP_TIME_METRIC "metric type: prederived_exclusive\n" \
    "display name: Maximal location group run time\n" \
    "unique name: maxgrouptime\n" \
    "uom: sec\n" \
    "url: \n" \
    "description: Returns maximal exclusive time within call tree. Author: Pavel Saviankou. \n" \
    "cubepl expression: metric::time()\n" \
    "cubepl init expression: \n" \
    "cubepl plus expression: max(arg1,arg2)\n"

#define PERFECT_PARALLELIZATION_TIME_METRIC "metric type: prederived_exclusive\n" \
    "display name: Perfect run time\n" \
    "unique name: perftime\n" \
    "uom: sec\n" \
    "url: \n" \
    "description: Presents run time of the application as every region would be perfectly parallelized. Requires defined metric \"Maximal location group run time\" from this collection. Author: Pavel Saviankou.\n" \
    "cubepl expression: metric::call::maxgrouptime( ${calculation::callpath::id}, e, ${cube::location::parent::sysid}[${calculation::sysres::id}], i   )\n" \
    "cubepl init expression: \n" \
    "cubepl plus expression:"

#define PARALLELIZATION_LEVEL_METRIC "metric type: postderived\n" \
    "display name: Parallelization\n" \
    "unique name: parallelization\n" \
    "uom: %\n" \
    "url: \n" \
    "description: Presents level of parallelization of the call path as a ration actuall run time to the ren time of a perfectly parallelized callpath. Requires defined metric \"Perfect run time\" form this collection. Author: Pavel Saviankou.\n" \
    "cubepl expression: metric::time()/metric::perftime()\n" \
    "cubepl init expression: "


using namespace metric_editor;

QString&
DerivedMetricsCollection::getDerivedMetricText( int key )
{
    if ( key >= DM_SIZE )
    {
        return derived_metrics[ DM_NO_METRIC ];
    }
    return derived_metrics[ key ];
}



QString DerivedMetricsCollection::derived_metrics[ DM_SIZE ] = {
    QString( EMPTY_METRIC ),
    QString( KENOBI_METRIC ),
    QString( REGION_SELECTION_METRIC ),
    QString( LEAVES_SELECTION_METRIC ),
    QString( ROOT_SELECTION_METRIC ),
    QString( N_LEVEL_SELECTION_METRIC ),
    QString( RECURSION_LEVEL_METRIC ),
    QString( N_RECURSIONS_METRIC ),
    QString( MAX_GROUP_TIME_METRIC ),
    QString( PERFECT_PARALLELIZATION_TIME_METRIC ),
    QString( PARALLELIZATION_LEVEL_METRIC )
};

QStringList
DerivedMetricsCollection::getDerivedMetricsNames()
{
    QStringList _to_return;
    MetricData  data;
    for ( unsigned i = 0; i < DM_SIZE; i++ )
    {
        data.setCubePL( derived_metrics[ i ] );
        _to_return << data.getDisp_name();
    }

    return _to_return;
}
