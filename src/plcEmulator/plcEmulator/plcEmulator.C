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

#include "plcEmulator.H"
#include "timerPlcTrigger.H"
#include "solverSignalPlcTrigger.H"
#include "boolList.H"
//#include "IOstreams.H"
//#include "token.H"
//#include "dictionary.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * Static Member Data  * * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(plcEmulator, 0);
}

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

void Foam::plcEmulator::readInputDict()
{
    // Read algorithms
    const dictionary& algorithmDict
    (
        plcEmulatorDict_.subDict("algorithmGroups")
    );
    
    algorithms_ = algorithmDict.toc();
    algorithmGroups_.setSize(algorithms_.size());
    
    forAll(algorithms_, algorithmIndex)
    {
        wordList inThisGroup
        (
            algorithmDict.lookup(algorithms_[algorithmIndex])
        );
        algorithmGroups_[algorithmIndex].setSize(inThisGroup.size());
        
        forAll(inThisGroup, groupIndex)
        {
            const word& thisSolver(inThisGroup[groupIndex]);
            bool foundIt(false);
            forAll(solverDomains_, solverIndex)
            {
                if (thisSolver == solverDomains_[solverIndex])
                {
                    algorithmGroups_[algorithmIndex][groupIndex] = solverIndex;
                    foundIt = true;
                    break;
                }
            }
            if (!foundIt)
            {
                FatalIOErrorIn("plcEmulator::readInputDict", algorithmDict)
                    << "Solver domain '" << thisSolver << "' not recognized. "
                    << "Must also be defined in multiSolverControlDict."
                    << exit(FatalIOError);
            }
        } // forAll algorithmGroups
    } // forAll algorithms
    
    // Read triggers
    const dictionary& triggerDict
    (
        plcEmulatorDict_.subDict("triggers")
    );
    
    wordList triggerNames(triggerDict.toc());
    triggers_.setSize(triggerNames.size());
    forAll(triggers_, triggerIndex)
    {
        const word& tName(triggerNames[triggerIndex]);
        triggers_.set
        (
            triggerIndex,
            plcTrigger::New
            (
                *this,
                tName,
                triggerDict.subDict(tName)
            ).ptr()
        );
        
        plcTrigger& newTrigger(triggers_[triggerIndex]);
        if (newTrigger.type() == "timer")
        {
            timerPlcTrigger& newTimer
            (
                refCast<timerPlcTrigger>(newTrigger)
            );
            label newIndex(timerTriggers_.size());
            timerTriggers_.setSize(newIndex + 1);
            timerTriggers_.set(newIndex, & newTimer);
        }
        else if (newTrigger.type() == "solverSignal")
        {
            solverSignalPlcTrigger& newSignal
            (
                refCast<solverSignalPlcTrigger>(newTrigger)
            );
            label newIndex(solverSignalTriggers_.size());
            solverSignalTriggers_.setSize(newIndex + 1);
            solverSignalTriggers_.set(newIndex, & newSignal);
        }
    }
    
    // Read logic
    const dictionary& logicDict
    (
        plcEmulatorDict_.subDict("logic")
    );
    
    wordList logicNames(logicDict.toc());
    wordList logicOrder;
    if (logicDict.found("order"))
    {
        logicOrder = wordList(logicDict.lookup("order"));
        logic_.setSize(logicNames.size() - 1);

        if (logicOrder.size() != (logicNames.size() - 1))
        {
            FatalIOErrorIn("plcEmulator::readInputDict", logicDict)
                << "Keyword 'order' contains " << logicOrder.size()
                << " elements, expecting " << logicNames.size() - 1
                << exit(FatalIOError);
        }
    }
    else
    {
        // Assume order doesn't matter
        logicOrder = logicNames;
        logic_.setSize(logicNames.size());
    }
    
    forAll(logicOrder, logicIndex)
    {
        word& logicName(logicOrder[logicIndex]);
        logic_.set
        (
            logicIndex,
            new plcLogic
            (
                logicName,
                logicDict.subDict(logicName),
                triggers_,
                solverDomains_
            )
        );
    }
}


void Foam::plcEmulator::readOutputDict(Time& runTime)
{
    IOdictionary outputIODict
    (
        IOobject
        (
            outputDictName_,
            runTime.timeName(),
            runTime,
            IOobject::READ_IF_PRESENT,
            IOobject::NO_WRITE
        )
    );
    outputDict_ = outputIODict;

    if (outputDict_.found("endTimeChanged"))
    {
        nextTimerEnd_ = readScalar(outputDict_.lookup("nextTimerEnd"));
        endTimeChanged_ = readBool(outputDict_.lookup("endTimeChanged"));
        oldEndTime_ = readScalar(outputDict_.lookup("oldEndTime"));
        deltaTChanged_ = readBool(outputDict_.lookup("deltaTChanged"));
        oldDeltaT_ = readScalar(outputDict_.lookup("oldDeltaT"));
        writeIntervalChanged_ = readBool
        (
            outputDict_.lookup("writeIntervalChanged")
        );
    }

    forAll(triggers_, triggerI)
    {
        plcTrigger& thisTrigger(triggers_[triggerI]);
        if (outputDict_.found(thisTrigger.name()))
        {
            thisTrigger.readOutputDict
            (
                outputDict_.subDict(thisTrigger.name())
            );
        }
    }
}


void Foam::plcEmulator::resetTimers(Time& runTime)
{
    bool timerExpired(false);
    scalar globalTime(runTime.value() + multiRun_.globalTimeOffset());
    forAll(timerTriggers_, triggerI)
    {
        timerPlcTrigger& trigger(timerTriggers_[triggerI]);
        if ((trigger.globalEndTime() - globalTime) < trigger.roundOff())
        {
            trigger.reset();
            timerExpired = true;
        }
    }

    if (timerExpired)
    {
        if (endTimeChanged_)
        {
            runTime.setEndTime(oldEndTime_);
            endTimeChanged_ = false;
            if (debug)
            {
                Info << "Resetting endTime to " << oldEndTime_ << endl;
            }
        }
        if (deltaTChanged_)
        {
            if (debug)
            {
                Info << "Resetting deltaT to " << oldDeltaT_ << endl;
            }
            runTime.setDeltaT(oldDeltaT_);
            deltaTChanged_ = false;
        }
        if (writeIntervalChanged_)
        {
            if (debug)
            {
                Info << "Resetting writeInterval to " << readScalar
                (
                    runTime.controlDict().lookup("writeInterval")
                ) << endl;
            }
            runTime.setWriteInterval
            (
                readScalar(runTime.controlDict().lookup("writeInterval"))
            );
            writeIntervalChanged_ = false;
        }
        adjustTime(runTime);
    }
}


void Foam::plcEmulator::updateOutputDict()
{
    outputDict_.set("nextTimerEnd", nextTimerEnd_);
    outputDict_.set("endTimeChanged", endTimeChanged_);
    outputDict_.set("oldEndTime", oldEndTime_);
    outputDict_.set("deltaTChanged", deltaTChanged_);
    outputDict_.set("oldDeltaT", oldDeltaT_);
    outputDict_.set("writeIntervalChanged", writeIntervalChanged_);

    forAll(triggers_, triggerI)
    {
        outputDict_.set
        (
            triggers_[triggerI].name(),
            triggers_[triggerI].writeOutputDict()
        );
    }
}


Foam::label Foam::plcEmulator::findAssociatedAlgorithm
(
    label solverDomainIndex
)
{
    forAll(algorithmGroups_, agIndex)
    {
        forAll(algorithmGroups_[agIndex], sdIndex)
        {
            if (algorithmGroups_[agIndex][sdIndex] == solverDomainIndex)
            {
                return agIndex;
            }
        }
    }
    FatalErrorIn("plcEmulator::findAssociatedAlgorithm")
        << "Cannot find an algorithm associated with solver domain "
        << solverDomains_[solverDomainIndex]
        << abort(FatalError);
        
    // dummy
    return -1;
}

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::plcEmulator::plcEmulator
(
    multiSolver& multiRun,
    word inputDictName,
    word outputDictName
)
:
    multiRun_(multiRun),

    plcEmulatorDict_
    (
        IOobject
        (
            inputDictName,
            multiRun_.multiDictRegistry().constant(),
            multiRun_.multiDictRegistry(),
            IOobject::MUST_READ,
            IOobject::NO_WRITE
        )
    ),

    outputDict_(),
    outputDictName_(outputDictName),
    outputDictRead_(false),

    triggers_(0),
    solverDomains_(multiRun.prefixes()),
    algorithms_(0),
    algorithmGroups_(0),

    currentSolverDomain_(-1),
    currentAlgorithm_(-1),

    nextTimerEnd_(VGREAT),
    endTimeChanged_(false),
    oldEndTime_(0.0),
    deltaTChanged_(false),
    oldDeltaT_(0.0),
    writeIntervalChanged_(false)
{
    // Read input dictionary
    readInputDict();

    word solverDomainName(multiRun.currentSolverDomain());
    if (solverDomainName == "default")
    {
        FatalErrorIn("plcEmulator::plcEmulator")
            << "Constructing plcEmulator when multiSolver does not have a "
            << "solver domain set."
            << abort(FatalError);
    }
    forAll(solverDomains_, sdi)
    {
        if (solverDomainName == solverDomains_[sdi])
        {
            currentSolverDomain_ = sdi;
            currentAlgorithm_ = findAssociatedAlgorithm(currentSolverDomain_);
            break;
        }
    }
    if (currentSolverDomain_ < 0)
    {
        FatalErrorIn("plcEmulator::plcEmulator")
            << "Initial solver domain " << solverDomainName << " is not "
            << "recognized. Available solverDomainNames are: "
            << solverDomains_
            << abort(FatalError);
    }
}

// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

void Foam::plcEmulator::signal(word signalName)
{
    forAll(solverSignalTriggers_, triggerI)
    {
        if (solverSignalTriggers_[triggerI].name() == signalName)
        {
            solverSignalTriggers_[triggerI].toggle();
            return;
        }
    }
    WarningIn("plcEmulator::signal")
        << "Received unrecognised solver signal " << signalName << ". Signal "
        << "is being ignored."
        << endl;
    return;
}


void Foam::plcEmulator::signal(word signalName, bool newValue)
{
    forAll(solverSignalTriggers_, triggerI)
    {
        if (solverSignalTriggers_[triggerI].name() == signalName)
        {
            solverSignalTriggers_[triggerI].set(newValue);
            return;
        }
    }
    WarningIn("plcEmulator::signal")
        << "Received unrecognised solver signal " << signalName << ". Signal "
        << "is being ignored."
        << endl;
    return;
}


void Foam::plcEmulator::timerExpired()
{
    timerExpired_ = true;
}


bool Foam::plcEmulator::initializeAndAdjustTime(Time& runTime)
{
    // clear flags
    endTimeChanged_ = false;
    deltaTChanged_ = false;
    writeIntervalChanged_ = false;

    if (!outputDictRead_)
    {
        readOutputDict(runTime);
        updateTriggers(runTime);
        adjustTime(runTime);
        if (applyLogic(runTime))
        {
            runTime.writeAndEnd();
            return true;
        }
        return false;
    }

    updateTriggers(runTime);
    adjustTime(runTime);
    return false;
}


void Foam::plcEmulator::updateTriggers(const Time& runTime)
{
    boolListList states(0);
    label iteration(-1);
    
    while (true)
    {
        iteration++;
        states.setSize(iteration + 1);
        states[iteration].setSize(triggers_.size());

        // Evaluate triggers
        forAll(triggers_, triggerI)
        {
            states[iteration][triggerI] =
                triggers_[triggerI].evaluate(runTime);
        }
        
        // Comparing against last iteration
        if ((iteration > 0) && (states[iteration] == states[iteration - 1]))
        {
            // Triggers have stabilized
            break;
        }
        
        // Comparing against all but last iteration
        for (label compIndex(0); compIndex < iteration - 1; compIndex++)
        {
            if (states[compIndex] == states[iteration])
            {
                Info << "Infinite loop detected. Check your logic. Current "
                    << "state:" << endl;
                displayCurrentState();
                FatalErrorIn("plcEmulator::updateTriggers")
                    << "Infinite loop detected, see Info message above."
                    << abort(FatalError);
            }
        }
    }
    updateOutputDict();
}


bool Foam::plcEmulator::applyLogic(Time& runTime)
{
    // For debug output purposes only
    word chosenLogic(word::null);

    bool returnMe(false);
    
    forAll(logic_, logicI)
    {
        if (logic_[logicI].test())
        {
            chosenLogic = logic_[logicI].name();
            label newSolverDomain(logic_[logicI].solverDomain());
            if (currentSolverDomain_ != newSolverDomain)
            {
                currentSolverDomain_ = newSolverDomain;
                currentAlgorithm_ = findAssociatedAlgorithm(currentSolverDomain_);
                returnMe = true;
            }
            break;
        }
    }

    if (!returnMe)
    {
        // If the solverDomain doesn't change and a timer has expired,
        // we have to reset endTime, deltaT, and timer flags
        if (timerExpired_)
        {
            timerExpired_ = false;

            // Timer has expired
            if (endTimeChanged_)
            {
                runTime.setEndTime(oldEndTime_);
                endTimeChanged_ = false;
                
                if (debug)
                {
                    Info << "Resetting endTime to " << oldEndTime_ << endl;
                }
            }
            if (deltaTChanged_)
            {
                runTime.setDeltaT(oldDeltaT_);
                deltaTChanged_ = false;

                if (debug)
                {
                    Info << "Resetting deltaT to " << oldDeltaT_ << endl;
                }
            }
            if (writeIntervalChanged_)
            {
                scalar newInterval
                (
                    readScalar(runTime.controlDict().lookup("writeInterval"))
                );
                runTime.setWriteInterval(newInterval);
                writeIntervalChanged_ = false;
                
                if (debug)
                {
                    Info << "Resetting writeInterval to " << newInterval
                        << endl;
                }
            }
            adjustTime(runTime);
        }
    }

    if (debug)
    {
        Info << "Current state of plcTriggers is:" << endl;
        displayCurrentState();
        if (chosenLogic == word::null)
        {
            Info << "Result: no conditions met, staying with previous state:"
                << endl;
        }
        else
        {
            Info << "Result: logic '" << chosenLogic << "' conditions have "
                << "been met:" << endl;
        }
        Info << token::TAB << "solver domain = '" << currentSolverDomainName()
            << "'" << token::NL << token::TAB << "algorithm = '"
            << currentAlgorithmName() << "'" << endl;
    }

    //resetTimers(runTime);

    return returnMe;
}


bool Foam::plcEmulator::adjustTime(Time& runTime)
{
    bool returnMe(false);
    nextTimerEnd_ = VGREAT;
    nextTimerEndRoundOff_ = 0.0;
    
    // Find next timer to expire
    forAll(timerTriggers_, triggerI)
    {
        const timerPlcTrigger& trigger(timerTriggers_[triggerI]);
        if
        (
            (
                nextTimerEnd_ - nextTimerEndRoundOff_
              - trigger.globalEndTime() + trigger.roundOff()
            ) > 0
        )
        {
            nextTimerEnd_ = trigger.globalEndTime();
            nextTimerEndRoundOff_ = trigger.roundOff();
        }
    }
    scalar localEndTime(nextTimerEnd_ - multiRun_.globalTimeOffset());
    
    if
    (
        (
            runTime.endTime().value() - nextTimerEndRoundOff_ - localEndTime
        ) > 0.0
    )
    {
        // runTime needs to be changed
        returnMe = true;
        if (!endTimeChanged_)
        {
            oldEndTime_ = runTime.endTime().value();
            endTimeChanged_ = true;
        }

        if (debug)
        {
            Info << "Adjusting endTime from " << runTime.endTime().value()
                << " to " << localEndTime << endl;
        }

        runTime.setEndTime(localEndTime);
        multiRun_.endTime() = runTime.endTime().value();
        
        // did runTime override the change to endTime?
        if (mag(runTime.endTime().value() - localEndTime) > (VSMALL * 2))
        {
            WarningIn("plcEmulator::adjustTime")
                << "Attempted to set endTime to " << localEndTime << " to "
                << "meet a timer end, but endTime is "
                << runTime.endTime().value()
                << endl;
        }
    }

    // test if deltaT needs to change as well - we leave deltaT changes up
    // to the solver unless absolutely necessary
    if
    (
        (
            runTime.value() + runTime.deltaT().value()
          - nextTimerEndRoundOff_ - localEndTime
        ) > 0.0
    )
    {
        scalar newDeltaT(localEndTime - runTime.value());

        // test if write interval needs to change
        word writeControl(word::null);
        if (runTime.controlDict().found("writeControl"))
        {
            writeControl = word(runTime.controlDict().lookup("writeControl"));
        }
//scalar oldWriteInterval(GREAT);
//scalar newWriteInterval(0);
        if (writeControl == "adjustableRunTime")
        {
            // write interval needs to change
            scalar writeInterval(GREAT);
            if (runTime.controlDict().found("writeInterval"))
            {
                writeInterval = readScalar
                (
                    runTime.controlDict().lookup("writeInterval")
                );
            }
//oldWriteInterval = writeInterval;

            label outputIndex
            (
                (
                    (runTime.value() - runTime.startTime().value())
                  + 0.5 * runTime.deltaT().value()
                )
                / writeInterval
            );
/*Info << "outputIndex = " << outputIndex << endl;
Info << "newDeltaT = " << newDeltaT << endl;
Info << "runTime.value() = " << runTime.value() << endl;
Info << "runTime.startTime() = " << runTime.startTime().value() << endl;
Info << "runTime.deltaT() = " << runTime.deltaT().value() << endl;*/

            scalar newInterval
            (
                (newDeltaT + (runTime.value() - runTime.startTime().value()))
              / (outputIndex + 1)
            );
/*Info << "newInterval = " << newInterval << endl;
scalar timeToNextWrite = max
(
    0.0,
    (outputIndex + 1)*newInterval
  - (runTime.value() - runTime.startTime().value())
);
Info << "giving timeToNextWrite = " << timeToNextWrite << endl;

newWriteInterval = newInterval;*/
            if (debug)
            {
                Info << "Temporarily adjusting writeInterval from "
                    << writeInterval << " to " << newInterval << endl;
            }

            runTime.setWriteInterval(newInterval);
            writeIntervalChanged_ = true;
        }

        if (!deltaTChanged_)
        {
            oldDeltaT_ = runTime.deltaT().value();
            deltaTChanged_ = true;
        }

        if (debug)
        {
            Info << "Adjusting deltaT from " << runTime.deltaT().value()
                << " to " << newDeltaT << endl;
/*label outputIndex = label
(
    ((runTime.value() - runTime.startTime().value())
    + 0.5*runTime.deltaT().value())
    /newWriteInterval
);
scalar timeToNextWrite = max
(
    0.0,
    (outputIndex + 1)*newWriteInterval
  - (runTime.value() - runTime.startTime().value())
);
label nStepsToNextWrite = label
(
    timeToNextWrite/runTime.deltaT().value() - SMALL
) + 1;
scalar newDeltaT = timeToNextWrite/nStepsToNextWrite;
Info << "current Time = " << runTime.value() << endl;
Info << "outputIndex = " << outputIndex << endl;
Info << "timeToNextWrite = " << timeToNextWrite << endl;
Info << "nStepsToNextWrite = " << nStepsToNextWrite << endl;
Info << "newDeltaT = " << newDeltaT << endl;*/
        }

        runTime.setDeltaT(newDeltaT);
        multiRun_.deltaT() = runTime.deltaT().value();
        
        // did runTime override the change to deltaT?
        if (mag(runTime.deltaT().value() - newDeltaT) > (VSMALL * 2))
        {
            WarningIn("plcEmulator::adjustTime")
                << "Attempted to set deltaT to " << newDeltaT << " to "
                << "meet a timer end, but deltaT is "
                << runTime.deltaT().value() << endl;

/*label outputIndex = label
(
    ((runTime.value() - runTime.startTime().value())
    + 0.5*runTime.deltaT().value())
    /newWriteInterval
);
scalar timeToNextWrite = max
(
    0.0,
    (outputIndex + 1)*newWriteInterval
  - (runTime.value() - runTime.startTime().value())
);
label nStepsToNextWrite = label
(
    timeToNextWrite/runTime.deltaT().value() - SMALL
) + 1;
scalar newDeltaT = timeToNextWrite/nStepsToNextWrite;
Info << "outputIndex = " << outputIndex << endl;
Info << "timeToNextWrite = " << timeToNextWrite << endl;
Info << "nStepsToNextWrite = " << nStepsToNextWrite << endl;
Info << "newDeltaT = " << newDeltaT << endl;

Info << "runTime++ debugging:" << endl;
Info << "oldTimeValue = " << runTime.value() << endl;
word oldTimeName(runTime.dimensionedScalar::name());
Info << "oldTimeName = " << oldTimeName << endl;
scalar newTimeValue(runTime.value() + runTime.deltaT().value());
Info << "newTimeValue = " << newTimeValue << endl;
Info << "newTimeName = " << Time::timeName(newTimeValue) << endl;*/

        }
    }
    return returnMe;
}


void Foam::plcEmulator::updateAndWrite(Time& runTime)
{
    // control.updateAndWrite does this:
    //  * update all triggers
    //  * apply logic
    //      * if solverDomain changes, model.time().writeAndEnd();
    //      * if not, model.time().write();
    //  * check for new timers that may interrupt end time
    //      * adjust end time if necessary
    updateTriggers(runTime);

    // Create IOobject for output dictionary
    IOdictionary outputIODict
    (
        IOobject
        (
            outputDictName_,
            runTime.timeName(),
            runTime,
            IOobject::NO_READ,
            IOobject::AUTO_WRITE
        ),
        outputDict_
    );

    if (applyLogic(runTime))
    {
        runTime.writeAndEnd();
        multiRun_.endTime() = runTime.endTime().value();
    }
    else
    {
        adjustTime(runTime);
        runTime.write();
    }
}


void Foam::plcEmulator::displayCurrentState()
{
    forAll(triggers_, triggerI)
    {
        Info << token::TAB << triggers_[triggerI].name()
            << token::TAB << triggers_[triggerI].value()
            << endl;
    }
}

// ************************************************************************* //
