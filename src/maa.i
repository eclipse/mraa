// Now list ANSI C/C++ declarations
int get_version();

namespace maa {
class I2C {
public:
    I2C(unsigned int sda, unsigned int scl);
    void frequency(int hz);
    int read(int address, char *data, int length, bool repeated = false);
    int read(int ack);
    int write(int address, const char *data, int length, bool repeated = false);
    int write(int data);
    void start(void);
    void stop(void);
};
};
