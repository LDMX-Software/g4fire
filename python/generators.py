
##################################################################################
# @file generators.py
# Define some helpful functions and standardized generators for the simulation
#
# @author Tom Eichlersmith, University of Minnesota
##################################################################################

from LDMX.SimApplication import simcfg

#############################################################
# @function gun
# @param name name of ParticleGun
# @return PrimaryGenerator of class ldmx::ParticleGun
#############################################################
def gun( name ) :
    return simcfg.PrimaryGenerator( name , "ldmx::ParticleGun" )

#############################################################
# @function multi
# @param name name of MultiParticleGunPrimaryGenerator
# @return PrimaryGenerator of class ldmx::MultiParticleGunPrimaryGenerator
#############################################################
def multi( name ) :
    return simcfg.PrimaryGenerator( name , "ldmx::MultiParticleGunPrimaryGenerator" )

#############################################################
# @function lhe
# @param name name of LHEPrimaryGenerator
# @param filePath LHE file to use
# @return PrimaryGenerator of class ldmx::LHEPrimaryGenerator with filePath parameter set
#############################################################
def lhe( name , filePath ) :
    sim = simcfg.PrimaryGenerator( name , "ldmx::LHEPrimaryGenerator" )
    sim.parameters[ "filePath" ] = filePath
    return sim

#############################################################
# @function completeReSim
# @param name name of RootCompleteReSim
# @param filePath root file to re-sim
# @return PrimaryGenerator of class ldmx::RootCompleteReSim with parameters set to reasonable defaults
#############################################################
def completeReSim( name , filePath ) :
    sim = simcfg.PrimaryGenerator( name , "ldmx::RootCompleteReSim" )
    sim.parameters[ "filePath" ] = filePath
    sim.parameters[ "simParticleCollName" ] = "SimParticles"
    sim.parameters[ "simParticlePassName" ] = ""
    return sim

#############################################################
# @function ecalSP
# @param name name of RootSimFromEcalSP
# @param filePath root file to re-sim
# @return PrimaryGenerator of class ldmx::RootSimFromEcalSP with parameters set to reasonable defaults
#############################################################
def ecalSP( name , filePath ) :
    sim = simcfg.PrimaryGenerator( name , "ldmx::RootSimFromEcalSP" )
    sim.parameters[ "filePath" ] = filePath
    sim.parameters[ "ecalSPHitsCollName" ] = "EcalScoringPlaneHits"
    sim.parameters[ "ecalSPHitsPassName" ] = ""
    sim.parameters[ "timeCutoff" ] = 50.
    return sim

#############################################################
# @function gps
# @param name name of GeneralParticleSource
# @return PrimaryGenerator of class ldmx::GeneralParticleSource
#############################################################
def gps( name ) :
    return simcfg.PrimaryGenerator( name , "ldmx::GeneralParticleSource" )


def single_4gev_e_upstream_tagger() :
    """Configure a particle gun to fire a 4 GeV electron upstream of the tagger tracker.

    The position and direction are set such that the electron will be bent by 
    the field and arrive at the target at approximately [0, 0, 0] (assuming 
    it's not smeared).
    
    Returns
    -------
    Instance of a particle gun configured to fire a single 4 Gev electron 
    directly upstream of the tagger tracker.  

    """

    particle_gun = gun('single_4gev_e_upstream_tagger')
    particle_gun.parameters[ 'particle'  ] = 'e-'
    particle_gun.parameters[ 'position'  ] = [ -27.926 , 5 , -700 ] # mm
    particle_gun.parameters[ 'direction' ] = [ 313.8 / 4000 , 0, 3987.7/4000 ]
    particle_gun.parameters[ 'energy'    ] = 4.0000000 # GeV

    return particle_gun

def single_4gev_e_upstream_target() :
    """Configure a particle gun to fire a 4 GeV electron upstream of the tagger tracker.

    The position and direction are set such that the electron will be bent by 
    the field and arrive at the target at approximately [0, 0, 0] (assuming 
    it's not smeared).
    
    Returns
    -------
    Instance of a particle gun configured to fire a single 4 Gev electron 
    directly upstream of the tagger tracker.  

    """

    particle_gun = gun('single_4gev_e_upstream_target')
    particle_gun.parameters[ 'particle'  ] = 'e-'
    particle_gun.parameters[ 'position'  ] = [ 0., 0., -1.2 ] # mm
    particle_gun.parameters[ 'direction' ] = [ 0., 0., 1]
    particle_gun.parameters[ 'energy'    ] = 4.0000000 # GeV

    return particle_gun

#############################################################
# @function farUpstreamSingle1p2GeVElectron
# @return a ParticleGun with a single 1.2GeV electron fired from far upstream of target
#############################################################
def farUpstreamSingle1p2GeVElectron() :
    farUpstreamElectron = gun( "farUpstreamSingle1.2GeVElectron" )
    farUpstreamElectron.parameters[ 'particle'  ] = 'e-'
    farUpstreamElectron.parameters[ 'position'  ] = [ -36.387, 5, -700 ] #mm
    farUpstreamElectron.parameters[ 'direction' ] = [ 0.2292 / 1.2 , 0, 1.1779 / 1.2 ] #unitless
    farUpstreamElectron.parameters[ 'energy'    ] = 1.200000 #GeV
    return farUpstreamElectron
