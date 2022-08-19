
import fire.cfg
p = fire.cfg.Process('sim')
p.event_limit = 10
p.log_frequency = 1
p.term_level = 0
p.output_file = f'basic_sim_{p.event_limit}_events.h5'
p.run = 1

from g4fire import simulator, generators, detector
sim = simulator('basic')
sim.description = 'Basic test simulation'
sim.detector_constructor = detector.SimplePrism(5,5,5,'lead')
sim.generators.append(generators.gun())

p.sequence = [ sim ]
