"""A few simple detectors shipped with g4fire

Helpful as examples as well
"""

from cfg import DetectorConstruction

class GDML(DetectorConstruction) :
    """Parse the detector from GDML

    Parameters
    ----------
    file_path : str
        Path to the GDML file to start reading from
    """

    def __init__(self, file_path) :
        super().__init__('g4fire::plugins::detector_constructors::GDML',
                file_path = 'NOTGIVEN')
        self.file_path = file_path

class SimplePrism(DetectorConstruction) :
    """A single-material prism of the input dimensions

    Parameters
    ----------
    box_material : str
        Material to have in the box (as named in G4NistManager)
    world_material : str
        Material to have outside the box (as named in G4NistManager)
    box_dimensions_mm : list[float]
        The three dimensions [x,y,z] in mm of the box
    """

    def __init__(self, box_material, world_material, box_dimensions_mm) :
        super().__init__('g4fire::plugins::detector_constructors::SimplePrism',
                box_material = 'NOTGIVEN',
                world_material = 'NOTGIVEN',
                box_dimensions_mm = [10.,10.,10.])
        self.box_material = box_material
        self.world_material = world_material
        self.box_dimensions_mm = box_dimensions_mm
