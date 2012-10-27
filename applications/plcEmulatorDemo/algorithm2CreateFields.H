    Info<< "Reading field T0\n" << endl;

    volScalarField T0
    (
        IOobject
        (
            "T0",
            runTime.timeName(),
            mesh,
            IOobject::MUST_READ,
            IOobject::AUTO_WRITE
        ),
        mesh
    );

    Info<< "Reading field T1\n" << endl;

    volScalarField T1
    (
        IOobject
        (
            "T1",
            runTime.timeName(),
            mesh,
            IOobject::MUST_READ,
            IOobject::AUTO_WRITE
        ),
        mesh
    );

    Info<< "Reading randomDict\n" << endl;

    IOdictionary randomDict
    (
        IOobject
        (
            "randomDict",
            runTime.constant(),
            mesh,
            IOobject::MUST_READ,
            IOobject::NO_WRITE
        )
    );

    Info<< "Reading ascent rates\n" << endl;

    dimensionedScalar ascentRate0
    (
        randomDict.lookup("ascentRate0")
    );
    dimensionedScalar ascentRate1
    (
        randomDict.lookup("ascentRate1")
    );

