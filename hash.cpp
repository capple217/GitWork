#include "hash.hpp"

namespace fs = std::filesystem;

/*
 * Allocates a 20-byte buffer hash (20 = SHA_DIGEST_LENGTH)
 * SHA1() computes SHA-1 digest of the data buffer
 * Converts the 20 binary bytes into a lowercase hex std::string (40 hex chars)
 */
/* CAVEAT
 * SHA-1 has been historically used by Git BUT has known collision weaknesses.
 * For more serious developments on this project, SHA-256 or multiple algorithms
 * will be beneficial
 */
std::string sha1_hex(const std::string &data) {
  unsigned char hash[SHA_DIGEST_LENGTH];
  SHA1(reinterpret_cast<const unsigned char *>(data.c_str()), data.length(),
       hash);
  std::ostringstream ss;
  for (int i = 0; i < SHA_DIGEST_LENGTH; ++i) {
    ss << std::hex << std::setw(2) << std::setfill('0')
       << static_cast<int>(hash[i]);
  }

  return ss.str();
}

/*
 * CompressBound(n) returns an upper bound on the size of the compressed data
 * for input length n We use them to allocate out Compress(dest, &destLen, src,
 * srcLen) compresses src into dest On success, desLen is set to actual
 * compressed length Resize out down to actual destLen and return
 */
/*
 * We don't get a specific error detail, we throw a generic runtime_error
 * We could map zlib error codes and include messages
 */
/*
std::vector<unsigned char>
compress_zlib(const std::vector<unsigned char> &input) {
  uLongf destLen = compressBound(input.size());
  std::vector<unsigned char> out(destLen);
  if (compress(out.data(), &destLen, input.data(), input.size()) != Z_OK)
    throw std::runtime_error("zlib compress failed");

  out.resize(destLen);
  return out;
}
*/
/*
 * ifstream: open the file in binary mode for reading. No error check, we should
 * check if (!in) and throw an error
 */

/*
 * vector content: Reads the whole file into a vector<unsigned char> using input
 * interators
 */

/*
 * hdr: Builds the Git header; for blobs, the exact header is "blob <size>\0"
 * where size is the decimal byte count
 */

/*
 * store.insert(...) twice to concatenate header and file bytes into store (the
 * data we will hash and compress)
 */

/*
 * We then consturctthe object directory path;
 * The first two hex chars become the directory name - object_dir
 * Remaining chars are the filename - object_path
 */

/*
 * We create the directory (if needed)
 * ofstream out: write compressed bytes to object file.
 * Again, no check for out being successful, should be added
 */

/*
 * Git stores loose objects by splitting the SHA into objects/XX/YYYY... so
 * directories are not overloaded with many filed and looking up by prefix is
 * easy
 */
/*
std::string write_blob_and_get_sha(const fs::path &repo_dir,
                                   const fs::path &filepath) {
  // read file
  std::ifstream in(filepath, std::ios::binary);
  std::vector<unsigned char> content((std::istreambuf_iterator<char>(in)), {});

  // header "blob <size>\0"
  std::string hdr = "blob " + std::to_string(content.size()) + '\0';
  std::vector<unsigned char> store;
  store.insert(store.end(), hdr.begin(), hdr.end());
  store.insert(store.end(), content.begin(), content.end());

  std::string sha = sha1_hex(store);
  auto compressed = compress_zlib(store);

  // write to .mygit/objects/xx/xxxx...
  fs::path object_dir = repo_dir / "objects" / sha.substr(0, 2);
  fs::create_directories(object_dir);
  fs::path object_path = object_dir / sha.substr(2);
  std::ofstream out(object_path, std::ios::binary);
  out.write(reinterpret_cast<const char *>(compressed.data()),
            compressed.size());
  return sha;
}
*/
