"""Package to help configure the simulation

Defines a derived class from fire.cfg.Processor
with several helpful member functions.
"""

from fire.cfg import Processor

class Simulator(Processor):
    """A instance of the simulation configuration

    This class is derived from ldmxcfg.Producer and is mainly
    focused on providing helper functions that can be used instead
    of accessing the parameters member directly.

    The parameters that are lists ('preInitCommands', 'postInitCommands', 'actions', and 'generators')
    are initialized as empty lists so that we can append to them later.

    The ECal hit conbtibutions are enabled and compressed by default.

    Parameters
    ----------
    instance_name : str
        Name of this instance of the Simulator

    Attributes
    ----------
    generators : list of PrimaryGenerator
        Generators to use to make primaries
    detector : str
        Full path to detector description gdml (suggested to use setDetector)
    validate_detector : bool, optional
        Should we have Geant4 validate that the gdml is correctly formatted?
    description : str
        Describe this run in a human-readable way
    scoringPlanes : str, optional
        Full path to the scoring planes gdml (suggested to use setDetector)
    beamSpotSmear : list of float, optional
        2 (x,y) or 3 (x,y,z) widths to smear ALL primary vertices by [mm]
    time_shift_primaries : bool
        Should we shift the times of primaries so that z=0mm corresponds to t=0ns? 
    enableHitContribs : bool, optional
        Should the simulation save contributions to Ecal sim hits?
    compressHitContribs : bool, optional
        Should the simulation compress contributions to Ecal sim hits by PDG ID?
    preInitCommands : list of str, optional
        Geant4 commands to run before the run is initialized
    postInitCommands : list of str, optional
        Geant4 commands to run after run is initialized (but before run starts)
    actions : list of UserAction, optional
        Special User-defined actions to take during the simulation
    biasing_operators : list of XsecBiasingOperators, optional
        Operators for biasing specific particles to undergo specific processes
    dark_brem : DarkBrem
        Configuration options for dark brem process
    logging_prefix : str, optional
        Prefix to prepend any Geant4 logging files
    rootPrimaryGenUseSeed : bool, optional
        Use the seed stored in the EventHeader for random generation
    verbosity : int, optional
        Verbosity level to print
    """
    def __init__(self, instance_name, detector, description, generators, 
                 scoring_planes='',
                 beam_spot_delta=[],
                 time_shift_primaries=True,
                 enable_hit_contribs=True,
                 compress_hit_contribs=True,
                 pre_init_cmds=[],
                 post_init_cmds=[],
                 actions=[],
                 biasing_operators=[],
                 logging_prefix='',
                 validate_detector=False,
                 verbosity = 0):
        super().__init__(instance_name,
                         "g4fire::Simulator",
                         detector=detector, 
                         description=description, 
                         generators=generators,
                         module="g4fire_simulator",
                         scoring_planes=scoring_planes,
                         beam_spot_delta=beam_spot_delta,
                         time_shift_primaries=time_shift_primaries,
                         enable_hit_contribs=enable_hit_contribs,
                         compress_hit_contribs=compress_hit_contribs,
                         pre_init_cmds=pre_init_cmds,
                         post_init_cmds=post_init_cmds,
                         actions=actions,
                         biasing_operators=biasing_operators,
                         logging_prefix=logging_prefix,
                         validate_detector=validate_detector,
                         verbosity=verbosity)

        #Dark Brem stuff
        #from LDMX.g4fire import dark_brem
        #self.dark_brem = dark_brem.DarkBrem()

        #def setDetector(self, det_name , include_scoring_planes = False ) :
        """Set the detector description with the option to include the scoring planes

        Parameters
        ----------
        det_name : str
            name of a detector in the Detectors module
        include_scoring_planes : bool
            True if you want to import and use scoring planes

        See Also
        --------
        @PYTHON_PACKAGE_NAME@.Detectors.makePath for definitions of the path making functions.
        """

        #from @PYTHON_PACKAGE_NAME@.Detectors import makePath as mP
        #self.detector = mP.makeDetectorPath( det_name )
        #if include_scoring_planes :
        #    self.scoringPlanes = mP.makeScoringPlanesPath( det_name )
