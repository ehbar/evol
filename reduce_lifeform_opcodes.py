#!/usr/bin/python2
'''
This is a script to determine the choices a lifeform's evolved Dna opcodes will
make under all combinations of conditions.  It takes a JSON dump from evol as
input and produces a simple text analysis of each Dna group listed therein.

Part of Evol: The non-life evolution simulator.
Copyright 2014-2015 Eric Barrett <arctil@gmail.com>.

This program is distributed under the terms of the GNU General Public
License Version 3.  See file `COPYING' for details.
'''

import errno
import itertools
import json
import sys


class ArenaCondition (object):
    '''
    Represents a lifeform's local condition on the arena; specifically whether
    it has neighbors, where they are, etc.
    '''
    FLAGS = 'NSEWCG'

    def __init__(self, conditions):
        '''
        Instantiate the ArenaCondition using the given conditions as a string
        of letters:
        'N' = north occpuied
        'S' = south occupied
        'E' = east occupied
        'W' = west occupied
        'C' = is crowded
        'G' = is neighbor (implied by N/S/E/W also, but not C)
        '''
        self.north = 'N' in conditions
        self.south = 'S' in conditions
        self.east = 'E' in conditions
        self.west = 'W' in conditions
        self.crowded = 'C' in conditions
        self.neighbor = 'G' in conditions or self.north or self.south or \
                self.east or self.west
        # Recompute flags so they're in standard order:
        self.flags = ''
        if self.north: self.flags += 'N'
        if self.south: self.flags += 'S'
        if self.east: self.flags += 'E'
        if self.west: self.flags += 'W'
        if self.crowded: self.flags += 'C'
        if self.neighbor: self.flags += 'G'

    def North(self): return self.north
    def South(self): return self.south
    def East(self): return self.east
    def West(self): return self.west
    def Neighbor(self): return self.neighbor
    def Crowded(self): return self.crowded

    def Flags(self):
        return self.flags

    def __str__(self):
        return self.flags

    def __eq__(self, other):
        return self.north == other.north and self.south == other.south and \
                self.east == other.east and self.west == other.west and \
                self.crowded == other.crowded and \
                self.neighbor == other.neighbor

    def __hash__(self):
        '''
        Supply a hash value for set membership.  Since we construct the flags
        attribute in a fixed order, we can use its hash.
        '''
        return hash(self.flags)


# Precompute all possible arena conditions for a lifeform to have in a single
# turn.  We take all mathematical combinations of the condition flags with
# sizes 0..len(flags), and then append the final condition with all flags set.
#
ALL_CONDITION_FLAGS = []
for ln in xrange(0, len(ArenaCondition.FLAGS)):
    for c in itertools.combinations(ArenaCondition.FLAGS, ln):
        ALL_CONDITION_FLAGS.append(c)
ALL_CONDITION_FLAGS.append(ArenaCondition.FLAGS)

# Many of the combinations of 'NSEWCG' will end up being duplicates because
# having a N/S/E/W neighbor implies the G (neighbor) flag, but not vice-versa.
# (The ArenaConditions object sets the flag correctly.)  Filter these throuth a
# frozenset to eliminate them.
#
ALL_CONDITIONS = list(frozenset([ArenaCondition(c)
                                 for c in ALL_CONDITION_FLAGS]))

# Now sort conditions by length, then alphabetically, for better display.
#
ALL_CONDITIONS.sort(lambda a, b: cmp(len(str(a)), len(str(b))) or
                                 cmp(str(a), str(b)))


class LifeformError (Exception):
    '''
    Base exception type for the Lifeform class.
    '''
    pass

class LifeformParseError (LifeformError):
    '''
    Raised by the Lifeform constructor if keys are missing or unconvertable
    from the unserialized JSON.
    '''
    pass

class LifeformDnaError (LifeformError):
    '''
    Raised by the Lifeform Dna parser if an unknown opcode is used.
    '''


class Lifeform (object):
    def __init__(self, raw):
        '''
        Initialize the lifeform from the structures recently unserialized
        from JSON.  Raises LifeformParseError if required structures aren't
        present.
        '''
        try:
            self.id = int(raw['id'])
            self.gen = int(raw['gen'])
            self.alive = bool(raw['alive'])
            self.energy = float(raw['energy'])
        except (KeyError, ValueError) as e:
            raise LifeformParseError('Missing basic stats from lifeform')

        self.dna = raw.get('dna')
        if self.dna is None:
            raise LifeformParseError('Missing dna from lifeform')

    def run_dna(self, conditions):
        '''
        Run the lifeform's DNA under the given condition(s) and returns
        its choise.
        '''
        pc = -1
        cmp = False
        while pc < len(self.dna):
            pc += 1
            if pc >= len(self.dna):
                return 'NOTHING'

            opcode = self.dna[pc]
            if opcode == 'NOP':
                continue
            elif opcode == 'APOPTOSIS' or opcode.startswith('FINAL'):
                return opcode
            elif opcode == 'IS_CROWDED':
                cmp = conditions.Crowded()
            elif opcode == 'IS_NEIGHBOR':
                cmp = conditions.Neighbor()
            elif opcode == 'IS_SOUTH_OCCUPIED':
                cmp = conditions.South()
            elif opcode == 'IS_WEST_OCCUPIED':
                cmp = conditions.West()
            elif opcode == 'IS_NORTH_OCCUPIED':
                cmp = conditions.North()
            elif opcode == 'IS_EAST_OCCUPIED':
                cmp = conditions.East()
            elif opcode.startswith('JMP'):
                pc += int(opcode[3])  # e.g. JMP3 -> pc += 3
            elif opcode.startswith('CJMP'):
                if cmp:
                    pc += int(opcode[4])  # e.g. CJMP4 -> pc += 4
            else:
                raise LifeformDnaError('Unknown Dna opcode ' + str(opcode))
        # End of Dna processing reached with no action
        return 'NOTHING'


def print_usage_and_exit(*msg):
    '''
    Print the error message(s) given, a simple usage summary, and exit with
    shell code 255.
    '''
    for m in msg:
        print >>sys.stderr, 'FATAL: {0}'.format(m)
    print >>sys.stderr, 'USAGE: {0} <lifeform json dump>'.format(sys.argv[0])
    sys.exit(255)


def die(*msg):
    '''
    Print the error message(s) given and exit with shell code 1.
    '''
    for m in msg:
        print >>sys.stderr, 'FATAL: {0}'.format(m)
    sys.exit(1)


def analyze(lf):
    '''
    Calculate and print what the lifeform would do under various conditions.
    '''
    print 'Lifeform id {0}, gen {1}'.format(lf.id, lf.gen)
    for condition in ALL_CONDITIONS:
        print condition, '->', lf.run_dna(condition)


def main():
    if len(sys.argv) < 2:
        print_usage_and_exit("No file specified")

    lifeforms = []
    for file in sys.argv[1:]:
        with open(file, "r") as f:
            lifeforms.extend([Lifeform(l) for l in json.load(f)])

    print 'Loaded {0} lifeforms'.format(len(lifeforms))
    print

    try:
        for lf in lifeforms:
            analyze(lf)
            print
    except IOError as e:
        # Suppress pipe errors; quitting out of /bin/more & friends can cause
        # this, and it's annoying
        if e.errno != errno.EPIPE:
            raise

if __name__ == '__main__':
    main()
