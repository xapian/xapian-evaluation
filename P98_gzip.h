/* All the files in this Gunzip directory make a single object file
   gunzipper.o with the single external interface defined below.
   */


extern "C" int decompress_bundle(unsigned char *cifname, unsigned char *obuf, int obuflen);

/* cifname - path to file to be decompressed
   obuf - buffer into which file is decompressed
   obuflen - size of said buffer
*/
