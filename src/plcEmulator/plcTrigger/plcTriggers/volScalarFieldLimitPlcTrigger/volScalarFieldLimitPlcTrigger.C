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

#include "volScalarFieldLimitPlcTrigger.H"
#include "addToRunTimeSelectionTable.H"
#include "fvCFD.H"
#include "plcEmulator.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(volScalarFieldLimitPlcTrigger, 0);

    addToRunTimeSelectionTable
    (
        plcTrigger,
        volScalarFieldLimitPlcTrigger,
        dictionary
    );
}

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

void Foam::volScalarFieldLimitPlcTrigger::readDict
(
    const dictionary& inputDict
)
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
                "volScalarFieldLimitPlcTrigger::volScalarFieldLimitPlcTrigger",
                inputDict
            ) << "volScalarFieldLimit plcTrigger " << name_ << " has unknown "
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
            "volScalarFieldLimitPlcTrigger::volScalarFieldLimitPlcTrigger",
            inputDict
        ) << "volScalarFieldLimit plcTrigger " << name_ << " requires keyword "
            << "greaterThan or lessThan"
            << exit(FatalIOError);
    }
}

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::volScalarFieldLimitPlcTrigger::volScalarFieldLimitPlcTrigger
(
    plcEmulator& control,
    const word& triggerName,
    const dictionary& inputDict
)
:
    plcTrigger(control, triggerName, inputDict),
    objectRegistrySearcher<volScalarField>
    (
        inputDict.lookup("objectRegistry")
    ),

    evalMethod_(emunknown),

    limit_(0.0),
    overLimitValue_(false),
    variableName_(inputDict.lookup("variableName")),

    geoIndex_(0),
    cellIndex_(-1)
{
    readDict(inputDict);
}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool Foam::volScalarFieldLimitPlcTrigger::evaluate(const Time& runTime)
{
    const volScalarField * vsfPtr(lookupObject(variableName_, runTime));

    if (!vsfPtr)
    {
        const wordList& storeFields(control_.multiRun().storeFields());
        forAll(storeFields, storeI)
        {
            if (storeFields[storeI] == variableName_)
            {
                // The variable is a store field - assume no change
                return value_;
            }
        }
        Info << "Valid names in given objectRegistries are:" << endl;
        showNames(runTime);
        FatalErrorIn("volScalarFieldLimitPlcTrigger::evaluate")
            << "volScalarFieldLimitPlcTrigger " << name_ << " cannot find "
            << variableName_ << " in any of the objectRegistry paths: "
            << objectRegistryPaths() << "\nSee Info above for valid names."
            << abort(FatalError);
    }

    const volScalarField& vsf(*vsfPtr);
    scalar result(0);
    switch (evalMethod_)
    {
        case empoint:
            if (geoIndex_ == 0)
            {
                result = vsf.internalField()[cellIndex_];
            }
            else
            {
                result = vsf.boundaryField()[geoIndex_ - 1][cellIndex_];
            }
            break;
        case empatchFieldAverage:
        {
            const fvPatch& fvp
            (
                vsf.dimensionedInternalField().mesh().boundary()[geoIndex_ - 1]
            );
            result = gSum
            (
                vsf.boundaryField()[geoIndex_ - 1] * fvp.magSf()
            ) / gSum(fvp.magSf());
            break;
        }
        case empatchFieldMaximum:
        {
            result = max(vsf.boundaryField()[geoIndex_ - 1]);
            break;
        }
        case empatchFieldMinimum:
        {
            result = min(vsf.boundaryField()[geoIndex_ - 1]);
            break;
        }
        case eminternalFieldAverage:
        {
            result = vsf.weightedAverage
            (
                vsf.dimensionedInternalField().mesh().V()
            ).value();
            break;
        }
        case eminternalFieldMaximum:
        {
            result = max(vsf.internalField());
            break;
        }
        case eminternalFieldMinimum:
        {
            result = min(vsf.internalField());
            break;
        }
        default:
            FatalErrorIn("volScalarFieldLimitPlcTrigger::evaluate")
                << "Unrecognized evaluation method value "
                << label(evalMethod_)
                << abort(FatalError);
    }
    value_ = (overLimitValue_) == (result > limit_);
    return value_;
}

// ************************************************************************* //
