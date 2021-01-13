"""Configuration classes for sensitive detectors"""

from LDMX.SimCore import simcfg

class ScoringPlaneSD(simcfg.SensitiveDetector) :
    def __init__(self,subsystem) :
        super().__init__(f'{subsystem}_sp','simcore::ScoringPlaneSD')

        self.collection_name = f'{subsystem.capitalize()}ScoringPlaneHits'
        self.match_substr = f'sp_{subsystem.lower()}' #depends on gdml

    def ecal() :
        return ScoringPlaneSD('ecal')

    def hcal()
        return ScoringPlaneSD('hcal')

    def target()
        return ScoringPlaneSD('target')

    def magnet()
        return ScoringPlaneSD('magnet')

class TrackerSD(simcfg.SensitiveDetector) :
    def __init__(self,subsystem,subdet_id) :
        super().__init__(f'{subsystem}_TrackerSD','simcore::TrackerSD')

        self.subsystem = subsystem
        self.subdet_id = subdet_id

        self.collection_name = f'{subsystem}SimHits'

    def tagger() :
        return TrackerSD('Tagger',1)

    def recoil() :
        return TrackerSD('Recoil',4)

class HcalSD(simcfg.SensitiveDetector) :
    def __init__(self) :
        super().__init__('hcal_sd', 'simcore::HcalSD')
