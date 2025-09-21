#ifndef NOTES_H
#define NOTES_H

#include <cmath>

namespace Notes {

enum class Note : int {
    C = -9,
    Cs = -8,
    D = -7,
    Ds = -6,
    E = -5,
    F = -4,
    Fs = -3,
    G = -2,
    Gs = -1,
    A = 0,
    As = 1,
    B = 2,
};

template<Note note, int octave>
struct NoteFrequency {
    static float value() {
        constexpr float A4{440.f};
        constexpr int semitones_from_A4 =
            static_cast<int>(note) - static_cast<int>(Note::A) + 12 * (octave - 4);
        return A4 * std::pow(2.0f, semitones_from_A4 / 12.0f);
    }
};

template<int octave> float C = NoteFrequency<Note::C, octave>::value();
template<int octave> float Cs = NoteFrequency<Note::Cs, octave>::value();
template<int octave> float D = NoteFrequency<Note::D, octave>::value();
template<int octave> float Ds = NoteFrequency<Note::Ds, octave>::value();
template<int octave> float E = NoteFrequency<Note::E, octave>::value();
template<int octave> float F = NoteFrequency<Note::F, octave>::value();
template<int octave> float Fs = NoteFrequency<Note::Fs, octave>::value();
template<int octave> float G = NoteFrequency<Note::G, octave>::value();
template<int octave> float Gs = NoteFrequency<Note::Gs, octave>::value();
template<int octave> float A = NoteFrequency<Note::A, octave>::value();
template<int octave> float As = NoteFrequency<Note::As, octave>::value();
template<int octave> float B = NoteFrequency<Note::B, octave>::value();

} // namespace Notes

#endif //NOTES_H
