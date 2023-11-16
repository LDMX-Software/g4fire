
import fire.cfg
p = fire.cfg.Process('sim')
p.event_limit = 10
p.log_frequency = 1
p.term_level = 0
p.output_file = f'basic_sim_{p.event_limit}_events.h5'
p.run = 1

from g4fire import Simulator, generators, detectors
sim = Simulator('basic', detectors.SimplePrism('lead','air',[5,5,5]),
        generators.ParticleGun(), 'basic test sim')

p.sequence = [ sim ]
