#pragma once

// https://tools.ietf.org/html/rfc1950 - ZLIB Compressed Data Format Specification version 3.3
// https://tools.ietf.org/html/rfc1951 - DEFLATE Compressed Data Format Specification version 1.3
// 
// A zlib stream has the following structure:
// 
//   Compression method - 4 bits, `1000` denotes the "deflate" compression method
//                        with a window size up to 32K. "deflate" is used by gzip and PNG.
//   Compression info - 4 bits, for "deflate" it is the base-2 logarithm of the LZ77 window size,
//                      minus 8. E.g. 7 indicates a 32K window size.
//   Check for compression method and info - 4 bits
//   Preset dictionary - 1 bit
//   Compression level - 2 bits
//     0 - fastest algorithm
//     1 - fast algorithm
//     2 - default algorithm
//     3 - slowest algorithm
//   Space - 1 bit
//   Dictionary ID - 4 bytes if dictionary is preset. Contain ADLER32 checksum of the dictionary.
//   Compressed data
//   Checksum - 4 bytes, ADLER32 checksum of the uncompressed data


// zlib_decompress:
//   read file bytes
//   assert(lower 4 bits = '1000') // 8
//   assert(upper 4 bits = '0111') // 7
//   assert(dictionary is not preset)
//   decompress
//   assert(checksum is correct)

