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


#ifndef _HTML_HELP_COLLECTION_CPP
#define _HTML_HELP_COLLECTION_CPP
#include <QObject>

#include "HtmlHelpCollection.h"

using namespace cubegui;

QString&
HelpCollection::getHelpText( CUBEPL_HELP_KEY key )
{
    if ( key >= NUM_CUBEPL_HELP_TEXTES )
    {
        return textes[ NO_HELP ];
    }
    return textes[ key ];
}



QString HelpCollection::textes[ NUM_CUBEPL_HELP_TEXTES ] = {
    QObject::tr( "no help available" ),
    QObject::tr( "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">"
                 "<html>"
                 "<head>"
                 "<title>Syntax of CubePL</title>"
                 "</head>"
                 "<body>"
                 "Derived metric calculates a value F(m1, m2, ${xyz},...), where <br>"
                 "F - is an arithmetical expression. <br>"
                 "m1,m2,... - is a reference to existing metric in loaded cube<br>"
                 "${xyz} - a value of the variable \"xyz\".<br>"
                 "<br>"
                 "<u>Supported operations</u>:<br>"
                 "+, /, - , *, ^, |?| and (?)<br>"
                 "<br>"
                 "<u>Supported functions</u>:<br>"
                 "sin,cos,tan,asin,acos,atan,sqrt,log,exp,random,abs,pos,neg,sgn,min(a,b), max(a,b) <br>"
                 "<br>"
                 "<u>Supported inplace function definition</u>:<br>"
                 "{ [statements] return [expression];} "
                 "<br>"
                 "As <u>[statement]</u> are supported:<br>"
                 "1. if ([condition]) { [statements] }  or <br>"
                 "2. if ([condition]) { [stetements] } else { [statements]} - define a conditional execution <br>"
                 "3. while (condition) {[statements] } - defines a loop, which is executed as long the conditioin is true. (max 1000000000 times) <br>"
                 "4. ${xyz} = expression - assignment of the value of an expression to the variable  \"xyz\"<br>"
                 "<u>[expression]</u>  - is either an arithmetical expression or a value of a variable ${xyz}."
                 "<br>"
                 "As <u>condition</u> supported: <br>"
                 "boolean operations with \"and\", \"or\", \"not\", \"xor\", \"&gt;\", \"&lt;\", \"&gt;=\", \"&lt;=\", \"==\" , \"!=\" <br><br>"
                 "<u>Metric reference syntax</u>:<br>"
                 "<b>metric::[unique name of metric X]()</b>br"
                 " takes the value of metric \"X\", calculated with the same arguments, with which the derivated metric is going to be calculated <br>"
                 "<br>"
                 "<b>metric::[unique name of metric X](x)</b><br>"
                 " takes the value of metric \"X\", calculated with the same arguments, with which the derivated metric is going to be calculated and  x denotes a calculation modification along calltree;<br>"
                 "<br>"
                 "<b>metric::[unique name of metric X](x,y)</b><br>"
                 " takes the value of metric \"X\", calculated with the same arguments, with which the derivated metric is going to be calculated, x denotes a calculation modification along calltree and y - along system tree<br>"
                 "<br>"
                 "<u>Calculation modificators</u>:<br>"
                 " \"*\" -doesn't modify calculation<br>"
                 " \"i\" - enforces inclusive calculation and <br>"
                 " \"e\" - enforces calculation of an exclusive value.<br>"
                 "<br>"
                 "<u>Examples</u>:<br>"
                 "<br>"
                 "<b>metric::time(i)</b><br>"
                 "has always (independen, if callpatgh is expanded or collapsed) an inclusive value of metric \"time\"<br><br>"
                 "<br>"
                 "<b>metric::time(i)/metric::visits(*)</b><br>"
                 "calculates inclusive time per visits<br><br>"
                 "<br>"
                 "<b>metric::numberofbytes()/metric::numberofallocations(*)</b>"
                 " calculates average value of a memory allocation<br>"
                 "<br>"
                 "<b>sin(23.4)*23^4-sqrt(3)/34</b><br>"
                 " shows a constant value<br>"
                 "<br>"
                 "<b>{ ${a}=0; while (${a} &lt; 100) { ${b}=${b}+metric::visits(); ${a}=${a}+1;}; return ${b}; }</b><br>"
                 " shows a 100 times a metric \"visits\" value, calculated using inplace function definition.<br>"
                 "<br>"
                 "<br><br>"
                 "Unique name of a metric you can get in the item \"Full info\" in the metric context menu"
                 "</body>"
                 "</html>"
                 ),
    QObject::tr( "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">"
                 "<html>"
                 "<head>"
                 "<title>Derived metrics in Cube</title>"
                 "</head>"
                 "<body>"
                 "Cube supports three kinds of derived metrics:<br>"
                 "<br>"
                 "<u>Prederived inclusive</u>, <u>Prederived exclusive</u> and <u>Postderived</u><br>"
                 "<br>"
                 "Difference between those metrics is in the place related to the aggregation, where the derivation appears.<br>"
                 "<br>"
                 "- <u>Prederived metric</u> - derivation (calculation of the arithmetical expression) is done <b>before</b> of the aggregation in GUI<br><br>"
                 "- <u>Postderived metric</u> - derivation (calculation of the arithmetical expression) is done <b>after</b> of the aggregation in GUI<br>"
                 "<br><br>"
                 "<u>Example 1</u>:<br>"
                 "<br>"
                 "Given are metrics \"Number of cache misses\" and \"Time\".<br>"
                 "<br>"
                 "One would like to calculate \"Rate of the cache misses\"<br>"
                 "The calculation expression is : <br>"
                 "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;    (CM1 + CM2 + ...)/(Time1 + Time2 + ...), <br>"
                 "where: <br>"
                 "CM1, CM, ... - number of cache misses in the callpath 1, callpath 2 and so on<br>"
                 "Time1, Time2, ... - time spend in the callpath 1, callpath 2 and so on<br>"
                 "<br>"
                 "Result is the value of the cache misses rate for the aggregated callpath (over system tree, or sub call path, or how else)<br>"
                 "From the expression is easy to see, that the evaluation of the expression happens after the aggregation <br>"
                 " -> Therefore one uses POSTDERIVED metric with the expression <br>"
                 "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;   <b>metric::cm()/metric::time()</b><br>"
                 "<br><br>"
                 "<u>Example 2</u>:<br>"
                 "<br>"
                 "Given is metric \"Time\".<br>"
                 "<br>"
                 "One would like to calculate time spend in MPI Regions <br>"
                 "The calculation expression is : <br>"
                 "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;{ <br>"
                 "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;if (${calculation::region::name} =~ /^MPI_/) <br>"
                 "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;{ ${time}=metric::time(); } <br>"
                 "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;else <br>"
                 "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;{ ${time}=0; } ; <br>"
                 "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;return ${time};<br>"
                 "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;} <br>"
                 "where: <br>"
                 "${calculation::region::name} -automatic created variable, storing the name of he currently calculation region. <br>"
                 "For the list of the automatic variables please see Cube User Guide.<br>"
                 "<br>"
                 "As aggregated value one would like to have normal summation over system tree, or sub call path.<br>"
                 "<br>"
                 "From that it is easy to see, that the evaluation of the expression should  happen before the aggregation <br>"
                 " -> Therefore one uses PREDERIVED metric.<br>"
                 "<br>"
                 "Value of prederived metrics can be observed being inclusive or exclusive related to the call path.<br>"
                 ""
                 "</body>"
                 "</html>"
                 ),
    QString(
        ""
        ),
    QObject::tr( "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">"
                 "<html>"
                 "<head>"
                 "<title>Derived Metric Execution</title>"
                 "</head>"
                 "<body>"
                 "By every calculation of a value of the derived metric it executes formulated in CubePL expression. "
                 "<br>"
                 "<br>"
                 "Depending on the context of the execution (without aggregation, with the aggregation over system tree, etc.) "
                 "<br>"
                 "CubePL engine initialize different standard variables within CubePL memory. "
                 "<br>"
                 "<br>"
                 "For further information please look <a href=\"http://www.scalasca.org/software/cube-4.x/documentation.html\"> Cube Derived Metrics </a> help."
                 "</body>"
                 "</html>"
                 ),
    QObject::tr( "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">"
                 "<html>"
                 "<head>"
                 "<title>Derived Metric Initialization</title>"
                 "</head>"
                 "<body>"
                 "Once derived metric is created, it executed initialization expression only once. "
                 "<br>"
                 "<br>"
                 "It is always without any context ( no call path, no system tree parameters)."
                 "<br>"
                 "One has to use  this  part of the calculation to perform context-free part of "
                 "<br>"
                 "the calculation."
                 "<br>"
                 "<br>"
                 "For further information please look <a href=\"http://www.scalasca.org/software/cube-4.x/documentation.html\"> Cube Derived Metrics </a> help."
                 "</body>"
                 "</html>"
                 ),
    QObject::tr( "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">"
                 "<html>"
                 "<head>"
                 "<title>\"Plus\" Operation while exclusive or inclusive value calculation</title>"
                 "</head>"
                 "<body>"
                 "For exclusive metric: to calculate inclusive value out of exclusive values one <b>sums</b> up values of the children in calltree. "
                 "<br>"
                 "For inclusive metric: to calculate exclusive value out of inclusive values one uses a <b>sum</b> of all values of the children in calltree. "
                 "<br>"
                 "<br>"
                 " One can redefine this operation by any CubePL valid expression, using keywords \"arg1\" and \"arg2\". "
                 "<br>"
                 "<br>"
                 "For further information please look <a href=\"http://www.scalasca.org/software/cube-4.x/documentation.html\"> Cube Derived Metrics </a> help."
                 "</body>"
                 "</html>"
                 ),
    QObject::tr( "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">"
                 "<html>"
                 "<head>"
                 "<title>\"Minus\" Operation while exclusive or inclusive value calculation</title>"
                 "</head>"
                 "<body>"
                 "For inclusive metric: to calculate exclusive value out of inclusive values one <b>substracts</b>  a sum of all values of the children in calltree out if the current callpath value. "
                 "<br>"
                 "<br>"
                 " One can redefine this operation by any CubePL valid expression, using keywords \"arg1\" and \"arg2\". "
                 "<br>"
                 "<br>"
                 "For further information please look <a href=\"http://www.scalasca.org/software/cube-4.x/documentation.html\"> Cube Derived Metrics </a> help."
                 "</body>"
                 "</html>"
                 ),
    QObject::tr( "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">"
                 "<html>"
                 "<head>"
                 "<title>Operation while aggregation over system dimension</title>"
                 "</head>"
                 "<body>"
                 "To calculate valus in central pane in Cube GUI one has to aggregate over system dimension. This operation is performed to do this aggregation."
                 "<br>"
                 "<br>"
                 " One can redefine this operation by any CubePL valid expression, using keywords \"arg1\" and \"arg2\". "
                 "<br>"
                 "<br>"
                 "For further information please look <a href=\"http://www.scalasca.org/software/cube-4.x/documentation.html\"> Cube Derived Metrics </a> help."
                 "</body>"
                 "</html>"
                 ),
    QObject::tr( "<span style=\"color: black; \">"
                 "Creates a metric from the clipboard content (if it is a text).  <br>"
                 "<br>"
                 "<br>"
                 "<u>Content syntax</u>:<br>"
                 "<br>"
                 "<b>#</b>XXXX <br>"
                 " comment - will be ignored <br>"
                 "<br>"
                 "<u>Metric type:</u><b> postderived | prederived_exclusive | prederived_inclusive  </b><br>"
                 " Type of the derived metric.<br>"
                 "<br>"
                 "<u>Display name:</u><b>XXX</b><br>"
                 " Display name of the metric.<br>"
                 "<br>"
                 "<u>Unique name:</u><b>XXX</b><br>"
                 " Unique  name of the metric.<br>"
                 "<br>"
                 "<u>Uom:</u><b>XXX</b><br>"
                 " Unit of measurement.<br>"
                 "<br>"
                 "<u>Url:</u><b>XXX</b><br>"
                 " Url with the description.<br>"
                 "<br>"
                 "<u>Description:</u><b>XXX</b><br>"
                 " Description of the metric.<br>"
                 "<br>"
                 "<u>CubePL Expression:</u><b>XXX</b><br>"
                 " Expression for the calculation of the metric value in CubePL.<br>"
                 " Expression can be written in several lines. Parser collects all lines<br>"
                 " till it finds another keyword.<br>"
                 "<br>"
                 "<u>CubePL Init Expression:</u><b>XXX</b><br>"
                 " Initialization expression for the calculation of the metric value in CubePL.<br>"
                 " Expression can be written in several lines. Parser collects all lines<br>"
                 " till it finds another keyword.<br>"
                 "<br>"
                 "</span>"
                 ),
    QObject::tr( "<span style=\"color: black; \">"
                 "Creates a metric from file.  <br>"
                 "<br>"
                 "<br>"
                 "<u>File syntax</u>:<br>"
                 "<br>"
                 "<b>#</b>XXXX <br>"
                 " comment - will be ignored <br>"
                 "<br>"
                 "<u>Metric type:</u><b> postderived | prederived_exclusive | prederived_inclusive  </b><br>"
                 " Type of the derived metric.<br>"
                 "<br>"
                 "<u>Display name:</u><b>XXX</b><br>"
                 " Display name of the metric.<br>"
                 "<br>"
                 "<u>Unique name:</u><b>XXX</b><br>"
                 " Unique  name of the metric.<br>"
                 "<br>"
                 "<u>Uom:</u><b>XXX</b><br>"
                 " Unit of measurement.<br>"
                 "<br>"
                 "<u>Url:</u><b>XXX</b><br>"
                 " Url with the description.<br>"
                 "<br>"
                 "<u>Description:</u><b>XXX</b><br>"
                 " Description of the metric.<br>"
                 "<br>"
                 "<u>CubePL Expression:</u><b>XXX</b><br>"
                 " Expression for the calculation of the metric value in CubePL.<br>"
                 " Expression can be written in several lines. Parser collects all lines<br>"
                 " till it finds another keyword.<br>"
                 "<br>"
                 "<u>CubePL Init Expression:</u><b>XXX</b><br>"
                 " Initialization expression for the calculation of the metric value in CubePL.<br>"
                 " Expression can be written in several lines. Parser collects all lines<br>"
                 " till it finds another keyword.<br>"
                 "<br>"
                 "</span>"
                 ),
    QObject::tr( "<span style=\"color: black; \">"
                 "Stores current definition of a metric in a file.  <br>"
                 "<br>"
                 "<br>"
                 "<u>File syntax</u>:<br>"
                 "<br>"
                 "<u>Metric type:</u><b> postderived | prederived_exclusive | prederived_inclusive  </b><br>"
                 " Type of the derived metric.<br>"
                 "<br>"
                 "<u>Display name:</u><b>XXX</b><br>"
                 " Display name of the metric.<br>"
                 "<br>"
                 "<u>Unique name:</u><b>XXX</b><br>"
                 " Unique  name of the metric.<br>"
                 "<br>"
                 "<u>Uom:</u><b>XXX</b><br>"
                 " Unit of measurement.<br>"
                 "<br>"
                 "<u>Url:</u><b>XXX</b><br>"
                 " Url with the description.<br>"
                 "<br>"
                 "<u>Description:</u><b>XXX</b><br>"
                 " Description of the metric.<br>"
                 "<br>"
                 "<u>CubePL Expression:</u><b>XXX</b><br>"
                 " Expression for the calculation of the metric value in CubePL.<br>"
                 " Expression can be written in several lines. Parser collects all lines<br>"
                 " till it finds another keyword.<br>"
                 "<br>"
                 "<u>CubePL Init Expression:</u><b>XXX</b><br>"
                 " Initialization expression for the calculation of the metric value in CubePL.<br>"
                 " Expression can be written in several lines. Parser collects all lines<br>"
                 " till it finds another keyword.<br>"
                 "<br>"
                 "</span>"
                 ),
    QString( ""
             )
};


#endif
