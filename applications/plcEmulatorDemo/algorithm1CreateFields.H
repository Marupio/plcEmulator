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

    Info<< "Reading ascent rate\n" << endl;

    dimensionedScalar ascentRate
    (
        randomDict.lookup("ascentRate")
    );

