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

#include "conditionalSwitchPlcTrigger.H"
#include "addToRunTimeSelectionTable.H"
#include "plcEmulator.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(conditionalSwitchPlcTrigger, 0);

    addToRunTimeSelectionTable
    (
        plcTrigger,
        conditionalSwitchPlcTrigger,
        dictionary
    );
}

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

void Foam::conditionalSwitchPlcTrigger::readDict()
{
    const PtrList<plcTrigger>& triggers(control_.triggers());
    
    const dictionary& trueWhenDict(inputDict_.subDict("trueWhen"));
    wordList onTriggeredNames(trueWhenDict.lookup("triggered"));
    onTriggered_.setSize(onTriggeredNames.size());
    
    forAll (onTriggeredNames, nameI)
    {
        const word& findMe(onTriggeredNames[nameI]);
        if (findMe == name_)
        {
            FatalIOErrorIn
            (
                "conditionalSwitchPlcTrigger::readDict",
                trueWhenDict
            )
                << "Self-referencing not allowed."
                << exit(FatalIOError);
        }
        bool foundMe(false);
        forAll(triggers, triggerI)
        {
            if (findMe == triggers[triggerI].name())
            {
                onTriggered_.set(nameI, &triggers[triggerI]);
                foundMe = true;
                break;
            }
        }
        if (!foundMe)
        {
            FatalIOErrorIn
            (
                "conditionalSwitchPlcTrigger::readDict",
                trueWhenDict
            )
                << "plcTrigger '" << findMe << "' not found."
                << exit(FatalIOError);
        }
    }

    wordList onNotTriggeredNames(trueWhenDict.lookup("notTriggered"));
    onNotTriggered_.setSize(onNotTriggeredNames.size());
    
    forAll (onNotTriggeredNames, nameI)
    {
        const word& findMe(onNotTriggeredNames[nameI]);
        if (findMe == name_)
        {
            FatalIOErrorIn
            (
                "conditionalSwitchPlcTrigger::readDict",
                trueWhenDict
            )
                << "Self-referencing not allowed."
                << exit(FatalIOError);
        }
        bool foundMe(false);
        forAll(triggers, triggerI)
        {
            if (findMe == triggers[triggerI].name())
            {
                onNotTriggered_.set(nameI, &triggers[triggerI]);
                foundMe = true;
                break;
            }
        }
        if (!foundMe)
        {
            FatalIOErrorIn
            (
                "conditionalSwitchPlcTrigger::readDict",
                trueWhenDict
            )
                << "plcTrigger '" << findMe << "' not found."
                << exit(FatalIOError);
        }
    }

    const dictionary& falseWhenDict(inputDict_.subDict("falseWhen"));
    wordList offTriggeredNames(falseWhenDict.lookup("triggered"));
    offTriggered_.setSize(offTriggeredNames.size());
    
    forAll (offTriggeredNames, nameI)
    {
        const word& findMe(offTriggeredNames[nameI]);
        if (findMe == name_)
        {
            FatalIOErrorIn
            (
                "conditionalSwitchPlcTrigger::readDict",
                falseWhenDict
            )
                << "Self-referencing not allowed."
                << exit(FatalIOError);
        }
        bool foundMe(false);
        forAll(triggers, triggerI)
        {
            if (findMe == triggers[triggerI].name())
            {
                offTriggered_.set(nameI, &triggers[triggerI]);
                foundMe = true;
                break;
            }
        }
        if (!foundMe)
        {
            FatalIOErrorIn
            (
                "conditionalSwitchPlcTrigger::readDict",
                falseWhenDict
            )
                << "plcTrigger '" << findMe << "' not found."
                << exit(FatalIOError);
        }
    }

    wordList offNotTriggeredNames(falseWhenDict.lookup("notTriggered"));
    offNotTriggered_.setSize(offNotTriggeredNames.size());
    
    forAll (offNotTriggeredNames, nameI)
    {
        const word& findMe(offNotTriggeredNames[nameI]);
        if (findMe == name_)
        {
            FatalIOErrorIn
            (
                "conditionalSwitchPlcTrigger::readDict",
                falseWhenDict
            )
                << "Self-referencing not allowed."
                << exit(FatalIOError);
        }
        bool foundMe(false);
        forAll(triggers, triggerI)
        {
            if (findMe == triggers[triggerI].name())
            {
                offNotTriggered_.set(nameI, &triggers[triggerI]);
                foundMe = true;
                break;
            }
        }
        if (!foundMe)
        {
            FatalIOErrorIn
            (
                "conditionalSwitchPlcTrigger::readDict",
                falseWhenDict
            )
                << "plcTrigger '" << findMe << "' not found."
                << exit(FatalIOError);
        }
    }
    
    label totalSize
    (
        onTriggered_.size() + onNotTriggered_.size()
      + offTriggered_.size() + offNotTriggered_.size()
    );
    
    if (!totalSize)
    {
        FatalIOErrorIn("conditionalSwitchPlcTrigger::readDict", inputDict_)
            << "No conditions specified."
            << exit(FatalIOError);
    }
    inputDictWasRead_ = true;
}

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::conditionalSwitchPlcTrigger::conditionalSwitchPlcTrigger
(
    plcEmulator& control,
    const word& triggerName,
    const dictionary& inputDict
)
:
    plcTrigger(control, triggerName, inputDict),

    inputDict_(inputDict),
    inputDictWasRead_(false),

    onTriggered_(0),
    onNotTriggered_(0),
    offTriggered_(0),
    offNotTriggered_(0)
{}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool Foam::conditionalSwitchPlcTrigger::evaluate(const Time& runTime)
{
    if (!inputDictWasRead_)
    {
        readDict();
    }
    bool conditionsMet(true);
    if (onTriggered_.size() + onNotTriggered_.size())
    {
        forAll(onTriggered_, triggerI)
        {
            if (!onTriggered_[triggerI].value())
            {
                conditionsMet = false;
                break;
            }
        }
        
        if (conditionsMet)
        {
            forAll(onNotTriggered_, triggerI)
            {
                if (onNotTriggered_[triggerI].value())
                {
                    conditionsMet = false;
                    break;
                }
            }
        }
        if (conditionsMet)
        {
            value_ = true;
            return value_;
        }
    }

    if (offTriggered_.size() + offNotTriggered_.size())
    {
        conditionsMet = true;
        forAll(offTriggered_, triggerI)
        {
            if (!offTriggered_[triggerI].value())
            {
                conditionsMet = false;
                break;
            }
        }
        
        if (conditionsMet)
        {
            forAll(offNotTriggered_, triggerI)
            {
                if (offNotTriggered_[triggerI].value())
                {
                    conditionsMet = false;
                    break;
                }
            }
        }
        
        if (conditionsMet)
        {
            value_ = false;
            return value_;
        }
    }
    
    // Neither conditions were met, check for empty conditions
    if (!(onTriggered_.size() + onNotTriggered_.size()))
    {
        value_ = true;
    }
    if (!(offTriggered_.size() + offNotTriggered_.size()))
    {
        value_ = false;
    }

    return value_;
}


void Foam::conditionalSwitchPlcTrigger::readOutputDict
(
    const dictionary& outputSubDict
)
{
    value_ = readBool(outputSubDict.lookup("value"));
}


Foam::dictionary Foam::conditionalSwitchPlcTrigger::writeOutputDict() const
{
    dictionary returnMe;
    returnMe.set("value", value_);
    return returnMe;
}

// ************************************************************************* //
