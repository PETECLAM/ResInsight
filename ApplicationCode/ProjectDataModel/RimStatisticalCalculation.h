/////////////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2011-2012 Statoil ASA, Ceetron AS
// 
//  ResInsight is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
// 
//  ResInsight is distributed in the hope that it will be useful, but WITHOUT ANY
//  WARRANTY; without even the implied warranty of MERCHANTABILITY or
//  FITNESS FOR A PARTICULAR PURPOSE.
// 
//  See the GNU General Public License at <http://www.gnu.org/licenses/gpl.html> 
//  for more details.
//
/////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "cvfBase.h"
#include "cvfObject.h"
#include "cafPdmField.h"
#include "cafPdmObject.h"

#include "RimReservoir.h"

class RimIdenticalGridCaseGroup;
class RimResultDefinition;
class RifReaderStatisticalCalculation;
class RimStatisticalCollection;
class RigMainGrid;


//==================================================================================================
//
// 
//
//==================================================================================================
class RimStatisticalCalculation : public RimReservoir
{
    CAF_PDM_HEADER_INIT;

public:
    RimStatisticalCalculation();
    virtual ~RimStatisticalCalculation();

    void setMainGrid(RigMainGrid* mainGrid);

    virtual bool openEclipseGridFile();

    caf::PdmField<QString> m_resultName;

    caf::PdmField<bool> statisticsMin;
    caf::PdmField<bool> statisticsMax;
    caf::PdmField<bool> statisticsMean;
    caf::PdmField<bool> statisticsStdDev;

    RimStatisticalCollection* parent();

    virtual void defineUiOrdering( QString uiConfigName, caf::PdmUiOrdering& uiOrdering ) const;
    void computeStatistics();

private:
    RimIdenticalGridCaseGroup* caseGroup();

    void createAndComputeMin();
    void createAndComputeMax();
    void createAndComputeMean();
    void createAndComputeStdDev();

    void getSourceCases(cvf::Collection<RigEclipseCase>& sourceCases);


private:
    cvf::ref<RifReaderStatisticalCalculation> m_readerInterface;

};