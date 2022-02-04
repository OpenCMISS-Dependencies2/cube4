/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2016                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2009-2015                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/



#include "config.h"

#include "CubeStringSemiEqualityEvaluation.h"

#define SEMI_COMPARE_STRING( A, B ) services::lowercase( A->strEval() ).compare(  services::lowercase( B->strEval() ) ) == 0



using namespace cube;
using namespace std;



StringSemiEqualityEvaluation::StringSemiEqualityEvaluation() : BinaryEvaluation()
{
};

StringSemiEqualityEvaluation::StringSemiEqualityEvaluation( GeneralEvaluation* _arg1,
                                                            GeneralEvaluation* _arg2 ) : BinaryEvaluation( _arg1, _arg2 )
{
};


StringSemiEqualityEvaluation::~StringSemiEqualityEvaluation()
{
}
