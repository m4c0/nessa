#pragma leco app
import nessa;
import siaudio;
import sith;
import sitime;

using namespace nessa::midi;

class thread : sith::thread {
  static constexpr const auto bpm = 140.0;
  static constexpr const auto bps = bpm / 60.0;
  static constexpr const auto notes_per_beat = 2.0;
  static constexpr const auto ms_per_note = 1000.0 / (bps * notes_per_beat);

  static constexpr const auto note_count = 32;
  static constexpr const midi_note inst_1[note_count] = {
      E4, EXTEND, B3, C4,     D4, EXTEND, C4,   B3,     //
      A3, EXTEND, A3, C4,     E4, EXTEND, D4,   C4,     //
      B3, EXTEND, B3, C4,     D4, EXTEND, E4,   EXTEND, //
      C4, EXTEND, A3, EXTEND, A3, EXTEND, MUTE, MUTE,   //
  };
  static constexpr const midi_note inst_2[note_count] = {
      MUTE, MUTE,   Ab3,  MUTE, MUTE, MUTE,   A3,   Ab3,  //
      E3,   EXTEND, MUTE, MUTE, MUTE, MUTE,   MUTE, MUTE, //
      Ab3,  EXTEND, Ab3,  A3,   B3,   EXTEND, MUTE, MUTE, //
      MUTE, MUTE,   MUTE, MUTE, MUTE, MUTE,   MUTE, MUTE, //
  };
  static constexpr const midi_note inst_3[note_count] = {
      E3,  E4,  E3,  E4,  E3,  E4,  E3,  E4,  //
      A2,  A3,  A2,  A3,  A2,  A3,  A2,  A3,  //
      Ab2, Ab3, Ab2, Ab3, Ab2, Ab3, Ab2, Ab3, //
      A2,  A3,  A2,  A3,  A2,  A3,  A2,  A3,  //
  };
  static constexpr const midi_note inst_4[note_count] = {
      MUTE, C5, MUTE, C5, MUTE, C5, MUTE, C5, //
      MUTE, C5, MUTE, C5, MUTE, C5, MUTE, C5, //
      MUTE, C5, MUTE, C5, MUTE, C5, MUTE, C5, //
      MUTE, C5, MUTE, C5, MUTE, C5, MUTE, C5, //
  };

  streamer m_streamer{};

  void play(midi_note s1, midi_note s2, midi_note sq, midi_note n) {
    m_streamer.producer().set_notes({s1, s2, sq, n});
    sitime::sleep_ms(ms_per_note);
  }

public:
  using sith::thread::start;

  void run();
};

void thread::run() {
  for (auto i = 0; i < note_count; i++) {
    play(inst_1[i], inst_2[i], inst_3[i], inst_4[i]);
  }
  play(MUTE, MUTE, MUTE, MUTE);
}
int main() {
  thread t{};
  t.start();

  sitime::sleep(10);
}
