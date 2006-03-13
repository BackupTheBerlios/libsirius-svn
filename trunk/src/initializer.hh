#ifndef INITIALIZER_HH
#define INITIALIZER_HH

class Initializer
{
    public:
        static void init();

    private:
        Initializer();
        Initializer(const Initializer &);
        Initializer& operator=(const Initializer &);

        static Initializer& get_instance();
};

#endif

