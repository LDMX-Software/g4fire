"""Primary Generator templates for use throughout g4fire

Mainly focused on reducing the number of places that certain parameter and class
names are hardcoded into the python configuration.
"""

from cfg import PrimaryGenerator

class ParticleGun(PrimaryGenerator):
    """New basic particle gun primary generator

    Attributes
    ----------
    time : float, optional
        Time to shoot from [ns]
    verbosity : int, optional
        Verbosity flag for this generator
    particle : str
        Geant4 particle name to shoot
    energy : float
        Energy of particle to shoot [GeV]
    position : list of float
        Position to shoot from [mm]
    direction : list of float
        Unit vector direction to shoot from

    Examples
    --------
        four_gev_electrons = ParticleGun()
        four_gev_electrons.particle = 'e-'
        four_gev_electrons.energy = 4.0
    """
    def __init__(self):
        super().__init__(
                "g4fire::plugins::primary_generators::ParticleGun",
                particle = 'e-',
                energy = 4.0,
                time = 0.,
                verbosity = 0,
                position = [0.,0.,0.],
                direction = [0.,0.,1.]
                )

class MultiParticleGun(PrimaryGenerator):
    """New multi particle gun primary generator

    Attributes
    ----------
    enablePoisson : bool, optional
        Poisson-distribute number of particles?
    vertex : list of float
        Position to shoot particle(s) from [mm]
    momentum : list of float
        3-momentum to give particle(s) in [MeV]
    nParticles : int, optional
        Number of particles to shoot (or average of Poisson distribution)
    pdgID : int
        PDG ID of particle(s) to shoot
    """
    def __init__(self):
        super().__init__('g4fire::plugins::primary_generators::MultiParticleGunPrimaryGenerator',
                enablePoisson = False,
                vertex = [0.,0.,0.],
                momentum = [0.,0.,1.],
                nParticles = 1,
                pdgID = 11)

class LHEPrimaryGenerator(PrimaryGenerator):
    """New LHE file primary generator

    Parameters
    ----------
    filePath : str
        path to LHE file containing the primary vertices
    """
    def __init__(self, filePath):
        super().__init__('g4fire::plugins::primary_generators::LHEPrimaryGenerator',
                filePath = 'NONEGIVEN')
        self.filePath = filePath

class GeneralParticleSource(PrimaryGenerator):
    """New general particle source

    The input initialization commands are run in the order that they are listed.

    Parameters
    ----------
    initCommands : list of strings
        List of Geant4 commands to initialize this GeneralParticleSource

    Returns
    -------
    PrimaryGenerator
        configured to be a GeneralParticleSource with the passed initialization commands

    Examples
    --------
        myGPS = GeneralParticleSource([
            "/gps/particle e-",
            "/gps/pos/type Plane",
            "/gps/pos/shape Square",
            "/gps/pos/centre 0 0 0 mm",
            "/gps/pos/halfx 40 mm",
            "/gps/pos/halfy 80 mm",
            "/gps/ang/type cos",
            "/gps/ene/type Lin",
            "/gps/ene/min 3 GeV",
            "/gps/ene/max 4 GeV",
            "/gps/ene/gradient 1",
            "/gps/ene/intercept 1"
            ] )
    """
    def __init__(self, initCommands):
        super().__init__(name, 'g4fire::plugins::primary_generators::GeneralParticleSource',
                initCommands = [])
        self.initCommands = initCommands

