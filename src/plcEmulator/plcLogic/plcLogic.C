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

#include "plcLogic.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * Static Member Data  * * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(plcLogic, 0);
}

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

void Foam::plcLogic::readDict
(
    const dictionary& inputDict,
    const PtrList<plcTrigger>& triggers
)
{
    wordList triggeredNames(inputDict.lookup("triggered"));
    triggered_.setSize(triggeredNames.size());
    forAll(triggeredNames, nIndex)
    {
        word triggerName(triggeredNames[nIndex]);
        bool foundIt(false);
        forAll(triggers, tIndex)
        {
            const plcTrigger& testTrigger(triggers[tIndex]);
            if (testTrigger.name() == triggerName)
            {
                triggered_.set(nIndex, &testTrigger);
                foundIt = true;
                break;
            }
        }
        if (!foundIt)
        {
            FatalIOErrorIn("plcLogic::readDict", inputDict)
                << "Could not find trigger " << triggerName
                << exit(FatalIOError);
        }
    }

    wordList notTriggeredNames(inputDict.lookup("notTriggered"));
    notTriggered_.setSize(notTriggeredNames.size());
    forAll(notTriggeredNames, nIndex)
    {
        word triggerName(notTriggeredNames[nIndex]);
        bool foundIt(false);
        forAll(triggers, tIndex)
        {
            const plcTrigger& testTrigger(triggers[tIndex]);
            if (testTrigger.name() == triggerName)
            {
                notTriggered_.set(nIndex, &testTrigger);
                foundIt = true;
                break;
            }
        }
        if (!foundIt)
        {
            FatalIOErrorIn("plcLogic::readDict", inputDict)
                << "Could not find trigger " << triggerName
                << exit(FatalIOError);
        }
    }
}

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::plcLogic::plcLogic
(
    const word& name,
    const dictionary& inputDict,
    const PtrList<plcTrigger>& triggers,
    const wordList& solverDomains
)
:
    name_(name),
    solverDomainName_(inputDict.lookup("solverDomain")),
    solverDomainIndex_(-1),
    triggered_(0),
    notTriggered_(0)
{
    forAll(solverDomains, sdi)
    {
        if (solverDomainName_ == solverDomains[sdi])
        {
            solverDomainIndex_ = sdi;
            break;
        }
    }
    if (solverDomainIndex_ == -1)
    {
        FatalIOErrorIn("plcLogic::plcLogic", inputDict)
            << "Solver domain '" << solverDomainName_ << "' is unrecognized. "
            << "Available solver domains are:" << solverDomains
            << exit(FatalIOError);
    }

    // Read inputDict
    readDict(inputDict, triggers);
}

// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

bool Foam::plcLogic::test() const
{
    forAll(triggered_, triggerI)
    {
        if (!triggered_[triggerI].value())
        {
            return false;
        }
    }
    
    forAll(notTriggered_, triggerI)
    {
        if (notTriggered_[triggerI].value())
        {
            return false;
        }
    }
    return true;
}


void Foam::plcLogic::report() const
{
    Info << "plcLogic " << name_ << ":" << endl;
    Info << token::TAB << "triggered (" << triggered_.size() << "):" << endl;
    forAll(triggered_, ti)
    {
        Info << token::TAB << token::TAB << triggered_[ti].name()
            << " = " << triggered_[ti].value() << endl;
    }
    Info << token::TAB << "notTriggered (" << notTriggered_.size() << "):"
        << endl;
    forAll(notTriggered_, ti)
    {
        Info << token::TAB << token::TAB << notTriggered_[ti].name()
            << " = " << notTriggered_[ti].value() << endl;
    }
}

// ************************************************************************* //
