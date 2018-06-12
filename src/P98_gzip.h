#include <zlib.h>

inline int decompress_bundle(const char* path,
			     void* buf,
			     int buflen) {
    gzFile in = gzopen(path, "rb");
    int result = gzfread((voidp)buf, 1, buflen, in);
    gzclose(in);
    return result;
}
