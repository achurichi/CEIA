class NoiseGate:
    def __init__(self, open, close, hold, samplerate):
        self.open = open**2
        self.close = close**2
        self.samples_hold = int(hold * samplerate)
        self.reset()

    def reset(self):
        self.open_counter = 0
        self.is_open = False

    def transform(self, sample):
        if sample >= self.open:
            self.is_open = True
            self.open_counter = 0
        elif self.is_open:
            if sample < self.close and self.open_counter >= self.samples_hold:
                self.is_open = False
            else:
                self.open_counter += 1

        return sample if self.is_open else None
