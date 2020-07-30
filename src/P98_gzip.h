#include <zlib.h>

inline int decompress_bundle(const char* path,
			     void* buf,
			     int buflen) {
    gzFile in = gzopen(path, "rb");
    int result = gzread(in, (voidp)buf, buflen);
    gzclose(in);
    return result;
}
