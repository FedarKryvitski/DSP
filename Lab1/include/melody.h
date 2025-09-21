#ifndef MELODY_H
#define MELODY_H

#include <vector>

namespace Melody {

    struct NoteData {
        float frequency{0.f};
        float startBar{0.f};
        float length{0.f};
    };

    inline std::vector<NoteData> createSimpleMelody() {
        return {
            {Notes::C<4>, 0.f, 0.5f},
            {Notes::D<4>, 0.5f, 0.5f},
            {Notes::E<4>, 1.f, 0.5f},
            {Notes::F<4>, 1.5f, 0.5f},
            {Notes::G<4>, 2.f, 0.5f},
            {Notes::A<4>, 2.5f, 0.5f},
            {Notes::B<4>, 3.f, 0.5f},
        };
    }

    inline std::vector<NoteData> createComplexMelody() {
        return {
            {Notes::C<4>, 0.0f, 1.0f},
            {Notes::E<4>, 0.0f, 1.0f},
            {Notes::G<4>, 0.0f, 1.0f},

            {Notes::D<4>, 1.0f, 0.5f},
            {Notes::F<4>, 1.0f, 0.5f},
            {Notes::A<4>, 1.0f, 0.5f},

            {Notes::E<4>, 1.5f, 0.5f},
            {Notes::G<4>, 1.5f, 0.5f},
            {Notes::B<4>, 1.5f, 0.5f},

            {Notes::C<4>, 2.0f, 2.0f},
            {Notes::E<4>, 2.0f, 2.0f},
            {Notes::G<4>, 2.0f, 2.0f},

            {Notes::C<3>, 0.0f, 2.0f},
            {Notes::G<3>, 2.0f, 2.0f},

            {Notes::F<4>, 4.0f, 0.75f},
            {Notes::A<4>, 4.0f, 0.75f},
            {Notes::C<5>, 4.0f, 0.75f},

            {Notes::E<4>, 4.75f, 0.25f},
            {Notes::G<4>, 4.75f, 0.25f},
            {Notes::B<4>, 4.75f, 0.25f},

            {Notes::D<4>, 5.0f, 0.5f},
            {Notes::F<4>, 5.0f, 0.5f},
            {Notes::A<4>, 5.0f, 0.5f},

            {Notes::C<4>, 5.5f, 0.5f},
            {Notes::E<4>, 5.5f, 0.5f},
            {Notes::G<4>, 5.5f, 0.5f},

            {Notes::F<3>, 4.0f, 2.0f},
            {Notes::C<3>, 6.0f, 2.0f},

            {Notes::G<4>, 8.0f, 0.5f},
            {Notes::B<4>, 8.0f, 0.5f},
            {Notes::D<5>, 8.0f, 0.5f},

            {Notes::F<4>, 8.5f, 0.25f},
            {Notes::A<4>, 8.5f, 0.25f},
            {Notes::C<5>, 8.5f, 0.25f},

            {Notes::E<4>, 8.75f, 0.25f},
            {Notes::G<4>, 8.75f, 0.25f},
            {Notes::B<4>, 8.75f, 0.25f},

            {Notes::C<4>, 9.0f, 1.0f},
            {Notes::E<4>, 9.0f, 1.0f},
            {Notes::G<4>, 9.0f, 1.0f},

            {Notes::G<3>, 8.0f, 2.0f},

            {Notes::A<4>, 10.0f, 0.5f},
            {Notes::C<5>, 10.0f, 0.5f},
            {Notes::E<5>, 10.0f, 0.5f},

            {Notes::G<4>, 10.5f, 0.25f},
            {Notes::B<4>, 10.5f, 0.25f},
            {Notes::D<5>, 10.5f, 0.25f},

            {Notes::F<4>, 10.75f, 0.25f},
            {Notes::A<4>, 10.75f, 0.25f},
            {Notes::C<5>, 10.75f, 0.25f},

            {Notes::E<4>, 11.0f, 1.0f},
            {Notes::G<4>, 11.0f, 1.0f},
            {Notes::B<4>, 11.0f, 1.0f},

            {Notes::A<3>, 10.0f, 2.0f},

            {Notes::C<5>, 12.0f, 1.5f},
            {Notes::E<5>, 12.0f, 1.5f},
            {Notes::G<5>, 12.0f, 1.5f},

            {Notes::B<4>, 13.5f, 0.5f},
            {Notes::D<5>, 13.5f, 0.5f},
            {Notes::F<5>, 13.5f, 0.5f},

            {Notes::C<5>, 14.0f, 2.0f},
            {Notes::E<5>, 14.0f, 2.0f},
            {Notes::G<5>, 14.0f, 2.0f},

            {Notes::C<3>, 12.0f, 4.0f},

            {Notes::Ds<5>, 13.0f, 0.25f},
            {Notes::F<5>, 13.25f, 0.25f},
            {Notes::Gs<4>, 13.5f, 0.25f},
            {Notes::As<4>, 13.75f, 0.25f},

            {Notes::C<4>, 15.5f, 0.25f},
            {Notes::E<4>, 15.75f, 0.25f},
            {Notes::G<4>, 16.0f, 0.25f},
            {Notes::C<5>, 16.25f, 0.25f},
            {Notes::E<5>, 16.5f, 0.25f},
            {Notes::G<5>, 16.75f, 0.5f}
        };
    }

} // namespace Melody

#endif //MELODY_H
