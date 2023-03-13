import nessa;
import siaudio;

enum midi_note {
  MUTE = -1,
  C3 = 48,
  Db3,
  D3,
  Eb3,
  E3,
  F3,
  Gb3,
  G3,
  Ab3,
  A3,
  Bb3,
  B3,
  C4,
  Db4,
  D4,
  Eb4,
  E4,
  F4,
  Gb4,
  G4,
  Ab4,
  A4,
  Bb4,
  B4,
  C5,
  Db5,
  D5,
  Eb5,
  E5,
  F5,
  Gb5,
  G5,
  Ab5,
  A5,
  Bb5,
  B5,
  C6,
  Db6,
  D6,
  Eb6,
  E6,
  F6,
  Gb6,
  G6,
  Ab6,
  A6,
  Bb6,
  B6,
};
static_assert(A4 == 69);
static_assert(B6 == 95);

static constexpr const float note_freqs[] = {
    16.351597831287414, 17.323914436054505,
    18.354047994837977, 19.445436482630058,
    20.601722307054366, 21.826764464562746,
    23.12465141947715,  24.499714748859326,
    25.956543598746574, 27.5,
    29.13523509488062,  30.86770632850775,
    32.70319566257483,  34.64782887210901,
    36.70809598967594,  38.890872965260115,
    41.20344461410875,  43.653528929125486,
    46.2493028389543,   48.999429497718666,
    51.91308719749314,  55.0,
    58.27047018976124,  61.7354126570155,
    65.40639132514966,  69.29565774421802,
    73.41619197935188,  77.78174593052023,
    82.4068892282175,   87.30705785825097,
    92.4986056779086,   97.99885899543733,
    103.82617439498628, 110.0,
    116.54094037952248, 123.47082531403103,
    130.8127826502993,  138.59131548843604,
    146.8323839587038,  155.56349186104046,
    164.81377845643496, 174.61411571650194,
    184.9972113558172,  195.99771799087463,
    207.65234878997256, 220.0,
    233.08188075904496, 246.94165062806206,
    261.6255653005986,  277.1826309768721,
    293.6647679174076,  311.1269837220809,
    329.6275569128699,  349.2282314330039,
    369.9944227116344,  391.99543598174927,
    415.3046975799451,  440.0,
    466.1637615180899,  493.8833012561241,
    523.2511306011972,  554.3652619537442,
    587.3295358348151,  622.2539674441618,
    659.2551138257398,  698.4564628660078,
    739.9888454232688,  783.9908719634985,
    830.6093951598903,  880.0,
    932.3275230361799,  987.7666025122483,
    1046.5022612023945, 1108.7305239074883,
    1174.6590716696303, 1244.5079348883237,
    1318.5102276514797, 1396.9129257320155,
    1479.9776908465376, 1567.981743926997,
    1661.2187903197805, 1760.0,
    1864.6550460723597, 1975.533205024496,
    2093.004522404789,  2217.4610478149766,
    2349.31814333926,   2489.0158697766474,
    2637.02045530296,   2793.825851464031,
    2959.955381693075,  3135.9634878539946,
    3322.437580639561,  3520.0,
    3729.3100921447194, 3951.066410048992,
    4186.009044809578,  4434.922095629953,
    4698.63628667852,   4978.031739553295,
    5274.04091060592,   5587.651702928062,
    5919.91076338615,   6271.926975707989,
    6644.875161279122,  7040.0,
    7458.620184289437,  7902.132820097988,
};
static constexpr const auto C0_MIDI_ID = 12;
static_assert(note_freqs[A4 - C0_MIDI_ID] == 440.0f);

class mixer {
  nessa::gen::square m_sq1;

public:
  [[nodiscard]] float operator()(float t) const noexcept {
    constexpr const auto volume = 0.5f;
    return m_sq1(t) * volume;
  }

  [[nodiscard]] constexpr auto &square_1() noexcept { return m_sq1; }
};

class player {
  mixer m{};
  volatile unsigned m_index;

  [[nodiscard]] float time(unsigned idx) const noexcept {
    constexpr const auto frate = static_cast<float>(siaudio::os_streamer::rate);
    return static_cast<float>(idx) / frate;
  }

public:
  void operator()(float *buf, unsigned len) {
    auto idx = m_index;
    for (auto i = 0; i < len; ++i, ++idx) {
      *buf++ = m(time(idx));
    }
    m_index = idx;
  }

  [[nodiscard]] auto current_note_index() const noexcept {
    constexpr const float bpm = 140.0;
    constexpr const float bps = bpm / 60.0;
    constexpr const float notes_per_beat = 2; // 4/4?
    return static_cast<unsigned>(time(m_index) * bps * notes_per_beat);
  }

  void set_note(midi_note n) noexcept {
    m.square_1().set_freq(note_freqs[n]);
    m.square_1().set_duty_cycle(0.25);
  }
};

extern "C" auto *poc_start() {
  static siaudio::streamer s{player{}};
  return &s;
}
extern "C" bool poc_loop() {
  auto &p = poc_start()->producer();
  switch (p.current_note_index()) {
  case 1:
    p.set_note(E4);
    return true;
  case 3:
    p.set_note(B3);
    return true;
  case 4:
    p.set_note(C4);
    return true;
  case 5:
    p.set_note(D4);
    return true;
  case 7:
    p.set_note(C4);
    return true;
  case 8:
    p.set_note(B3);
    return true;
  case 9:
    p.set_note(A3);
    return true;
  case 11:
    p.set_note(A3);
    return true;
  case 12:
    p.set_note(C4);
    return true;
  case 13:
    p.set_note(E4);
    return true;
  case 15:
    p.set_note(D4);
    return true;
  case 16:
    p.set_note(C4);
    return true;
  case 17:
    p.set_note(B3);
    return true;
  case 18:
    p.set_note(B3);
    return true;
  case 20:
    p.set_note(C4);
    return true;
  case 22:
    p.set_note(D4);
    return true;
  case 24:
    p.set_note(E4);
    return true;
  case 26:
    p.set_note(C4);
    return true;
  case 28:
    p.set_note(A3);
    return true;
  case 30:
    p.set_note(A3);
    return true;
  case 32:
    return false;
  default:
    return true;
  }
}

int main() {
  auto s = poc_start();
  while (poc_loop()) {
  }
}
