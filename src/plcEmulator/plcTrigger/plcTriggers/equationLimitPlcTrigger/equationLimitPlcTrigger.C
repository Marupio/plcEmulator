/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright held by original author
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

\*---------------------------------------------------------------------------*/

#include "equationLimitPlcTrigger.H"
#include "addToRunTimeSelectionTable.H"
#include "IOEquationReader.H"
#include "fvCFD.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(equationLimitPlcTrigger, 0);

    addToRunTimeSelectionTable
    (
        plcTrigger,
        equationLimitPlcTrigger,
        dictionary
    );
}

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

void Foam::equationLimitPlcTrigger::readDict(const dictionary& inputDict)
{
    word evaluationWord(inputDict.lookup("evaluation"));
    if (evaluationWord == "point")
    {
        evalMethod_ = empoint;
    }
    else if (evaluationWord == "patchFieldAverage")
    {
        evalMethod_ = empatchFieldAverage;
    }
    else if (evaluationWord == "patchFieldMaximum")
    {
        evalMethod_ = empatchFieldMaximum;
    }
    else if (evaluationWord == "patchFieldMinimum")
    {
        evalMethod_ = empatchFieldMinimum;
    }
    else if (evaluationWord == "internalFieldAverage")
    {
        evalMethod_ = eminternalFieldAverage;
    }
    else if (evaluationWord == "internalFieldMaximum")
    {
        evalMethod_ = eminternalFieldMaximum;
    }
    else if (evaluationWord == "internalFieldMinimum")
    {
        evalMethod_ = eminternalFieldMinimum;
    }
    
    switch (evalMethod_)
    {
        case empoint:
            if (inputDict.found("geoIndex"))
            {
                geoIndex_ = readLabel(inputDict.lookup("geoIndex"));
            }
            cellIndex_ = readLabel(inputDict.lookup("cellIndex"));
            break;
            
        case empatchFieldAverage:
        case empatchFieldMaximum:
        case empatchFieldMinimum:
            geoIndex_ = readLabel(inputDict.lookup("patchIndex")) + 1;
            break;
            
        case eminternalFieldAverage:
        case eminternalFieldMaximum:
        case eminternalFieldMinimum:
            break;
            
        default:
        {
            wordList toc(7);
            toc[0] = "point";
            toc[1] = "patchFieldAverage";
            toc[2] = "patchFieldMaximum";
            toc[3] = "patchFieldMinimum";
            toc[4] = "internalFieldAverage";
            toc[5] = "internalFieldMaximum";
            toc[6] = "internalFieldMinimum";

            FatalIOErrorIn
            (
                "equationLimitPlcTrigger::equationLimitPlcTrigger",
                inputDict
            ) << "equationLimit plcTrigger " << name_ << " has unknown "
                << "evaluation method.  Available methods are: " << toc
                << exit(FatalIOError);
        }
    }

    if (inputDict.found("lessThan"))
    {
        limit_ = readScalar(inputDict.lookup("lessThan"));
    }
    else if (inputDict.found("greaterThan"))
    {
        limit_ = readScalar(inputDict.lookup("greaterThan"));
        overLimitValue_ = true;
    }
    else
    {
        FatalIOErrorIn
        (
            "equationLimitPlcTrigger::equationLimitPlcTrigger",
            inputDict
        ) << "equationLimit plcTrigger " << name_ << " requires keyword "
            << "greaterThan or lessThan"
            << exit(FatalIOError);
    }
}

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::equationLimitPlcTrigger::equationLimitPlcTrigger
(
    plcEmulator& control,
    const word& triggerName,
    const dictionary& inputDict
)
:
    plcTrigger(control, triggerName, inputDict),
    objectRegistrySearcher<IOEquationReader>
    (
        inputDict.lookup("equationReaderObjectRegistry")
    ),
    objectRegistrySearcher<fvMesh>
    (
        word(inputDict.lookup("evaluation")) != word("point")
      ? inputDict.lookup("meshObjectRegistry")
      : fileNameList(0)
    ),

    evalMethod_(emunknown),

    limit_(0.0),
    overLimitValue_(false),
    eqnName_(inputDict.lookup("equationName")),
    eqnReaderName_
    (
        inputDict.found("equationReaderName")
      ? inputDict.lookup("equationReaderName")
      : word("eqns")
    ),

    geoIndex_(0),
    cellIndex_(-1),

    meshName_
    (
        inputDict.found("meshName")
      ? inputDict.lookup("meshName")
      : word("region0")
    )
{
    readDict(inputDict);
}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool Foam::equationLimitPlcTrigger::evaluate(const Time& runTime)
{
    const IOEquationReader * eqnsPtr
    (
        objectRegistrySearcher<IOEquationReader>::lookupObject
        (
            eqnReaderName_,
            runTime
        )
    );

    if (!eqnsPtr)
    {
        Info << "Valid equationReader names in given objectRegistries are:"
            << endl;
        objectRegistrySearcher<IOEquationReader>::showNames(runTime);
        FatalErrorIn("equationLimitPlcTrigger::evaluate")
            << "equationLimitPlcTrigger " << name_ << " cannot find "
            << eqnReaderName_ << " in any of the objectRegistry paths: "
            << objectRegistrySearcher<IOEquationReader>::objectRegistryPaths()
            << "\nSee Info above for valid names."
            << abort(FatalError);
    }

    const IOEquationReader& eqns(*eqnsPtr);

    label eqnIndex(eqns.lookup(eqnName_));
    if (eqnIndex < 0)
    {
        FatalErrorIn("equationLimitPlcTrigger::evaluate")
            << "Equation " << eqnName_ << " not found."
            << abort(FatalError);
    }
    scalar result(0);
    
    const fvMesh * meshPtr(NULL);
    
    if (evalMethod_ != empoint)
    {
        meshPtr = objectRegistrySearcher<fvMesh>::lookupObject
        (
            meshName_,
            runTime
        );

        if (!meshPtr)
        {
            Info << "Valid fvMesh names in given objectRegistries are:"
                << endl;
            objectRegistrySearcher<fvMesh>::showNames(runTime);
            FatalErrorIn("equationLimitPlcTrigger::evaluate")
                << "equationLimitPlcTrigger " << name_ << " cannot find "
                << meshName_ << " in any of the objectRegistry "
                << "paths: "
                << objectRegistrySearcher<fvMesh>::objectRegistryPaths()
                << "\nSee Info above for valid names."
                << abort(FatalError);
        }
    }
    
    switch (evalMethod_)
    {
        case empoint:
            result = eqns.evaluateScalar(eqnIndex, cellIndex_, geoIndex_);
            break;
        case empatchFieldAverage:
        {
            const fvMesh& mesh(*meshPtr);
            const fvPatch& face(mesh.boundary()[geoIndex_ - 1]);
            scalarField sfa(face.size());
            eqns.evaluateScalarField(sfa, eqnIndex, geoIndex_);
            result = gSum(sfa * face.magSf()) / gSum(face.magSf());
            break;
        }    
        case empatchFieldMaximum:
        {
            const fvMesh& mesh(*meshPtr);
            const fvPatch& face(mesh.boundary()[geoIndex_ - 1]);
            scalarField sfa(face.size());
            eqns.evaluateScalarField(sfa, eqnIndex, geoIndex_);
            result = min(sfa);
            break;
        }
        case empatchFieldMinimum:
        {
            const fvMesh& mesh(*meshPtr);
            const fvPatch& face(mesh.boundary()[geoIndex_ - 1]);
            scalarField sfa(face.size());
            eqns.evaluateScalarField(sfa, eqnIndex, geoIndex_);
            result = max(sfa);
            break;
        }
        case eminternalFieldAverage:
        {
            const fvMesh& mesh(*meshPtr);
            scalarField sfa(mesh.nCells());
            eqns.evaluateScalarField(sfa, eqnIndex);
            result = gSum(sfa * mesh.V().field()) / gSum(mesh.V().field());
            break;
        }
        case eminternalFieldMaximum:
        {
            const fvMesh& mesh(*meshPtr);
            scalarField sfa(mesh.nCells());
            eqns.evaluateScalarField(sfa, eqnIndex);
            result = max(sfa);
            break;
        }
        case eminternalFieldMinimum:
        {
            const fvMesh& mesh(*meshPtr);
            scalarField sfa(mesh.nCells());
            eqns.evaluateScalarField(sfa, eqnIndex);
            result = min(sfa);
            break;
        }
        default:
            FatalErrorIn("equationLimitPlcTrigger::evaluate")
                << "Unrecognized evaluation method value "
                << label(evalMethod_)
                << abort(FatalError);
    }
    value_ = (overLimitValue_) == (result > limit_);
    return value_;
}

// ************************************************************************* //
