"""Configuration module for g4fire user classes"""

from . import _parameters

class PrimaryGenerator(_parameters.Parameters) :
    def __init__(self, class_name, **kwargs) :
        super().__init__(class_name, **kwargs)

class UserAction(_parameters.Parameters) :
    def __init__(self, class_name, **kwargs) :
        super().__init__(class_name, **kwargs)

class DetectorConstruction(_parameters.Parameters) :
    def __init__(self, class_name, **kwargs) :
        super().__init__(class_name, **kwargs)

class SensitiveDetector(_parameters.Parameters) :
    def __init__(self, class_name, name, **kwargs) :
        super().__init__(class_name, name = name, **kwargs)

class BiasingOperator(_parameters.Parameters) :
    def __init__(self, class_name, name, **kwargs) :
        super().__init__(class_name, name = name, **kwargs)

class PhysicsConstructor(_parameters.Parameters) :
    def __init__(self, class_name, name, **kwargs) :
        super().__init__(class_name, name = name, **kwargs)

