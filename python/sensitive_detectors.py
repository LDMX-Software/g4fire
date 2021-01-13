"""Configuration classes for sensitive detectors"""

from LDMX.SimCore import simcfg

class ScoringPlaneSD(simcfg.SensitiveDetector) :
    def __init__(self,subsystem) :
        super().__init__(f'{subsystem}_sp','simcore::ScoringPlaneSD')

        self.collection_name = f'{subsystem.capitalize()}ScoringPlaneHits'
        self.match_substr = f'sp_{subsystem.lower()}' #depends on gdml

class EcalScoringPlanes(ScoringPlaneSD) :
    def __init__(self) :
        super().__init__('ecal')

class HcalScoringPlanes(ScoringPlaneSD) :
    def __init__(self) :
        super().__init__('hcal')

class TargetScoringPlanes(ScoringPlaneSD) :
    def __init__(self) :
        super().__init__('target')

class MagentScoringPlanes(ScoringPlaneSD) :
    def __init__(self) :
        super().__init__('magnet')
